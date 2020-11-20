/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "utils.h"
#include <vector>
#include <string>
#include <cpprest/streams.h>

std::string util_encode_base64(std::string str)
{
    std::vector<unsigned char> strVector(str.begin(), str.end());
    utility::string_t base64 = utility::conversions::to_base64(strVector);
    std::string encodedString = utility::conversions::to_utf8string(base64);

    return encodedString;
}

std::string util_decode_base64(std::string str)
{
    utility::string_t serializedData = utility::conversions::to_string_t(str);
    std::vector<unsigned char> strVector = utility::conversions::from_base64(serializedData);
    std::string decodedString(strVector.begin(), strVector.end());

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

