/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "info.h"
#include <iostream>
#include <string>
#include  <iomanip>
#include "commands.h"
#include "utils.h"
#include "network.h"

const int PADDING = 25;

void out_text(const std::string name, const std::vector<unsigned char> value, const unsigned char delimeter = ' ', const bool hex = true)
{
    std::cout << name << std::setfill(' ') << std::setw(PADDING - name.size())  << ": ";
    int char_count = 1;
    for (unsigned char tmp : value)
    {
        (hex) ? std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)tmp
              : std::cout << std::dec << (unsigned int)tmp;

        if (char_count++ < value.size())
        {
            std::cout << delimeter;
        }
    }

    std::cout << std::endl;
}

void out_text(const std::string name, const std::string value)
{
    std::cout << name << std::setfill(' ') << std::setw(PADDING - name.size()) << ": ";
    std::cout << value;
    std::cout << std::endl;
}

void out_text(const std::string name, const int value)
{
    std::cout << name << std::setfill(' ') << std::setw(PADDING - name.size()) << ": ";
    std::cout << value;
    std::cout << std::endl;
}

void out_text(const std::string name, const std::vector<std::string> value)
{
    std::cout << name << std::setfill(' ') << std::setw(PADDING - name.size()) << ": " << std::endl;
    for (std::string tmp : value)
    {
        std::cout << tmp << std::endl;
    }
}

bool info_get_version() 
{
    std::string tmp;

    if (!cmd_get_version(tmp)) return false;

    out_text("Version", tmp);

    return true;
}

bool info_get_build_number()
{
    std::string tmp;

    if (!cmd_get_build_number(tmp)) return false;

    out_text("Build Number", tmp);

    return true;
}

bool info_get_sku()
{
    std::string tmp;

    if (!cmd_get_sku(tmp)) return false;

    out_text("SKU", tmp);

    return true;
}

bool info_get_uuid()
{
    std::string uuid_string;
    std::vector<unsigned char> tmp;

    if (!cmd_get_uuid(tmp)) return false;

    if (!util_format_uuid(tmp, uuid_string)) return false;

    out_text("UUID", uuid_string);

    return true;
}

bool info_get_control_mode()
{
    int tmp;

    if (!cmd_get_control_mode(tmp)) return false;

    std::string control_mode;
    if (tmp == 0) control_mode = "pre-provisioning state";
    else if (tmp == 1) control_mode = "activated in client control mode";
    else if (tmp == 2) control_mode = "activated in admin control mode";

    out_text("Control Mode", control_mode);

    return true;
}

bool info_get_dns_suffix()
{
    std::string tmp;

    if (!cmd_get_dns_suffix(tmp)) return false;

    out_text("DNS Suffix", tmp);


    tmp = net_get_dns();
    out_text("DNS Suffix (OS)", tmp);

    return true;
}

bool info_get_fqdn()
{
    fqdn_settings fqdn;

    if (cmd_get_fqdn(fqdn))
    {
        out_text("FQDN", fqdn.fqdn);
    }

    std::string tmp;
    std::string dns;

    tmp = net_get_hostname();
    out_text("Hostname (OS)", tmp);


    return true;
}

bool info_get_certificate_hashes()
{
    std::vector<cert_hash_entry> hash_entries;
    std::vector<std::string> tmp;

    if (!cmd_get_certificate_hashes(hash_entries)) return false;

    for (cert_hash_entry entry : hash_entries)
    {
        std::string name = entry.name;
        (entry.is_default) ? name += ", (Default, " : ", (Not Default, ";
        (entry.is_active) ? name += "Active)" : "Not Active)";
        tmp.push_back(name);

        std::string algorithm = "  " + entry.algorithm;
        algorithm += ":  " + entry.hash;
        tmp.push_back(algorithm);
    }

    out_text("Certificate Hashes", tmp);

    return true;
}

bool info_get_all()
{
    bool status_ver  = info_get_version();
    bool status_bld  = info_get_build_number();
    bool status_sku  = info_get_sku();
    bool status_uuid = info_get_uuid();
    bool status_mode = info_get_control_mode();
    bool status_dns  = info_get_dns_suffix();
    bool status_fqdn = info_get_fqdn();
    bool status_ras  = info_get_remote_access_connection_status();
    bool status_lan  = info_get_lan_interface_settings();
    bool status_cert = info_get_certificate_hashes();

    if (status_ver && status_bld && status_sku && status_uuid && status_mode &&
        status_dns && status_fqdn && status_ras && status_lan && status_cert)
    {
        return true;
    }

    return false;
}

bool info_get_remote_access_connection_status()
{
    int network_status;
    int remote_status;
    int remote_trigger;
    std::string mps_hostname;

    if (!cmd_get_remote_access_connection_status(network_status, remote_status, remote_trigger, mps_hostname)) return false;

    std::string tmp;

    switch (network_status)
    {
    case 0: tmp = "direct";
        break;
    case 1: tmp = "vpn";
        break;
    case 2: tmp = "outside enterprise";
        break;
    case 3: 
    default: tmp = "unknown";
        break;
    }

    out_text("RAS Network", tmp);

    switch (remote_status)
    {
    case 0: tmp = "not connected";
        break;
    case 1: tmp = "connecting";
        break;
    case 2: tmp = "connected";
        break;
    default: tmp = "unknown";
        break;
    }

    out_text("RAS Remote Status", tmp);

    switch (remote_trigger)
    {
    case 0: tmp = "user initiated";
        break;
    case 1: tmp = "alert";
        break;
    case 2: tmp = "periodic";
        break;
    case 3: tmp = "provisioning";
        break;
    default: tmp = "unknown";
        break;
    
    }

    out_text("RAS Trigger", tmp);

    out_text("RAS MPS Hostname", mps_hostname);

    return true;
}

bool info_get_lan_interface_settings()
{
    lan_interface_settings tmp;

    tmp.is_enabled = false;
    tmp.link_status = false;
    tmp.dhcp_enabled = false;
    tmp.dhcp_mode = 0;
    tmp.ip_address.clear();
    tmp.mac_address.clear();

    bool hasWired = cmd_get_lan_interface_settings(tmp);
    if (hasWired)
    {
        out_text("LAN Interface", "wired");
        out_text("DHCP Enabled", (tmp.dhcp_enabled) ? "true" : "false");
        out_text("DHCP Mode", (tmp.dhcp_mode == 1) ? "active" : "passive");
        out_text("Link Status", (tmp.link_status) ? "up" : "down");
        out_text("IP Address", tmp.ip_address, '.', false);
        out_text("MAC Address", tmp.mac_address, ':');
    }
    
    tmp.is_enabled = false;
    tmp.link_status = false;
    tmp.dhcp_enabled = false;
    tmp.dhcp_mode = 0;
    tmp.ip_address.clear();
    tmp.mac_address.clear();

    bool hasWireless = cmd_get_lan_interface_settings(tmp, false);
    if (hasWireless)
    {
        out_text("LAN Interface", "wireless");
        out_text("DHCP Enabled", (tmp.dhcp_enabled) ? "true" : "false");
        out_text("DHCP Mode", (tmp.dhcp_mode == 1) ? "active" : "passive");
        out_text("Link Status", (tmp.link_status) ? "up" : "down");
        out_text("IP Address", tmp.ip_address, '.', false);
        out_text("MAC Address", tmp.mac_address, ':');
    }

    if (hasWired || hasWireless)
    {
        return true;
    }

    return false;
}

bool info_get(const std::string info)
{
    if (info.compare("ver") == 0)
    {
        return info_get_version();
    }
    else if (info.compare("bld") == 0)
    {
        return info_get_build_number();
    }
    else if (info.compare("sku") == 0)
    {
        return info_get_sku();
    }
    else if (info.compare("uuid") == 0)
    {
        return info_get_uuid();
    }
    else if (info.compare("mode") == 0)
    {
        return info_get_control_mode();
    }
    else if (info.compare("dns") == 0)
    {
        return info_get_dns_suffix();
    }
    else if (info.compare("fqdn") == 0)
    {
        return info_get_fqdn();
    }
    else if (info.compare("cert") == 0)
    {
        return info_get_certificate_hashes();
    }
    else if (info.compare("ras") == 0)
    {
        return info_get_remote_access_connection_status();
    }
    else if (info.compare("lan") == 0)
    {
        return info_get_lan_interface_settings();
    }
    else if (info.compare("all") == 0)
    {
        return info_get_all();
    }

    return false;
}

bool info_get_verify(const std::string info)
{
    if ((info.compare("ver")  == 0) || (info.compare("bld")  == 0) || (info.compare("sku") == 0)  || 
        (info.compare("uuid") == 0) || (info.compare("mode") == 0) || (info.compare("fqdn") == 0) || 
        (info.compare("dns") == 0)  || (info.compare("cert") == 0) || (info.compare("ras")  == 0) || 
        (info.compare("lan") == 0)  || (info.compare("all") == 0))
    {
        return true;
    }

    return false;
}
