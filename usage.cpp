/*********************************************************************
* Copyright (c) Intel Corporation 2019 - 2020
* SPDX-License-Identifier: Apache-2.0
**********************************************************************/

#include "usage.h"
#include <iostream>
#include "version.h"
#include "activation.h"

void usage_show_help()
{
    // 80 chars  "01234567890123456789012345678901234567890123456789012345678901234567890123456789"
    std::cout << "Usage: "                                                                          << std::endl;
    std::cout << "  " << PROJECT_NAME << " <required> [optional]"                                   << std::endl;
    std::cout << "  " << PROJECT_NAME << " <informational>"                                         << std::endl;
    std::cout                                                                                       << std::endl;
    std::cout << "Required:"                                                                        << std::endl;
    std::cout << "  -u, --url <url>                 websocket server"                               << std::endl;
    std::cout << "  -c, --cmd <command>             server command"                                 << std::endl;
    std::cout                                                                                       << std::endl;
    std::cout << "  Since <command> can contain multiple options and arguments, the entire"         << std::endl;
    std::cout << "  <command> must be in quotes. Please see examples below."                        << std::endl;
    std::cout                                                                                       << std::endl;
    std::cout << "Optional:"                                                                        << std::endl;
    std::cout << "  -p, --proxy <addr>              proxy address and port"                         << std::endl;
    std::cout << "  -d, --dns <dns>                 dns suffix override"                            << std::endl;
    std::cout << "  -n, --nocertcheck               skip websocket server certificate verification" << std::endl;
    std::cout << "  -v, --verbose                   verbose output"                                 << std::endl;
    std::cout                                                                                       << std::endl;
    std::cout << "Informational:"                                                                   << std::endl;
    std::cout << "  --help                          this help text"                                 << std::endl;
    std::cout << "  --version                       version"                                        << std::endl;
    std::cout << "  --amtinfo <item>                AMT info on an <item>"                          << std::endl;
    std::cout                                                                                       << std::endl;
    std::cout << "Item:"                                                                            << std::endl;
    std::cout << "   all                            all items"                                      << std::endl;
    std::cout << "   ver                            BIOS version"                                   << std::endl;
    std::cout << "   bld                            build number"                                   << std::endl;
    std::cout << "   sku                            product SKU"                                    << std::endl;
    std::cout << "   uuid                           unique identifier"                              << std::endl;
    std::cout << "   mode                           current control mode"                           << std::endl;
    std::cout << "   dns                            domain name suffix"                             << std::endl;
    std::cout << "   fqdn                           fully qualified domain name"                    << std::endl;
    std::cout << "   cert                           certificate hashes"                             << std::endl;
    std::cout << "   ras                            remote access status"                           << std::endl;
    std::cout << "   lan                            LAN settings"                                   << std::endl;
    std::cout                                                                                       << std::endl;
    std::cout << "Examples:"                                                                        << std::endl;
    std::cout << "  # Activate platform using profile1"                                             << std::endl;
    std::cout << "  " << PROJECT_NAME << \
       " --url wss://localhost:8080 --cmd \"-t activate --profile profile1\""                       << std::endl;
    std::cout                                                                                       << std::endl;
    std::cout << "  # Activate platform using profile1 and override DNS detection" << std::endl;
    std::cout << "  " << PROJECT_NAME << \
       " --url wss://localhost:8080 --cmd \"-t activate --profile profile1\" --dns corp.com"        << std::endl;
    std::cout                                                                                       << std::endl;
    std::cout << "  # Deactivate platform and connect through a proxy"                              << std::endl;
    std::cout << "  " << PROJECT_NAME << \
       " -u wss://localhost:8080 -c \"-t deactivate --password P@ssw0rd\" -p http://proxy.com:1000" << std::endl;
    std::cout                                                                                       << std::endl;
    std::cout << "  # Show all informational items"                                                 << std::endl;
    std::cout << "  " << PROJECT_NAME << " --amtinfo all"                                           << std::endl;
}

void usage_show_version()
{
    std::string project_name = PROJECT_NAME;
    for (auto& c : project_name) c = toupper(c); // get upper case string

    std::cout << project_name << " " << PROJECT_VER << "." << std::endl;
    std::cout << "Protocol " << PROTOCOL_VERSION << "." << std::endl;
}
