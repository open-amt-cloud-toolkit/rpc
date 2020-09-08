/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>

std::string util_encode_base64(std::string str);
std::string util_decode_base64(std::string str);
bool util_is_printable(std::string str);
bool util_format_uuid(std::vector<unsigned char> uuid_bytes, std::string& uuid_string);

#endif