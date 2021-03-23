/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "heartbeat.h"
#include <cpprest/ws_client.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>
#include <iostream>
#include <string>
#include "activation.h"
#include "version.h"
#include "commands.h"
#include "network.h"
#include "utils.h"

bool heartbeat_create_response(std::string& response)
{
    web::json::value msg;

    utility::string_t tmp = utility::conversions::convertstring("heartbeat_response");
    msg[U("method")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("");
    msg[U("apiKey")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROJECT_VER);
    msg[U("appVersion")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROTOCOL_VERSION);
    msg[U("protocolVersion")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("success");
    msg[U("status")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("");
    msg[U("message")] = web::json::value::string(tmp);

    // set empty payload
    tmp = utility::conversions::convertstring("");
    msg[U("payload")] = web::json::value::string(tmp);

    // serialize the entire message
    response = utility::conversions::to_utf8string(msg.serialize());

    return true;
}