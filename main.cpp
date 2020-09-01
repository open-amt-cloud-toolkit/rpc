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

#include <string>
#include <thread>
#include <cpprest/ws_client.h>
#include <cpprest/json.h>
#include "port.h"
#include "lms.h"
#include "commands.h"
#include "activation.h"
#include "utils.h"
#include "usage.h"
#include "args.h"
#include "info.h"

// timer thread globals
long long  g_timeout_val     = 0;
const int  g_timeout_max     = 10;
bool       g_thread_alive    = true;

// timeout thread function
// used to exit application in case a timeout occurs
void timeout_thread_function(std::condition_variable *cv, std::mutex *mx)
{
    while (g_thread_alive)
    {
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        long long currTime = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        if (currTime > g_timeout_val)
        {
            if (currTime - g_timeout_val >= g_timeout_max)
            {
                cv->notify_all();

                // check if timeoutTimer is not 0 since we explicitly set to zero when an
                // activation is successfull. If it's not zero, we are in a time out scenario.
                if (g_timeout_val)
                {
                    std::cout << std::endl << "Timed-out due to inactivity." << std::endl;
                }
                break;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

int main(int argc, char* argv[])
{
    std::string activation_info;
    std::string arg_url;
    std::string arg_proxy;
    std::string arg_cmd;
    std::string arg_dns;
    std::string arg_info;

    if (argc == 1)
    {
        std::cout << "Use -h, --help for help." << std::endl;
        return 0;
    }

    // get for help
    if (args_get_help(argc, argv))
    {
        usage_show_help();
        return 0;
    }

    // check for version
    if (args_get_version(argc, argv))
    {
        usage_show_version();
        return 0;
    }

    // check for info
    if (args_get_info(argc, argv, arg_info))
    {
        if (!info_get(arg_info))
        {
            std::cout << "Incorrect or missing arguments." << std::endl;
            std::cout << "Use -h, --help for help." << std::endl;
        }
        return 0;
    }

    // get required arguments
    if (!args_get_url(argc, argv, arg_url) || !args_get_cmd(argc, argv, arg_cmd))
    {
        std::cout << "Incorrect or missing arguments." << std::endl;
        std::cout << "Use -h, --help for help." << std::endl;
        return 0;
    }

    // Print version info
    usage_show_version();

    try {
        // Get activation info
        if (args_get_dns(argc, argv, arg_dns))
        {
            if (!act_create_request(arg_cmd, arg_dns, activation_info)) throw std::runtime_error("unable to get activation info");
        }
        else
        {
            if (!act_create_request(arg_cmd, "" , activation_info)) throw std::runtime_error("unable to get activation info");
        }
    }
    catch (...)
    {
        std::cerr << std::endl << "Unable to get activation info. Try again later or check AMT configuration." << std::endl;
        return 1;
    }

    try
    {
        // check if LMS is available
        SOCKET s = lms_connect();
        closesocket(s);
    }
    catch (...)
    {
        // start MicroLMS thread
        std::thread main_micro_lms_thread(main_micro_lms);
        main_micro_lms_thread.detach();

        // wait for MicroLMS to startup
        for (int i=0; i<5; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

#ifdef DEBUG
    std::cout << "Activation info: " << std::endl << activation_info << std::endl;
#endif

    // webSocket Interface
    web::websockets::client::websocket_client_config client_config;
    if (args_get_proxy(argc, argv, arg_proxy))
    {
        client_config.set_proxy(web::web_proxy(utility::conversions::to_string_t(arg_proxy)));
    }
#ifdef DEBUG
    // skip certificate verification if debug build
    std::cout << "!!! SKIPPING CERTIFICATE VERIFICATION !!!" << std::endl;
    client_config.set_validate_certificates(false);
#endif
    web::websockets::client::websocket_callback_client client(client_config);
    std::condition_variable cv;
    std::mutex mx;
    SOCKET s;

    // set receive handler
    client.set_message_handler([&client, &mx, &cv, &s](web::websockets::client::websocket_incoming_message ret_msg)
    {
        // kick the timer
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        g_timeout_val = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        try
        {
            // handle message from server
            std::string rcv_websocket_msg = ret_msg.extract_string().get();
#ifdef DEBUG
            std::cout << std::endl << "<<<<< Received Message " << std::endl;
            std::cout << rcv_websocket_msg << std::endl;
#endif
            std::cout << "." << std::flush; // dot status output

            // parse incoming JSON message
            utility::string_t tmp = utility::conversions::convertstring(rcv_websocket_msg);
            web::json::value parsed = web::json::value::parse(tmp);

            utility::string_t out = U("");
            std::string msgMethod = "";
            std::string msgApiKey = "";
            std::string msgAppVersion = "";
            std::string msgProtocolVersion = "";
            std::string msgStatus = "";
            std::string msgMessage = "";
            std::string msgPayload = "";
            std::string payloadDecoded = "";

            if ( !parsed.has_field(U("method"))          || !parsed.has_field(U("apiKey")) || !parsed.has_field(U("appVersion"))  || 
                 !parsed.has_field(U("protocolVersion")) || !parsed.has_field(U("status")) || !parsed.has_field(U("message"))     ||
                 !parsed.has_field(U("payload"))  ) {
                     std::cerr << std::endl << "Received incorrectly formatted message." << std::endl;
                     cv.notify_all();
                     g_thread_alive = false;
                     return;
            }

            try 
            {
                out = parsed[U("method")].as_string();
                msgMethod = utility::conversions::to_utf8string(out);

                out = parsed[U("apiKey")].as_string();
                msgApiKey = utility::conversions::to_utf8string(out);

                out = parsed[U("appVersion")].as_string();
                msgAppVersion = utility::conversions::to_utf8string(out);

                out = parsed[U("protocolVersion")].as_string();
                msgProtocolVersion = utility::conversions::to_utf8string(out);

                out = parsed[U("status")].as_string();
                msgStatus = utility::conversions::to_utf8string(out);

                out = parsed[U("message")].as_string();
                msgMessage = utility::conversions::to_utf8string(out);
            }
            catch (...)
            {
                std::cerr << std::endl << "Received message parse error." << std::endl;
                return;
            }


#ifdef DEBUG
            std::cout << msgMethod << ", " << msgStatus << ", " << msgMessage << std::endl;
            std::cout << rcv_websocket_msg << std::endl;
#endif

            // process any messages we can
            //   - if success, done
            //   - if error, get out
            if (msgMethod.compare("success")==0)
            {
                // cleanup
                g_timeout_val = 0;

                // exit
                std::cout << std::endl << msgMessage << std::endl;
                return;
            }
            else if (msgMethod.compare("error")==0)
            {
                // cleanup
                g_timeout_val = 0;

                // exit
                std::cout << std::endl << msgMessage << std::endl;
                return;
            }

            // process payload afterward since success/error messages have zero length
            // payloads which cause an exception to be thrown
            try 
            {
                out = parsed[U("payload")].as_string();

                if (out.length()>0)
                {
                    msgPayload = utility::conversions::to_utf8string(out);
                    
                    // decode payload
                    payloadDecoded = util_decode_base64(msgPayload);
                }
                else
                {
                    // no payload, nothing to do
                    return;
                }
            }
            catch (...)
            {
                std::cerr << std::endl << "JSON format error. Unable to parse message." << std::endl;
                return;
            }

            try
            {
                // conntect to lms
                s = lms_connect();
            }
            catch (...)
            {
                std::cerr << std::endl << "Unable to connect to Local Management Service (LMS). Please ensure LMS is running." << std::endl;
                return;
            }
            
#ifdef DEBUG
            std::cout << std::endl << "vvvvv Sending Message " << std::endl;
            std::cout << payloadDecoded << std::endl;
#endif        

            // send message to LMS
            if (send(s, payloadDecoded.c_str(), (int)payloadDecoded.length(), 0) < 0)
            {
                throw std::runtime_error("error: socket send");
            }

            // handle response message from LMS
            int fd = ((int) s) + 1;
            fd_set rset;
            FD_ZERO(&rset);
            FD_SET(s, &rset);

            timeval timeout;
            memset(&timeout, 0, sizeof(timeval));
            timeout.tv_sec = 2; 
            timeout.tv_usec = 0;

            // read until connection is closed by LMS
            while (1)
            {
                std::string superBuffer = "";
                while (1)
                {
                    int res = select(fd, &rset, NULL, NULL, &timeout);
                    if (res == 0) 
                    {
                        // we timed-out waiting for the ME. ME usually delivers data very fast. If
                        // we time out, it means that there is no more data that the ME needs to send,
                        // but it's keeping the connection open.
                        break;
                    }

                    // read from LMS
                    char recv_buffer[4096];
                    memset(recv_buffer, 0, 4096);
                    res = recv(s, recv_buffer, 4096, 0);
                    if (res > 0)
                    {
#ifdef DEBUG
                        std::cout << std::endl << "^^^^^ Received Message" << std::endl;
                        std::cout << recv_buffer << std::endl;
#endif
                        superBuffer += recv_buffer;
                    }
                    else if (res < 0)
                    {
                        // on LMS read exception
                        break;
                    }
                    else
                    {
                        // case where res is zero bytes, select returns 1 
                        // with recv returning 0 to indicate close
                        break;
                    }
                } // while select()

                // if there is some data send it
                if (superBuffer.length() > 0) 
                {
                    std::string response;
                    if (!act_create_response(superBuffer.c_str(), response)) return;

                    web::websockets::client::websocket_outgoing_message send_websocket_msg;
                    std::string send_websocket_buffer(response);
                    send_websocket_msg.set_utf8_message(send_websocket_buffer);
#ifdef DEBUG
                    std::cout << std::endl << ">>>>> Sending Message" << std::endl;
                    std::cout << superBuffer << std::endl;
                    std::cout << send_websocket_buffer << std::endl;
#endif
                    client.send(send_websocket_msg).wait();

                    // done
                    closesocket(s);
                    return;
                }
            }

            closesocket(s);
        }
        catch (...)
        {
            std::cerr << std::endl << "Communication error in receive handler." << std::endl;
            closesocket(s);
        }
    });

    // set close handler
    client.set_close_handler([&mx,&cv](web::websockets::client::websocket_close_status status, const utility::string_t &reason, const std::error_code &code)
    {
        // websocket closed by server, notify main thread 
        cv.notify_all();
    });

    try
    {
        // connect to web socket server; AMT activation server
        client.connect(utility::conversions::to_string_t(arg_url)).wait();
    }
    catch (...)
    {
        std::cerr << "Unable to connect to websocket server. Please check url." << std::endl;
        exit(1);
    }

    try 
    {
        // send activationParams to websocket
        web::websockets::client::websocket_outgoing_message out_msg;
        out_msg.set_utf8_message(activation_info);
        
#ifdef DEBUG
        std::cout << std::endl << ">>>>> Sending Activiation Info" << std::endl;
        std::cout << activation_info << std::endl;
#endif        
        client.send(out_msg).wait();
    }
    catch (...)
    {
        std::cerr << std::endl << "Unable to send message to websocket server." << std::endl;
        exit(1);
    }

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    g_timeout_val = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    std::thread timeoutThread(timeout_thread_function, &cv, &mx);

    // wait for server to send success/failure command
    std::unique_lock<std::mutex> lock(mx);
    cv.wait(lock);

    // wait for timeout thread
    timeoutThread.join();

    // clean-up websocket
    client.close().wait();

    // clean-up socket
    if (s) {
        shutdown(s, SD_BOTH);
        closesocket(s);
    }

    exit(0);
}

