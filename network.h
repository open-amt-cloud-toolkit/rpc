/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <string>

std::string net_get_dns(char* macAddress);
std::string net_get_hostname();
std::string net_get_dns();

#endif