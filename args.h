/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __ARGS_H__
#define __ARGS_H__

#include <string>

bool args_get_help(int argc, char* argv[]);
bool args_get_version(int argc, char* argv[]);
bool args_get_url(int argc, char* argv[], std::string& url);
bool args_get_proxy(int argc, char* argv[], std::string& proxy);
bool args_get_cmd(int argc, char* argv[], std::string& cmd);
bool args_get_dns(int argc, char* argv[], std::string& dns);
bool args_get_info(int argc, char* argv[], std::string& info);

#endif
