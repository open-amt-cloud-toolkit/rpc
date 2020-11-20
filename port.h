/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#ifndef __PORT_H__
#define __PORT_H__

#include <string.h>

extern "C"
{
    // main entry into microlms
    extern int main_micro_lms();
}


#ifdef _WIN32
// Windows
#define strncpy    strncpy_s
#define strcasecmp    _stricmp
#else
// Linux
#endif

#endif
