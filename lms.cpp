/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/
#include "lms.h"
#include <string.h>

#ifdef _WIN32
// Windows
#include <Ws2tcpip.h>
#else
// Linux
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

SOCKET lms_connect()
{
    std::string lmsAddress = "localhost";
    std::string lmsPort = "16992";
    SOCKET s = INVALID_SOCKET;
    struct addrinfo *addr, hints;

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        throw std::runtime_error("error: unable to connect to LMS");
    }
#endif

    memset(&hints, 0, sizeof(hints));
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(lmsAddress.c_str(), lmsPort.c_str(), &hints, &addr) != 0)
    {
        throw std::runtime_error("error: unable to connect to LMS");
    }

    if (addr == NULL)
    {
        throw std::runtime_error("error: unable to connect to LMS");
    }

    for (addr; addr != NULL; addr = addr->ai_next)
    {
        s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (s == INVALID_SOCKET)
        {
            continue;
        }

        if (connect(s, addr->ai_addr, (int)addr->ai_addrlen) == 0)
        {
            break;
        }

        closesocket(s);
        s = INVALID_SOCKET;
    }

    if (addr != NULL)
    {
        freeaddrinfo(addr);
    }

    if (s == INVALID_SOCKET)
    {
        throw std::runtime_error("error: unable to connect to LMS");
    }

    return s;
}