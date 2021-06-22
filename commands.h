/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <vector>
#include <string>

struct cert_hash_entry
{
    std::string hash;
    std::string name;
    std::string algorithm;
    bool is_active;
    bool is_default;
};

struct lan_interface_settings
{
    bool is_enabled;
    bool link_status;
    bool dhcp_enabled;
    int  dhcp_mode;
    std::vector<unsigned char> ip_address;
    std::vector<unsigned char> mac_address;
};

struct fqdn_settings
{
    bool shared_fqdn;
    bool ddns_update_enabled;
    int  ddns_update_interval;
    int  ddns_ttl;
    std::string fqdn;
};

bool cmd_is_admin();
bool cmd_get_version(std::string& version);
bool cmd_get_build_number(std::string& version);
bool cmd_get_sku(std::string& version);
bool cmd_get_uuid(std::vector<unsigned char>& uuid);
bool cmd_get_local_system_account(std::string& username, std::string& password);
bool cmd_get_control_mode(int& mode);
bool cmd_get_fqdn(fqdn_settings& fqdn_settings);
bool cmd_get_dns_suffix(std::string& suffix);
bool cmd_get_wired_mac_address(std::vector<unsigned char>& address);
bool cmd_get_certificate_hashes(std::vector<cert_hash_entry>& hash_entries);
bool cmd_get_remote_access_connection_status(int& network_status, int& remote_status, int& remote_trigger, std::string& mps_hostname);
bool cmd_get_lan_interface_settings(lan_interface_settings& lan_interface_settings, bool wired_interface = true);

#endif