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

bool get_certificate_hashes(web::json::value& hashes)
{
    std::vector<web::json::value> hashValues;
    std::vector<cert_hash_entry> certHashes;
    if (!cmd_get_certificate_hashes(certHashes))
    {
        return false;
    }

    for (cert_hash_entry hashString : certHashes)
    {
        hashValues.push_back(web::json::value::string(utility::conversions::convertstring(hashString.hash)));
    }

    hashes = web::json::value::array(hashValues);

    return true;
}

bool get_uuid(web::json::value& value)
{
    int i = 0;
    std::vector<web::json::value> uuidValue;
    std::vector<unsigned char> uuid;

    if (!cmd_get_uuid(uuid)) return false;

    for (unsigned char value : uuid)
    {
        uuidValue.push_back(web::json::value(uuid[i++]));
    }

    value = web::json::value::array(uuidValue);

    return true;
}

std::string get_dns_info()
{
    std::string dnsSuffix;

    // get DNS according to AMT
    cmd_get_dns_suffix(dnsSuffix);

    if (!dnsSuffix.length())
    {
        // get DNS from OS
        dnsSuffix = net_get_dns();
    }

    return dnsSuffix;
}

web::json::value get_dns()
{
    utility::string_t tmp;

    std::string dnsSuffix = get_dns_info();
    tmp = utility::conversions::convertstring(dnsSuffix);

    return web::json::value::string(tmp);
}

web::json::value get_hostname()
{
    utility::string_t tmp;

    std::string hostName = net_get_hostname();
    tmp = utility::conversions::convertstring(hostName);

    return web::json::value::string(tmp);
}

bool getVersion(web::json::value& value)
{
    std::string version;
    utility::string_t tmp;

    if (!cmd_get_version(version)) return false;

    tmp = utility::conversions::convertstring(version);

    value = web::json::value::string(tmp);

    return true;
}

bool get_sku(web::json::value& value)
{
    std::string version;
    utility::string_t tmp;

    if (!cmd_get_sku(version)) return false;
    tmp = utility::conversions::convertstring(version);

    value = web::json::value::string(tmp);

    return true;
}

bool get_build_number(web::json::value& value)
{
    std::string version;
    utility::string_t tmp;

    if (!cmd_get_build_number(version)) return false;
    tmp = utility::conversions::convertstring(version);

    value = web::json::value::string(tmp);

    return true;
}

bool get_local_system_account_username(web::json::value& value)
{
    std::string username;
    std::string password;
    utility::string_t tmp;

    if (!cmd_get_local_system_account(username, password)) return false;
    tmp = utility::conversions::convertstring(username);

    value = web::json::value::string(tmp);

    return true;
}

 bool get_local_system_account_password(web::json::value& value)
{
    std::string username;
    std::string password;
    utility::string_t tmp;

    if (!cmd_get_local_system_account(username, password)) return false;
    tmp = utility::conversions::convertstring(password);

    value = web::json::value::string(tmp);

    return true;
}

bool get_control_mode(web::json::value& value)
{
    int controlMode;
    utility::string_t tmp;

    if (!cmd_get_control_mode(controlMode)) return false;

    value = web::json::value::number(controlMode);

    return true;
}

bool get_client_string(web::json::value& value)
{
    int controlMode;
    utility::string_t tmp;

    tmp = utility::conversions::convertstring("PPC");
    value = web::json::value::string(tmp);

    return true;
}

bool get_activation_payload(web::json::value& payload)
{
    web::json::value value;
    utility::string_t tmp;
    web::json::value activationParams;

    // get code version
    if (!getVersion(value)) return false;
    activationParams[U("ver")] = value;

    if (!get_build_number(value)) return false;
    activationParams[U("build")] = value;

    if (!get_sku(value)) return false;
    activationParams[U("sku")] = value;

    // get UUID
    if (!get_uuid(value)) return false;
    activationParams[U("uuid")] = value;

    // get local system account
    if (!get_local_system_account_username(value)) return false;
    activationParams[U("username")] = value;

    if (!get_local_system_account_password(value)) return false;
    activationParams[U("password")] = value;

    // get Control Mode
    if (!get_control_mode(value)) return false;
    activationParams[U("currentMode")] = value;
    
    // get DNS Info
    activationParams[U("fqdn")] = get_dns();

    // get hostname
    activationParams[U("hostname")] = get_hostname();

    // get client string
    if (!get_client_string(value)) return false;
    activationParams[U("client")] = value;
    
    // get certificate hashes
    if (!get_certificate_hashes(value)) return false;
    activationParams[U("certHashes")] = value;

    payload = activationParams;

    return true;
}

bool act_create_request(std::string commands, std::string dns_suffix, std::string& request)
{
    web::json::value msg;

    // get the activation info
    utility::string_t tmp = utility::conversions::convertstring(commands);
    msg[U("method")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("key");
    msg[U("apiKey")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROJECT_VER);
    msg[U("appVersion")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROTOCOL_VERSION);
    msg[U("protocolVersion")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("ok");
    msg[U("status")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("ok");
    msg[U("message")] = web::json::value::string(tmp);

    // get the activation payload
    web::json::value activationPayload;
    if (!get_activation_payload(activationPayload)) return false;

    // override dns value if passed in
    if (!dns_suffix.empty())
    {
        utility::string_t tmp = utility::conversions::convertstring(dns_suffix);
        activationPayload[U("fqdn")] = web::json::value::string(tmp);
    }

    // serialize payload
    std::string serializedPayload = utility::conversions::to_utf8string(activationPayload.serialize());
    std::string encodedPayload = util_encode_base64(serializedPayload);
    utility::string_t payload = utility::conversions::to_string_t(encodedPayload);
    msg[U("payload")] = web::json::value::string(payload);

    // serialize the entire message
    request = utility::conversions::to_utf8string(msg.serialize());

    return true;
}

bool act_create_response(std::string payload, std::string& response)
{
    web::json::value msg;

    utility::string_t tmp = utility::conversions::convertstring("response");
    msg[U("method")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("key");
    msg[U("apiKey")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROJECT_VER);
    msg[U("appVersion")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring(PROTOCOL_VERSION);
    msg[U("protocolVersion")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("ok");
    msg[U("status")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring("ok");
    msg[U("message")] = web::json::value::string(tmp);

    tmp = utility::conversions::convertstring(util_encode_base64(payload));
    msg[U("payload")] = web::json::value::string(tmp);

    response = utility::conversions::to_utf8string(msg.serialize());

    return true;
}

