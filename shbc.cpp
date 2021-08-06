/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "activation.h"
#include <cpprest/ws_client.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>
#include <iostream>
#include <string>
#include "version.h"
#include "commands.h"
#include "network.h"
#include "utils.h"

bool get_response_payload(std::string cert_algo, std::string cert_hash, web::json::value& payload)
{
    web::json::value value;
    utility::string_t tmp;
    web::json::value configParams;

    // get client string
    tmp = utility::conversions::convertstring(cert_algo);
    configParams[U("algorithm")] = web::json::value::string(tmp);

    // get certificate hashes
    tmp = utility::conversions::convertstring(cert_hash);
    configParams[U("hash")] = web::json::value::string(tmp);

    payload = configParams;

    return true;
}

bool shbc_create_response(std::string cert_algo, std::string cert_hash, bool config_status, std::string& response)
{
    web::json::value msg;

    utility::string_t tmp = utility::conversions::convertstring("secure_config_response");
    msg[U("method")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("");
    msg[U("apiKey")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROJECT_VER);
    msg[U("appVersion")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROTOCOL_VERSION);
    msg[U("protocolVersion")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("");
    msg[U("message")] = web::json::value::string(tmp);

    if (config_status)
    {
        // get the activation payload
        web::json::value responsePayload;
        if (!get_response_payload(cert_algo, cert_hash, responsePayload)) return false;

        // serialize payload
        std::string serializedPayload = utility::conversions::to_utf8string(responsePayload.serialize());
        std::string encodedPayload = util_encode_base64(serializedPayload);
        utility::string_t payload = utility::conversions::to_string_t(encodedPayload);
        msg[U("payload")] = web::json::value::string(payload);

        tmp = utility::conversions::convertstring("success");
        msg[U("status")] = web::json::value::string(tmp);
    }
    else
    {
        tmp = utility::conversions::convertstring("");
        msg[U("payload")] = web::json::value::string(tmp);

        tmp = utility::conversions::convertstring("failed");
        msg[U("status")] = web::json::value::string(tmp);
    }

    // serialize the entire message
    response = utility::conversions::to_utf8string(msg.serialize());

    return true;
}