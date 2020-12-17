/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "network.h"
#include "commands.h"
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3
#else
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
typedef int SOCKET;
#endif

#ifdef _WIN32
std::string net_get_dns(char* macAddress)
{
    std::string dnsSuffix = "";
    char dns[256];
    memset(dns, 0, 256);

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;
    outBufLen = WORKING_BUFFER_SIZE;

    // get info for all adapters
    do {

        pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
        if (pAddresses == NULL) {
            std::cout << "dns memory error" << std::endl;
            return dnsSuffix;
        }

        dwRetVal = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW)
        {
            free(pAddresses);
            pAddresses = NULL;
        }
        else
        {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    // get DNS from MAC
    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    pCurrAddresses = pAddresses;
    while (pCurrAddresses)
    {
        if (pCurrAddresses->PhysicalAddressLength != 0)
        {
            if (memcmp(macAddress, (char*)pCurrAddresses->PhysicalAddress, 6) == 0)
            {
                if (wcslen(pCurrAddresses->DnsSuffix) > 0)
                {
                    snprintf(dns, 256, "%ws", pCurrAddresses->DnsSuffix);
                    break;
                }
            }
        }

        pCurrAddresses = pCurrAddresses->Next;
    }

    dnsSuffix = dns;

    return dnsSuffix;
}

#else
std::string net_get_dns(char* macAddress)
{
    std::string dnsSuffix = "";

    // get socket
    SOCKET s = 0;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        std::cout << "couldn't get socket" << std::endl;
        return dnsSuffix;
    }

    // get info for all adapters
    struct ifconf ifc;
    memset(&ifc, 0, sizeof(ifconf));
    char buffer[8192];
    memset(buffer, 0, sizeof(buffer));

    ifc.ifc_buf = buffer;
    ifc.ifc_len = sizeof(buffer);
    if (ioctl(s, SIOCGIFCONF, &ifc) < 0)
    {
        std::cout << "ioctl SIOCGIFCONF failed" << std::endl;
        return dnsSuffix;
    }

    // get DNS from IP associated with MAC
    struct ifreq* ifr = ifc.ifc_req;
    int interfaceCount = ifc.ifc_len / sizeof(struct ifreq);

    char ip[INET6_ADDRSTRLEN] = { 0 };
    struct ifreq* item;
    struct sockaddr* addr;
    for (int i = 0; i < interfaceCount; i++)
    {
        item = &ifr[i];
        addr = &(item->ifr_addr);

        // get IP address
        if (ioctl(s, SIOCGIFADDR, item) < 0)
        {
            std::cout << "ioctl SIOCGIFADDR failed" << std::endl;
            continue;
        }

        if (inet_ntop(AF_INET, &(((struct sockaddr_in*)addr)->sin_addr),
            ip, sizeof(ip)) == NULL)
        {
            std::cout << "inet_ntop" << std::endl;
            continue;
        }

        // get MAC address
        if (ioctl(s, SIOCGIFHWADDR, item) < 0)
        {
            std::cout << "ioctl SIOCGIFHWADDR failed" << std::endl;
            continue;
        }

        if (memcmp(macAddress, (char*)item->ifr_hwaddr.sa_data, 6) == 0)
        {
            // Get host by using the IP address which AMT device is using
            struct in_addr addr = { 0 };
            struct hostent* host;
            addr.s_addr = inet_addr(ip);
            host = gethostbyaddr((char*)&addr, 4, AF_INET);
            if (host == NULL)
            {
                std::cout << "gethostbyaddr() failed";
                return dnsSuffix;
            }

            // strip off the hostname to get actual domain name
            int domainNameSize = 256;
            char domainName[domainNameSize];
            memset(domainName, 0, domainNameSize);

            char* dn = strchr(host->h_name, '.');
            if (dn != NULL)
            {
                if (domainNameSize >= strlen(dn + 1))
                {
                    snprintf(domainName, domainNameSize, "%s", ++dn);

                    dnsSuffix = domainName;
                }
            }
        }
    }

    return dnsSuffix;
}
#endif

std::string net_get_hostname()
{
    char hostname[256];
    std::string hostname_string = "";
    int result;

#ifdef WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        throw std::runtime_error("error: network error");
    }
#endif

    // get hostname
    result = gethostname(hostname, sizeof(hostname));

#ifdef WIN32
    WSACleanup();
#endif

    if (result == 0)
    {
        hostname_string = hostname;
    }

    return hostname_string;
}


std::string net_get_dns()
{
    std::string dns_suffix;

    std::vector<unsigned char> address;
    cmd_get_wired_mac_address(address);

    if (address.size() == 6)
    {
        char macAddress[6];
        macAddress[0] = address[0];
        macAddress[1] = address[1];
        macAddress[2] = address[2];
        macAddress[3] = address[3];
        macAddress[4] = address[4];
        macAddress[5] = address[5];

        // get DNS from OS
        dns_suffix = net_get_dns(macAddress);
    }

    return dns_suffix;
}
