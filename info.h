/*
Copyright 2019 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __INFO_H__
#define __INFO_H__

#include <string>

bool info_get(const std::string info);
bool info_get_version();
bool info_get_build_number();
bool info_get_sku();
bool info_get_uuid();
bool info_get_control_mode();
bool info_get_dns_suffix();
bool info_get_wired_mac_address();
bool info_get_all();

#endif
