/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "commands.h"
#include <string>
#include "port.h"
#include "utils.h"

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

bool cmd_is_admin()
{
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    return true;
}

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

bool cmd_get_fqdn(fqdn_settings& fqdn_settings)
{
    fqdn_settings.fqdn.clear();

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get fqdn
    CFG_GET_FQDN_RESPONSE fqdn;
    memset(&fqdn, 0, sizeof(CFG_GET_FQDN_RESPONSE));
    AMT_STATUS amt_status = pthi_GetHostFQDN(&fqdn);

    if (amt_status == 0)
    {
        fqdn_settings.ddns_ttl = fqdn.DDNSTTL;
        fqdn_settings.ddns_update_enabled = fqdn.DDNSUpdateEnabled;
        fqdn_settings.ddns_update_interval = fqdn.DDNSPeriodicUpdateInterval;

        if (fqdn.FQDN.Length > 0)
        {
            fqdn_settings.fqdn = std::string(fqdn.FQDN.Buffer, fqdn.FQDN.Length);
            free(fqdn.FQDN.Buffer);
        }

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
        if (amt_dns_suffix.Buffer != NULL)
        {
            std::string tmp(amt_dns_suffix.Buffer, amt_dns_suffix.Length);
            suffix = tmp;
            free(amt_dns_suffix.Buffer);
        }

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

bool cmd_get_certificate_hashes(std::vector<cert_hash_entry>& hash_entries)
{
    hash_entries.clear();

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
            cert_hash_entry tmp;
            switch (certhash_entry.HashAlgorithm) {
            case 0: // MD5
                hashSize = 16;
                tmp.algorithm = "MD5";
                break;
            case 1: // SHA1
                hashSize = 20;
                tmp.algorithm = "SHA1";
                break;
            case 2: // SHA256
                hashSize = 32;
                tmp.algorithm = "SHA256";
                break;
            case 3: // SHA512
                hashSize = 64;
                tmp.algorithm = "SHA512";
                break;
            default:
                hashSize = 0;
                tmp.algorithm = "UNKNOWN";
                break;
            }

            if (certhash_entry.IsActive == 1) 
            {
                std::string cert_name(certhash_entry.Name.Buffer, certhash_entry.Name.Length);
                tmp.name = cert_name;
                tmp.is_default = certhash_entry.IsDefault;
                tmp.is_active = certhash_entry.IsActive;

                std::string hashString;
                for (int i = 0; i < hashSize; i++)
                {
                    char hex[10];
                    snprintf(hex, 10, "%02x", certhash_entry.CertificateHash[i]);
                    hashString += hex;
                }

                tmp.hash = hashString;

                hash_entries.push_back(tmp);
            }
        }

        return true;
    }

    return false;
}

bool cmd_get_remote_access_connection_status(int& network_status, int& remote_status, int& remote_trigger, std::string& mps_hostname)
{
    network_status = 0;
    remote_status = 0;
    remote_trigger = 0;
    mps_hostname = "";
    const int MPS_SERVER_MAXLENGTH = 256;

    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get DNS according to AMT
    REMOTE_ACCESS_STATUS remote_access_connection_status;
    AMT_STATUS amt_status = pthi_GetRemoteAccessConnectionStatus(&remote_access_connection_status);

    if (amt_status == 0)
    {
        network_status = remote_access_connection_status.AmtNetworkConnectionStatus;
        remote_status  = remote_access_connection_status.RemoteAccessConnectionStatus;
        remote_trigger = remote_access_connection_status.RemoteAccessConnectionTrigger;

        if (remote_access_connection_status.MpsHostname.Buffer != NULL)
        {
            if (remote_access_connection_status.MpsHostname.Length < MPS_SERVER_MAXLENGTH)
            {
                std::string tmp(remote_access_connection_status.MpsHostname.Buffer, remote_access_connection_status.MpsHostname.Length);
                if (util_is_printable(tmp))
                {
                    mps_hostname = tmp;
                }
            }

            free(remote_access_connection_status.MpsHostname.Buffer);
        }

        return true;
    }

    return false;
}

bool cmd_get_lan_interface_settings(lan_interface_settings& lan_interface_settings)
{
    // initialize HECI interface
    if (heci_Init(NULL, PTHI_CLIENT) == 0) return false;

    // get wired interface
    LAN_SETTINGS lan_settings;
    UINT32 interface_settings = 0; // wired=0, wireless=1
    AMT_STATUS amt_status = pthi_GetLanInterfaceSettings(interface_settings, &lan_settings);
    if (amt_status == 0)
    {
        lan_interface_settings.is_enabled   = lan_settings.Enabled;
        lan_interface_settings.dhcp_mode    = lan_settings.DhcpIpMode;
        lan_interface_settings.dhcp_enabled = lan_settings.DhcpEnabled;
        lan_interface_settings.link_status  = lan_settings.LinkStatus;

        lan_interface_settings.ip_address.push_back((lan_settings.Ipv4Address >> 24) & 0xff);
        lan_interface_settings.ip_address.push_back((lan_settings.Ipv4Address >> 16) & 0xff);
        lan_interface_settings.ip_address.push_back((lan_settings.Ipv4Address >> 8) & 0xff);
        lan_interface_settings.ip_address.push_back((lan_settings.Ipv4Address) & 0xff);

        lan_interface_settings.mac_address.push_back(lan_settings.MacAddress[0]);
        lan_interface_settings.mac_address.push_back(lan_settings.MacAddress[1]);
        lan_interface_settings.mac_address.push_back(lan_settings.MacAddress[2]);
        lan_interface_settings.mac_address.push_back(lan_settings.MacAddress[3]);
        lan_interface_settings.mac_address.push_back(lan_settings.MacAddress[4]);
        lan_interface_settings.mac_address.push_back(lan_settings.MacAddress[5]);

        return true;
    }

    return false;
}
