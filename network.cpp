/*
Copyright 2019 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "network.h"
#include <iostream>

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