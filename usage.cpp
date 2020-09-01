/*
Copyright 2019 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "usage.h"
#include <iostream>
#include "version.h"
#include "activation.h"

void usage_show_help()
{
    std::cout << "Usage: " << PROJECT_NAME << " --help | --version | --info <item> | --url <url> --cmd <command> [--proxy <addr>] [--dns <dns>]" << std::endl;
    std::cout << "Required:"                                                                                                                     << std::endl;
    std::cout << "  -u, --url <url>                 websocket server"                                                                            << std::endl;
    std::cout << "  -c, --cmd <command>             server command"                                                                              << std::endl;
    std::cout << "Optional:"                                                                                                                     << std::endl;
    std::cout << "  -x, --proxy <addr>              proxy address and port"                                                                      << std::endl;
    std::cout << "  -d, --dns <dns>                 dns suffix"                                                                                  << std::endl;
    std::cout << "Informational:"                                                                                                                << std::endl;
    std::cout << "  -h, --help                      this help text"                                                                              << std::endl;
    std::cout << "  -v, --version                   version"                                                                                     << std::endl;
    std::cout << "  -i, --info <item>               info on an <item>"                                                                           << std::endl;
    std::cout                                                                                                                                    << std::endl;
    std::cout << "Info <item>:"                                                                                                                  << std::endl;
    std::cout << "   all                            all items"                                                                                   << std::endl;
    std::cout << "   ver                            bios version"                                                                                << std::endl;
    std::cout << "   build                          build number"                                                                                << std::endl;
    std::cout << "   sku                            product sku"                                                                                 << std::endl;
    std::cout << "   uuid                           unique identifier"                                                                           << std::endl;
    std::cout << "   mode                           current control mode {0=none, 1=client, 2=admin}"                                            << std::endl;
    std::cout << "   dns                            domain name suffix"                                                                          << std::endl;
    std::cout << "   mac                            wired MAC address"                                                                           << std::endl;
    std::cout << "   cert                           certificate hashes"                                                                          << std::endl;
    std::cout                                                                                                                                    << std::endl;
    std::cout << "Examples:"                                                                                                                     << std::endl;
    std::cout << "  " << PROJECT_NAME << " --url wss://localhost:8080 --cmd \"-t activate --profile profile1\""                                  << std::endl;
    std::cout << "  " << PROJECT_NAME << " -u wss://localhost:8080 -c \"-t deactivate --password P@ssw0rd\" -x http://proxy.com:1000"            << std::endl;
    std::cout << "  " << PROJECT_NAME << " -u wss://localhost:8080 -c \"-e [encrypted-command-text] -h [signature-hash]\""                       << std::endl;
    std::cout                                                                                                                                    << std::endl;
    std::cout << "Note:"                                                                                                                         << std::endl;
    std::cout << "  Since <command> can contain multiple options and arguments, the entire <command> must be in quotes as shown"                 << std::endl;
    std::cout << "  in the examples."                                                                                                            << std::endl;
}

void usage_show_version()
{
    std::cout << PROJECT_NAME << " " << PROJECT_VER << std::endl;
    std::cout << "protocol " << PROTOCOL_VERSION << std::endl;
}