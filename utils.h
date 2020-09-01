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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>

std::string util_encode_base64(std::string str);
std::string util_decode_base64(std::string str);
bool util_is_printable(std::string str);
bool util_format_uuid(std::vector<unsigned char> uuid_bytes, std::string& uuid_string);

#endif