/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __SHBC_H__
#define __SHBC_H__

#include <string>

#ifdef _WIN32
#define convertstring   to_utf16string
#else
#define convertstring   to_utf8string
#endif

bool shbc_create_response(std::string cert_algo, std::string cert_hash, bool config_status, std::string& response);

#endif 