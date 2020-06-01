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

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <iostream>
#include <string>

#include <cpprest/ws_client.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>

using namespace std;
using namespace web::websockets::client;
using namespace web;

#define PROTOCOL_VERSION "2.0.0"

#ifdef _WIN32
#define convertstring   to_utf16string
#else
#define convertstring   to_utf8string
#endif

string getDNSInfo();
string createActivationRequest(string profile);
json::value getCertificateHashes();
string createResponse(string payload);
string getActivateInfo(string profile);
string encodeBase64(string str);
string decodeBase64(string str);
void dumpMessage(string tmp);

#endif