/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __ACTIVATION_H__
#define __ACTIVATION_H__

#include <string>

#define PROTOCOL_VERSION "4.1.0"

#ifdef _WIN32
#define convertstring   to_utf16string
#else
#define convertstring   to_utf8string
#endif

bool act_create_request(std::string commands, std::string dns_suffix, std::string& request);
bool act_create_response(std::string payload, std::string& response);

#endif 