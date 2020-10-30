/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "args.h"
#include <string>
#include "port.h"
#include "utils.h"

bool get_arg_exists(int argc, char* argv[], const char* long_opt, const char* short_opt)
{
    for (int i = 1; i < argc; i++)
    {
        if ((strcasecmp(argv[i], long_opt) == 0) || (strcasecmp(argv[i], short_opt) == 0))
        {
            return true;
        }
    }

    return false;
}

bool get_arg_exists(int argc, char* argv[], const char* opt)
{
    for (int i = 1; i < argc; i++)
    {
        if ((strcasecmp(argv[i], opt) == 0))
        {
            return true;
        }
    }

    return false;
}

bool get_arg_string(int argc, char* argv[], const char* long_opt, const char* short_opt, std::string& value)
{
    value = "";
    
    for (int i = 1; i < argc; i++)
    {
        if ((strcasecmp(argv[i], long_opt) == 0) || (strcasecmp(argv[i], short_opt) == 0))
        {
            if (i + 1 < argc)
            {
                std::string tmp = argv[++i];

                if (!util_is_printable(tmp))
                {
                    return false;
                }

                value = tmp;

                return true;
            }
        }
    }

    return false;
}

bool get_arg_string(int argc, char* argv[], const char* opt, std::string& value)
{
    value = "";

    for (int i = 1; i < argc; i++)
    {
        if ((strcasecmp(argv[i], opt) == 0))
        {
            if (i + 1 < argc)
            {
                std::string tmp = argv[++i];

                if (!util_is_printable(tmp))
                {
                    return false;
                }

                value = tmp;

                return true;
            }
        }
    }

    return false;
}

bool args_get_help(int argc, char* argv[])
{
    return get_arg_exists(argc, argv, "--help");
}

bool args_get_version(int argc, char* argv[])
{
    return get_arg_exists(argc, argv, "--version");
}

bool args_get_url(int argc, char* argv[], std::string& url)
{
    return get_arg_string(argc, argv, "--url", "-u", url);
}

bool args_get_proxy(int argc, char* argv[], std::string& proxy)
{
    return get_arg_string(argc, argv, "--proxy", "-p", proxy);
}

bool args_get_cmd(int argc, char* argv[], std::string& cmd)
{
    return get_arg_string(argc, argv, "--cmd", "-c", cmd);
}

bool args_get_dns(int argc, char* argv[], std::string& dns)
{
    return get_arg_string(argc, argv, "--dns", "-d", dns);
}

bool args_get_info(int argc, char* argv[], std::string& info)
{
    return get_arg_string(argc, argv, "--amtinfo", info);
}

bool args_get_verbose(int argc, char* argv[])
{
    return get_arg_exists(argc, argv, "--verbose", "-v");
}