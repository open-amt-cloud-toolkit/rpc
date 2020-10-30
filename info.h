/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __INFO_H__
#define __INFO_H__

#include <string>

bool info_get(const std::string info);
bool info_get_verify(const std::string info);
bool info_get_version();
bool info_get_build_number();
bool info_get_sku();
bool info_get_uuid();
bool info_get_control_mode();
bool info_get_dns_suffix();
bool info_get_all();
bool info_get_remote_access_connection_status();
bool info_get_lan_interface_settings();

#endif
