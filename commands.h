/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <vector>
#include <string>

bool cmd_get_version(std::string& version);
bool cmd_get_build_number(std::string& version);
bool cmd_get_sku(std::string& version);
bool cmd_get_uuid(std::vector<unsigned char>& uuid);
bool cmd_get_local_system_account(std::string& username, std::string& password);
bool cmd_get_control_mode(int& mode);
bool cmd_get_dns_suffix(std::string& suffix);
bool cmd_get_wired_mac_address(std::vector<unsigned char>& address);
bool cmd_get_certificate_hashes(std::vector<std::string>& hashes);

#endif