/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include <string>
#include <thread>
#include <cpprest/ws_client.h>
#include <cpprest/json.h>
#include "port.h"
#include "utils.h"

const std::string plainText = "Ut aliquet ex id enim accumsan bibendum. Nullam nibh ligula, rhoncus vitae nisl eu, fermentum luctus tellus. Sed non semper augue, vitae congue nibh. Suspendisse sed placerat metus. Nunc a sapien vel nisl semper fringilla. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Aliquam accumsan, nisi vitae efficitur ultricies, velit neque mattis velit, sed sodales tellus est at orci. Duis sed ipsum lorem. In eu enim eu odio fringilla lacinia id id lorem. Vestibulum velit augue, ultricies et neque eget, molestie vehicula urna. Etiam venenatis nibh vel nunc fringilla, vitae feugiat ipsum posuere. Pellentesque ac libero quis nulla pretium iaculis sed at felis. Integer malesuada turpis eget lectus interdum, a commodo nisl tristique. Proin rutrum nisl ut metus aliquam, vel lacinia tortor ullamcorper. Nulla rhoncus ullamcorper accumsan. Fusce eget augue vulputate, porta quam at, laoreet justo.";
const std::string encodedText = "VXQgYWxpcXVldCBleCBpZCBlbmltIGFjY3Vtc2FuIGJpYmVuZHVtLiBOdWxsYW0gbmliaCBsaWd1bGEsIHJob25jdXMgdml0YWUgbmlzbCBldSwgZmVybWVudHVtIGx1Y3R1cyB0ZWxsdXMuIFNlZCBub24gc2VtcGVyIGF1Z3VlLCB2aXRhZSBjb25ndWUgbmliaC4gU3VzcGVuZGlzc2Ugc2VkIHBsYWNlcmF0IG1ldHVzLiBOdW5jIGEgc2FwaWVuIHZlbCBuaXNsIHNlbXBlciBmcmluZ2lsbGEuIFBlbGxlbnRlc3F1ZSBoYWJpdGFudCBtb3JiaSB0cmlzdGlxdWUgc2VuZWN0dXMgZXQgbmV0dXMgZXQgbWFsZXN1YWRhIGZhbWVzIGFjIHR1cnBpcyBlZ2VzdGFzLiBBbGlxdWFtIGFjY3Vtc2FuLCBuaXNpIHZpdGFlIGVmZmljaXR1ciB1bHRyaWNpZXMsIHZlbGl0IG5lcXVlIG1hdHRpcyB2ZWxpdCwgc2VkIHNvZGFsZXMgdGVsbHVzIGVzdCBhdCBvcmNpLiBEdWlzIHNlZCBpcHN1bSBsb3JlbS4gSW4gZXUgZW5pbSBldSBvZGlvIGZyaW5naWxsYSBsYWNpbmlhIGlkIGlkIGxvcmVtLiBWZXN0aWJ1bHVtIHZlbGl0IGF1Z3VlLCB1bHRyaWNpZXMgZXQgbmVxdWUgZWdldCwgbW9sZXN0aWUgdmVoaWN1bGEgdXJuYS4gRXRpYW0gdmVuZW5hdGlzIG5pYmggdmVsIG51bmMgZnJpbmdpbGxhLCB2aXRhZSBmZXVnaWF0IGlwc3VtIHBvc3VlcmUuIFBlbGxlbnRlc3F1ZSBhYyBsaWJlcm8gcXVpcyBudWxsYSBwcmV0aXVtIGlhY3VsaXMgc2VkIGF0IGZlbGlzLiBJbnRlZ2VyIG1hbGVzdWFkYSB0dXJwaXMgZWdldCBsZWN0dXMgaW50ZXJkdW0sIGEgY29tbW9kbyBuaXNsIHRyaXN0aXF1ZS4gUHJvaW4gcnV0cnVtIG5pc2wgdXQgbWV0dXMgYWxpcXVhbSwgdmVsIGxhY2luaWEgdG9ydG9yIHVsbGFtY29ycGVyLiBOdWxsYSByaG9uY3VzIHVsbGFtY29ycGVyIGFjY3Vtc2FuLiBGdXNjZSBlZ2V0IGF1Z3VlIHZ1bHB1dGF0ZSwgcG9ydGEgcXVhbSBhdCwgbGFvcmVldCBqdXN0by4=";

// Test if characters are printable
TEST(testUtils, isPrintableTestValid)
{
    std::string s = "The quick brown fox jumps over the lazy dog.";
    EXPECT_EQ(true, util_is_printable(s));	
}

// Test if characters are printable
TEST(testUtils, isPrintableTestInvalid)
{
    std::string s = "The quick brown fox jumps over the lazy dog.";
    s[0] = 10; // non-printable character

    EXPECT_EQ(false, util_is_printable(s));	
}

// Test encode of base64 string
TEST(testUtils, encodebase64)
{
    EXPECT_EQ(encodedText, util_encode_base64(plainText));	
}

// Test decode of base64 sstring
TEST(testUtils, decodebase64)
{
    EXPECT_EQ(plainText, util_decode_base64(encodedText));	
}

// Test return value of util_format_uuid
TEST(testUtils, formatUUIDSuccess)
{
    std::vector<unsigned char> uuid_bytes;
    for (int i=0; i<16; i++)
    {
        uuid_bytes.push_back(i);
    }

    std::string uuid_string;
    util_format_uuid(uuid_bytes, uuid_string);

    EXPECT_EQ(true, util_format_uuid(uuid_bytes, uuid_string));	
}

// Test return value of util_format_uuid
TEST(testUtils, formatUUIDFail)
{
    std::vector<unsigned char> uuid_bytes;
    for (int i=0; i<5; i++) // invalid length
    {
        uuid_bytes.push_back(i);
    }

    std::string uuid_string;
    util_format_uuid(uuid_bytes, uuid_string);

    EXPECT_EQ(false, util_format_uuid(uuid_bytes, uuid_string));	
}

// Test value of the uuid format to ensure format is correct
TEST(testUtils, formatUUIDValue)
{
    std::string uuid_string;
    std::vector<unsigned char> uuid_bytes;

    for (int i=0; i<16; i++)
    {
        uuid_bytes.push_back(i);
    }
        
    util_format_uuid(uuid_bytes, uuid_string);

    EXPECT_EQ("03020100-0504-0706-0809-0a0b0c0d0e0f", uuid_string);
}