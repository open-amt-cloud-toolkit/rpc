/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __LMS_H__
#define __LMS_H__

#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
typedef int SOCKET;
#define INVALID_SOCKET (SOCKET)(-1)
#endif

#ifdef _WIN32
// Windows
#else
// Linux
static inline int closesocket(int fd)
{
    return close(fd);
}
#define SD_BOTH SHUT_RDWR
#endif

SOCKET lms_connect();

#endif