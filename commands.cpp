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

#include "commands.h"

#ifdef _WIN32
#include <boost/asio.hpp>
#include <winsock2.h>
#include <iphlpapi.h>
#endif

#include "AMTHICommand.h"
#include "MEIClientException.h"
#include "GetUUIDCommand.h"
#include "GetLocalSystemAccountCommand.h"
#include "GetCodeVersionCommand.h"
#include "GetControlModeCommand.h"
#include "GetProvisioningStateCommand.h"
#include "GetDNSSuffixCommand.h"
#include "GetLanInterfaceSettingsCommand.h"
#include "GetCertificateHashEntryCommand.h"
#include "EnumerateHashHandlesCommand.h"
#include "MEIparser.h"
#include "version.h"
#include <boost/algorithm/string.hpp>

#include <cpprest/ws_client.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>
#include <sstream>
#include <iostream>
#include <string>
#include "lms.h"

using namespace std;
using namespace Intel::MEI_Client::AMTHI_Client;
using namespace web::websockets::client;
using namespace web;

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3


#ifdef _WIN32
std::string getDNSFromMAC(char *macAddress)
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

        pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen);
        if (pAddresses == NULL) {
            cout << "dns memory error" << std::endl;
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
            if (memcmp(macAddress, (char *) pCurrAddresses->PhysicalAddress, 6) == 0)
            {
                if (wcslen(pCurrAddresses->DnsSuffix) > 0)
                {
                    snprintf(dns, 256, "%ws", pCurrAddresses->DnsSuffix );
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
std::string getDNSFromMAC(char *macAddress)
{
    std::string dnsSuffix = "";

    // get socket
    SOCKET s = 0;
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0) 
    {
        cout << "couldn't get socket" << endl;
        return dnsSuffix;
    }

    // get info for all adapters
    struct ifconf ifc;
    memset(&ifc, 0, sizeof(ifconf));
    char buffer[8192];
    memset(buffer, 0, sizeof(buffer));

    ifc.ifc_buf = buffer;
    ifc.ifc_len = sizeof(buffer);
    if(ioctl(s, SIOCGIFCONF, &ifc) < 0) 
    {
        cout << "ioctl SIOCGIFCONF failed" << endl;
        return dnsSuffix;
    }

    // get DNS from IP associated with MAC
    struct ifreq *ifr = ifc.ifc_req;
    int interfaceCount = ifc.ifc_len / sizeof(struct ifreq);

    char ip[INET6_ADDRSTRLEN] = {0};
    struct ifreq *item;
    struct sockaddr *addr;
    for(int i = 0; i < interfaceCount; i++) 
    {
        item = &ifr[i];
        addr = &(item->ifr_addr);

        // get IP address
        if(ioctl(s, SIOCGIFADDR, item) < 0) 
        {
            cout << "ioctl SIOCGIFADDR failed" << endl;
            continue;
        }

        if (inet_ntop(AF_INET, &( ((struct sockaddr_in *)addr)->sin_addr ), 
            ip, sizeof(ip) ) == NULL)
        {
            cout << "inet_ntop" << endl;
            continue;
        }

        // get MAC address
        if(ioctl(s, SIOCGIFHWADDR, item) < 0) 
        {
            cout << "ioctl SIOCGIFHWADDR failed" << endl;
            continue;
        }

        if (memcmp(macAddress, (char *) item->ifr_hwaddr.sa_data, 6) == 0)
        {
            // Get host by using the IP address which AMT device is using
            struct in_addr addr = {0};
            struct hostent *host;
            addr.s_addr = inet_addr(ip);
            host = gethostbyaddr((char *)&addr, 4, AF_INET);
            if (host == NULL)
            {
                cout << "gethostbyaddr() failed";
                return dnsSuffix;
            }

            // strip off the hostname to get actual domain name
            int domainNameSize = 256;
            char domainName[domainNameSize];
            memset(domainName, 0, domainNameSize);
            
            char *dn = strchr(host->h_name, '.');
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


json::value getCertificateHashes()
{
    json::value certHashes;
    vector<json::value> hashValues;

    // get the hash handles
    EnumerateHashHandlesCommand command;
    ENUMERATE_HASH_HANDLES_RESPONSE response = command.getResponse();

    vector<unsigned int>::iterator itr = response.HashHandles.begin();
    vector<unsigned int>::iterator endItr = response.HashHandles.end();
    for (; itr != endItr; ++itr)
    {
        // get each entry
        GetCertificateHashEntryCommand command(*itr);
        GET_CERTIFICATE_HASH_ENTRY_RESPONSE response = command.getResponse();

        int hashSize;
        switch (response.HashAlgorithm) {
            case 0: // MD5
                hashSize = 16;
                break;
            case 1: // SHA1
                hashSize = 20;
                break;
            case 2: // SHA256
                hashSize = 32;
                break;
            case 3: // SHA512
                hashSize = 64;
                break;
            default:
                hashSize = 64;
                break;
        }
        
        if (response.IsActive == 1) { 
            string hashString;
            hashString.clear();
            for (int i = 0; i < hashSize; i++)
            {
                char hex[10];
                snprintf(hex, 10, "%02x", response.CertificateHash[i]);
                hashString += hex;
            }

            hashValues.push_back( json::value::string(  utility::conversions::convertstring(hashString) ) );
        }
    }

    return json::value::array(hashValues);
}

std::string getDNSInfo()
{
    std::string dnsSuffix;

    // Get interface info which AMT is using. We don't worry about wireless since
    // only wired used for configuration
    GetLanInterfaceSettingsCommand getLanInterfaceSettingsCommandWired(Intel::MEI_Client::AMTHI_Client::WIRED);
    LAN_SETTINGS lanSettings = getLanInterfaceSettingsCommandWired.getResponse();

    if (!lanSettings.Enabled)
    {
        cout << "error: no wired AMT interfaces enabled" << endl;
        return "";
    }

    // Get DNS according to AMT
    GetDNSSuffixCommand getDnsSuffixCommand;
	dnsSuffix = getDnsSuffixCommand.getResponse();

    // get DNS from OS
    if (!dnsSuffix.length())
    {
        dnsSuffix = getDNSFromMAC((char *)&lanSettings.MacAddress);
    }

    return dnsSuffix;
}


string getActivateInfo(string profile, string dnssuffixcmd)
{
    utility::string_t tmp;

    // Activation parameters
    json::value activationParams;

    // Get code version
    GetCodeVersionCommand codeVersionCommand;
    CODE_VERSIONS codeVersion = codeVersionCommand.getResponse();

    // Additional versions
    // UINT8[16] UUID;
    // AMT_VERSION_TYPE Version and Description are std::string.
    for (vector<AMT_VERSION_TYPE>::iterator it = codeVersion.Versions.begin(); it != codeVersion.Versions.end(); it++)
    {
        if (boost::iequals(it->Description, "AMT")) 
        {
            tmp = utility::conversions::convertstring(it->Version);
            activationParams[U("ver")] = json::value::string(tmp);
        }
        else if (boost::iequals(it->Description, "Build Number")) 
        {
            tmp = utility::conversions::convertstring(it->Version);
            activationParams[U("build")] = json::value::string(tmp);
        }
        else if (boost::iequals(it->Description, "Sku")) 
        {
            tmp = utility::conversions::convertstring(it->Version);
            activationParams[U("sku")] = json::value::string(tmp);
        }
    }

    // Get UUID
    GetUUIDCommand get;
    GET_UUID_RESPONSE res = get.getResponse();
    std::vector<json::value> UUID;
    for (int i = 0; i < 16; i++)
    {
        UUID.push_back(json::value(res.UUID[i]));
    }
    activationParams[U("uuid")] = json::value::array(UUID);

    // Get local system account
    // User name in ASCII char-set. The string is NULL terminated. CFG_MAX_ACL_USER_LENGTH is 33
    // Password in ASCII char set. From AMT 6.1 this field is in BASE64 format. The string is NULL terminated. 
    GetLocalSystemAccountCommand sac;
    tmp = utility::conversions::convertstring(sac.getResponse().UserName);
    activationParams[U("username")] = json::value::string(tmp);
    tmp = utility::conversions::convertstring(sac.getResponse().Password);
    activationParams[U("password")] = json::value::string(tmp);

    // Get Control Mode
    GetControlModeCommand controlModeCommand;
    GET_CONTROL_MODE_RESPONSE controlMode = controlModeCommand.getResponse();
    activationParams[U("currentMode")] = json::value::number(controlMode.ControlMode);

    // Get DNS Info
    tmp = utility::conversions::convertstring("");
    string dnsSuffix = "";
    if (dnssuffixcmd.length() > 0)
    {
        // use what's passed in
        dnsSuffix = dnssuffixcmd;
    }
    else
    {
        // get it from AMT or OS
        dnsSuffix = getDNSInfo();
    }

    if (dnsSuffix.length())
    {
        tmp = utility::conversions::convertstring(dnsSuffix);
    }
    
    activationParams[U("fqdn")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring("PPC");
    activationParams[U("client")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring(profile);
    activationParams[U("profile")] = json::value::string(tmp);

    // Get certificate hashes
    activationParams[U("certHashes")] = getCertificateHashes();
    
    // Return serialized parameters in base64
    string serializedParams = utility::conversions::to_utf8string(activationParams.serialize());
#ifdef DEBUG
    cout << "Activation info payload:" << serializedParams << std::endl;
#endif

    return encodeBase64(serializedParams);
}

string encodeBase64(string str)
{
    std::vector<unsigned char> strVector(str.begin(), str.end());
    utility::string_t base64 = utility::conversions::to_base64(strVector);
    string encodedString = utility::conversions::to_utf8string(base64);

    return encodedString;
}

string decodeBase64(string str)
{
    utility::string_t serializedData = utility::conversions::to_string_t(str);
    std::vector<unsigned char> strVector = utility::conversions::from_base64(serializedData);
    string decodedString(strVector.begin(), strVector.end());
    
    return decodedString;
}

string createActivationRequest(string profile, string dnssuffixcmd)
{
    // Activation parameters
    json::value request;

    // placeholder stuff; will likely change
    utility::string_t tmp = utility::conversions::convertstring("activation");
    request[U("method")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring("key");
    request[U("apiKey")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROJECT_VER);
    request[U("appVersion")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROTOCOL_VERSION);
    request[U("protocolVersion")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring("ok");
    request[U("status")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring("all\'s good!");
    request[U("message")] = json::value::string(tmp);

    // payload 
    string activationInfo = getActivateInfo(profile, dnssuffixcmd);
    utility::string_t payload =  utility::conversions::to_string_t(activationInfo);

    request[U("payload")] = json::value::string(payload);

    return utility::conversions::to_utf8string(request.serialize());
}

string createResponse(string payload)
{
    // Activation parameters
    json::value response;

    // placeholder stuff; will likely change
    utility::string_t tmp = utility::conversions::convertstring("response");
    response[U("method")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring("key");
    response[U("apiKey")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROJECT_VER);
    response[U("appVersion")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROTOCOL_VERSION);
    response[U("protocolVersion")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring("ok");
    response[U("status")] = json::value::string(tmp);

    tmp = utility::conversions::convertstring("all\'s good!");
    response[U("message")] = json::value::string(tmp);

    // payload 
    tmp = utility::conversions::convertstring( encodeBase64(payload) );
    response[U("payload")] = json::value::string(tmp);

    return utility::conversions::to_utf8string(response.serialize());
}

void dumpMessage(utility::string_t tmp)
{
    web::json::value parsed = web::json::value::parse(tmp);

    if ( !parsed.has_field(U("method"))          || !parsed.has_field(U("apiKey")) || !parsed.has_field(U("appVersion"))  || 
         !parsed.has_field(U("protocolVersion")) || !parsed.has_field(U("status")) || !parsed.has_field(U("message"))     ||
         !parsed.has_field(U("payload"))  ) {
            cout << "error: dumpMessage message is empty" << endl;
            return;
    }

    utility::string_t out = parsed[U("method")].as_string();
    cout << "method: " << utility::conversions::to_utf8string(out) << endl;

    out = parsed[U("apiKey")].as_string();
    cout << "apiKey: " << utility::conversions::to_utf8string(out) << endl;

    out = parsed[U("appVersion")].as_string();
    cout << "appVersion: " << utility::conversions::to_utf8string(out) << endl;

    out = parsed[U("protocolVersion")].as_string();
    cout << "protocolVersion: " << utility::conversions::to_utf8string(out) << endl;

    out = parsed[U("status")].as_string();
    cout << "status: " << utility::conversions::to_utf8string(out) << endl;

    out = parsed[U("message")].as_string();
    cout << "message: " << utility::conversions::to_utf8string(out) << endl;

    out = parsed[U("payload")].as_string();
    cout << "payload: " << utility::conversions::to_utf8string(out) << endl;
}