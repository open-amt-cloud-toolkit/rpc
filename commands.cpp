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
#include <string>
#include "port.h"

#ifndef _WIN32
#include <string.h>
#endif

extern "C" {
#ifndef _WIN32
  #include "HECILinux.h"
#endif
  #include "PTHICommand.h"
#ifdef bool
  #undef bool
#endif
}
#include "version.h"

bool cmd_get_version(std::string& version)
{
    version.clear();

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get code version
    CODE_VERSIONS codeVersion;
    AMT_STATUS status = pthi_GetCodeVersions(&codeVersion);

    // additional versions
    if (status == 0)
    {
        for (int i = 0; i < (int) codeVersion.VersionsCount; i++)
        {
            if (strcmp((char*)(codeVersion.Versions[i].Description.String), "AMT") == 0)
            {
                version = ((char*)codeVersion.Versions[i].Version.String);

                return true;
            }
        }
    }

    return false;
}

bool cmd_get_build_number(std::string& version)
{
    version.clear();

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;
    
    // get code version
    CODE_VERSIONS codeVersion;
    AMT_STATUS status = pthi_GetCodeVersions(&codeVersion);

    // additional versions
    if (status == 0)
    {
        for (int i = 0; i < (int) codeVersion.VersionsCount; i++)
        {
            if (strcmp((char*)(codeVersion.Versions[i].Description.String), "Build Number") == 0)
            {
                version = ((char*)codeVersion.Versions[i].Version.String);

                return true;
            }
        }
    }

    return false;
}

bool cmd_get_sku(std::string& version)
{
    version.clear();

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get code version
    CODE_VERSIONS codeVersion;
    AMT_STATUS status = pthi_GetCodeVersions(&codeVersion);

    // additional versions
    if (status == 0)
    {
        for (int i = 0; i < (int) codeVersion.VersionsCount; i++)
        {
            if (strcmp((char*)(codeVersion.Versions[i].Description.String), "Sku") == 0)
            {
                version = ((char*)codeVersion.Versions[i].Version.String);

                return true;
            }
        }
    }

    return false;
}

bool cmd_get_uuid(std::vector<unsigned char>& uuid)
{
    uuid.clear();

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get UUID
    AMT_UUID amt_uuid;
    AMT_STATUS amt_status = pthi_GetUUID(&amt_uuid);
    if (amt_status == 0)
    {
        for (int i = 0; i < 16; i++)
        {
            uuid.push_back(amt_uuid[i]);
        }

        return true;
    }

    return false;
}

bool cmd_get_local_system_account(std::string& username, std::string& password)
{
    username.clear();
    password.clear();

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get Local System Account
    LOCAL_SYSTEM_ACCOUNT local_system_account;
    AMT_STATUS amt_status = pthi_GetLocalSystemAccount(&local_system_account);
    if (amt_status == 0)
    {
        username = local_system_account.username;
        password = local_system_account.password;

        return true;
    }

    return false;
}

bool cmd_get_control_mode(int& mode)
{
    mode = 0;

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get Control Mode
    int controlMode;
    AMT_STATUS amt_status = pthi_GetControlMode(&controlMode);
    if (amt_status == 0)
    {
        mode = controlMode;

        return true;
    }

    return false;
}

bool cmd_get_dns_suffix(std::string& suffix)
{
    suffix.clear();

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get DNS according to AMT
    AMT_ANSI_STRING amt_dns_suffix;
    AMT_STATUS amt_status = pthi_GetDnsSuffix(&amt_dns_suffix);

    if (amt_status == 0)
    {
        std::string tmp(amt_dns_suffix.Buffer, amt_dns_suffix.Length);
        suffix = tmp;

        return true;
    }

    return false;
}

bool cmd_get_wired_mac_address(std::vector<unsigned char>& address)
{
    address.clear();

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get wired interface
    LAN_SETTINGS lan_settings;
    UINT32 interface_settings = 0; // wired=0, wireless=1
    AMT_STATUS amt_status = pthi_GetLanInterfaceSettings(interface_settings, &lan_settings);
    if (amt_status == 0)
    {
        if (!lan_settings.Enabled)
        {
            return false;
        }

        for (int i = 0; i < 6; i++)
        {
            address.push_back(lan_settings.MacAddress[i]);
        }

        return true;
    }

    return false;
}

bool cmd_get_certificate_hashes(std::vector<std::string>& hashes)
{
    hashes.clear();

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get the hash handles
    AMT_HASH_HANDLES amt_hash_handles;
    CERTHASH_ENTRY certhash_entry;

    memset(&amt_hash_handles, 0, sizeof(AMT_HASH_HANDLES));
    if (pthi_EnumerateHashHandles(&amt_hash_handles) == 0)
    {
        for (int i = 0; i < (int) amt_hash_handles.Length; i++)
        {
            // get each entry
            AMT_STATUS status = pthi_GetCertificateHashEntry(amt_hash_handles.Handles[i], &certhash_entry);

            int hashSize;
            switch (certhash_entry.HashAlgorithm) {
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

            if (certhash_entry.IsActive == 1) 
            {
                std::string hashString;
                hashString.clear();
                for (int i = 0; i < hashSize; i++)
                {
                    char hex[10];
                    snprintf(hex, 10, "%02x", certhash_entry.CertificateHash[i]);
                    hashString += hex;
                }

                hashes.push_back(hashString);
            }
        }

        return true;
    }

    return false;
}
