/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "info.h"
#include <iostream>
#include <string>
#include  <iomanip>
#include "commands.h"
#include "utils.h"

void out_text(const std::string name, const std::vector<unsigned char> value, const unsigned char delimeter=' ')
{
    std::cout << name << ": ";
    int char_count = 1;
    for (unsigned char tmp : value)
    {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) tmp;

        if (char_count++ < value.size())
        {
            std::cout << delimeter;
        }
    }

    std::cout << std::endl;
}

void out_text(const std::string name, const std::string value)
{
    std::cout << name << ": ";
    std::cout << value;
    std::cout << std::endl;
}

void out_text(const std::string name, const int value)
{
    std::cout << name << ": ";
    std::cout << value;
    std::cout << std::endl;
}

void out_text(const std::string name, const std::vector<std::string> value)
{
    int count = 1;

    std::cout << name << std::endl;
    for (std::string tmp : value)
    {
        std::cout << std::setfill('0') << std::setw(2) << count++ << ":";
        std::cout << tmp << std::endl;
    }
}

bool info_get_version() 
{
    std::string tmp;

    if (!cmd_get_version(tmp)) return false;

    out_text("Version", tmp);

    return true;
}

bool info_get_build_number()
{
    std::string tmp;

    if (!cmd_get_build_number(tmp)) return false;

    out_text("Build number", tmp);

    return true;
}

bool info_get_sku()
{
    std::string tmp;

    if (!cmd_get_sku(tmp)) return false;

    out_text("SKU", tmp);

    return true;
}

bool info_get_uuid()
{
    std::string uuid_string;
    std::vector<unsigned char> tmp;

    if (!cmd_get_uuid(tmp)) return false;

    if (!util_format_uuid(tmp, uuid_string)) return false;

    out_text("UUID", uuid_string);

    return true;
}

bool info_get_control_mode()
{
    int tmp;

    if (!cmd_get_control_mode(tmp)) return false;

    std::string control_mode;
    if (tmp == 0) control_mode = "pre-provisioning state";
    else if (tmp == 1) control_mode = "activated in client control mode";
    else if (tmp == 2) control_mode = "activated in admin control mode";

    out_text("Control mode", control_mode);

    return true;
}

bool info_get_dns_suffix()
{
    std::string tmp;

    if (!cmd_get_dns_suffix(tmp)) return false;

    out_text("DNS suffix", tmp);

    return true;
}

bool info_get_wired_mac_address()
{
    std::vector<unsigned char> tmp;

    if (!cmd_get_wired_mac_address(tmp)) return false;

    out_text("Wired MAC address", tmp, '-');

    return true;
}

bool info_get_certificate_hashes()
{
    std::vector<std::string> tmp;

    if (!cmd_get_certificate_hashes(tmp)) return false;

    out_text("Certificate hashes", tmp);

    return true;
}

bool info_get_all()
{
    std::vector<std::string> tmp;

    if (info_get_version() && info_get_build_number() && info_get_sku() &&
        info_get_uuid() && info_get_control_mode() && info_get_dns_suffix() &&
        info_get_wired_mac_address() && info_get_certificate_hashes())
    {
        return true;
    }

    return true;
}

bool info_get(const std::string info)
{
    if (info.compare("ver") == 0)
    {
        return info_get_version();
    }
    else if (info.compare("build") == 0)
    {
        return info_get_build_number();
    }
    else if (info.compare("sku") == 0)
    {
        return info_get_sku();
    }
    else if (info.compare("uuid") == 0)
    {
        return info_get_uuid();
    }
    else if (info.compare("mode") == 0)
    {
        return info_get_control_mode();
    }
    else if (info.compare("dns") == 0)
    {
        return info_get_dns_suffix();
    }
    else if (info.compare("mac") == 0)
    {
        return info_get_wired_mac_address();
    }
    else if (info.compare("cert") == 0)
    {
        return info_get_certificate_hashes();
    }
    else if (info.compare("all") == 0)
    {
        return info_get_all();
    }

    return false;
}
