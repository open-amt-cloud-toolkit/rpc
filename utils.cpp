/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "utils.h"
#include <vector>
#include <string>
#include <cpprest/streams.h>

std::string util_encode_base64(std::vector<unsigned char> str)
{
    std::vector<unsigned char> strVector(str.begin(), str.end());
    utility::string_t base64 = utility::conversions::to_base64(strVector);
    std::string encodedString = utility::conversions::to_utf8string(base64);

    return encodedString;
}

std::vector<unsigned char> util_decode_base64(std::string str)
{
    utility::string_t serializedData = utility::conversions::to_string_t(str);
    std::vector<unsigned char> strVector = utility::conversions::from_base64(serializedData);
    std::vector<unsigned char> decodedString(strVector.begin(), strVector.end());

    return decodedString;
}

bool util_is_printable(std::string str)
{
    for (char c : str)
    {
        if (!std::isprint(c))
        {
            return false;
        }
    }

    return true;
}

bool util_format_uuid(std::vector<unsigned char> uuid_bytes, std::string& uuid_string)
{
    if (uuid_bytes.size() != 16) return false;

    char tmp[100];
    snprintf(tmp, 100, 
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid_bytes[3], uuid_bytes[2], uuid_bytes[1], uuid_bytes[0],
        uuid_bytes[5], uuid_bytes[4],
        uuid_bytes[7], uuid_bytes[6],
        uuid_bytes[8], uuid_bytes[9],
        uuid_bytes[10], uuid_bytes[11], uuid_bytes[12], uuid_bytes[13], uuid_bytes[14], uuid_bytes[15]);

    uuid_string = tmp;

    return true;
}

bool util_hex_string_to_bytes(std::string hex_string, std::vector<unsigned char>& hex_bytes)
{
    hex_bytes.clear();

    for (int i = 0; i < hex_string.length(); i += 2) 
    {
        std::string byte_string = hex_string.substr(i, 2);
        byte_string[0] = tolower(byte_string[0]);
        byte_string[1] = tolower(byte_string[1]);
        unsigned char value = (char)strtol(byte_string.c_str(), NULL, 16);
        hex_bytes.push_back(value);
    }
    
    return true;
}

bool util_bytes_to_hex_string(std::vector<unsigned char> hex_bytes, std::string& hex_string)
{
    hex_string.clear();

    for (unsigned char hex_char : hex_bytes)
    {
        char hex[10];
        snprintf(hex, 10, "%02x", hex_char);
        hex_string += hex;
    }
    
    return true;
}