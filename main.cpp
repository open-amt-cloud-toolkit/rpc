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

#include <cpprest/ws_client.h>
#include <cpprest/streams.h>
#include <iostream>
#include <string>
#include <thread>
#include <boost/algorithm/string.hpp>
#include "commands.h"
#include "lms.h"
#include "version.h"

using namespace std;
using namespace web;
using namespace web::websockets::client;

#include <cpprest/json.h>

void showUsage();

string websocket_address  = "";
string server_profile = "";
string websocket_proxy = "";

long long timeoutTimer = 0;
const int MAX_TIMEOUT = 10; // seconds
bool timeoutThreadAlive = true;

void timeoutFunc(std::condition_variable *cv, std::mutex *mx)
{
    while (timeoutThreadAlive) 
    {
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        long long currTime = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        if (currTime > timeoutTimer) 
        {
            if (currTime - timeoutTimer >= MAX_TIMEOUT) 
            {
                cv->notify_all();

                // check if timeoutTimer is not 0 since we explicitly set to zero when an
                // activation is successfull. If it's not zero, we are in a time out scenario.
                if (timeoutTimer)
                {
                    cout << endl << "Timed-out due to inactivity." <<endl;
                }
                break;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

int main(int argc, char *argv[])
{
    string activationInfo;

    bool gotURL = false;
    bool gotProfile = false;
    bool gotProxy = false;
    bool gotDns = false;
    
    if (argc==1)
    {
        std::cout << "Use --h, --help for help." << std::endl;
        return 0;
    }

    for (int i=1; i<argc; i++)
    {
        if ( (boost::equals(argv[i], "--help")) || (boost::equals(argv[i], "--h"))  )
        {
            showUsage();
            return 0;
        }
    }

    for (int i=1; i<argc; i++)
    {
        if ( (boost::equals(argv[i], "--url"))  || (boost::equals(argv[i], "--u")) )
        {
            if (i+1<argc) 
            {
                gotURL = true;
                websocket_address = argv[++i];
            }
        } 
        else if ( (boost::equals(argv[i], "--profile")) || (boost::equals(argv[i], "--p")) )
        {
            if (i+1<argc) 
            {
                gotProfile = true;
                server_profile = argv[++i];
            }
        }
        else if ( (boost::equals(argv[i], "--proxy")) ||(boost::equals(argv[i], "--x")) )
        {
            if (i+1<argc) 
            {
                gotProxy = true;
                websocket_proxy = argv[++i];
            }
        }
        else
        {
            std::cout << "Unrecognized command line arguments." << std::endl;
            std::cout << "Use --h, --help for help." << std::endl;
            return 0;
        }
        
    }

    if (!gotURL || !gotProfile)
    {
        std::cout << "Incorrect or missing arguments." << std::endl;
        std::cout << "Use --h, --help for help." << std::endl;
        return 0;
    }

    // Print version info
    cout << PROJECT_NAME << " v" PROJECT_VER << endl;

    try {
        // Get activation info
        activationInfo = createActivationRequest(server_profile);
    }
    catch (...)
    {
        std::cerr << endl << "Unable to get activation info. Check AMT configuration." << endl;
        return 1;
    }

    try
    {
        // Check if LMS is available
        SOCKET s = lmsConnect();
        closesocket(s);
    }
    catch (...)
    {
        std::cerr << endl << "Unable to connect to Local Management Service (LMS). Please ensure LMS is running." << endl;
        return 1;
    }

    // Show activation info
#ifdef DEBUG
    cout << "Activation info: " << endl << activationInfo << endl;
#endif

    // WebSocket Interface
    websocket_client_config client_config;
    if (!websocket_proxy.empty())
    {
        client_config.set_proxy(web::web_proxy(utility::conversions::to_string_t(websocket_proxy)));
    }
#ifdef DEBUG
    // skip certificate verification if debug build
    cout << "!!! SKIPPING CERTIFICATE VERIFICATION !!!" << endl;
    client_config.set_validate_certificates(false);
#endif
    websocket_callback_client client(client_config);
    std::condition_variable cv;
    std::mutex mx;
    SOCKET s;

    // set receive handler
    client.set_message_handler([&client, &mx, &cv, &s](websocket_incoming_message ret_msg) 
    {
        // kick the timer
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        timeoutTimer = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        try
        {
            // handle message from server...
            string rcv_websocket_msg = ret_msg.extract_string().get();
#ifdef DEBUG
            cout << endl << "<<<<< Received Message " << endl;
            cout << rcv_websocket_msg << endl;
#endif
            cout << "." << std::flush; // dot status output

            // parse incoming JSON message
            utility::string_t tmp = utility::conversions::convertstring(rcv_websocket_msg);
            web::json::value parsed = web::json::value::parse(tmp);

            utility::string_t out = U("");
            string msgMethod = "";
            string msgApiKey = "";
            string msgAppVersion = "";
            string msgProtocolVersion = "";
            string msgStatus = "";
            string msgMessage = "";
            string msgPayload = "";
            string payloadDecoded = "";

            if ( !parsed.has_field(U("method"))          || !parsed.has_field(U("apiKey")) || !parsed.has_field(U("appVersion"))  || 
                 !parsed.has_field(U("protocolVersion")) || !parsed.has_field(U("status")) || !parsed.has_field(U("message"))     ||
                 !parsed.has_field(U("payload"))  ) {
                     std::cerr << endl << "Received incorrectly formatted message." << endl;
                     cv.notify_all();
                     timeoutThreadAlive = false;
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
                std::cerr << endl << "Received message parse error." << endl;
                return;
            }


#ifdef DEBUG
            cout << msgMethod << ", " << msgStatus << ", " << msgMessage << endl;
            cout << rcv_websocket_msg << endl;
#endif

            // process any messages we can
            //   - if success, done
            //   - if error, get out
            if (boost::iequals(msgMethod, "success"))
            {
                // cleanup
                timeoutTimer = 0;

                // exit
                cout << endl << msgMessage << endl;
                return;
            }
            else if (boost::iequals(msgMethod, "error"))
            {
                // cleanup
                timeoutTimer = 0;

                // exit
                cout << endl << msgMessage << endl;
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
                    payloadDecoded = decodeBase64(msgPayload);
                }
                else
                {
                    // no payload, nothing to do
                    return;
                }
            }
            catch (...)
            {
                std::cerr << endl << "JSON format error. Unable to parse message." << endl;
                return;
            }

            try
            {
                // conntect to lms
                s = lmsConnect();
            }
            catch (...)
            {
                std::cerr << endl << "Unable to connect to Local Management Service (LMS). Please ensure LMS is running." << endl;
                return;
            }
            
#ifdef DEBUG
        cout << endl << "vvvvv Sending Message " << endl;
        cout << payloadDecoded << endl;        
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
                string superBuffer = "";
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
                        cout << endl << "^^^^^ Received Message" << endl;
                        cout << recv_buffer << endl;
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
                        // case where res is zero bytes
                        // discussion below, but select returns 1 with recv returning 0 to indicate close
                        // https://stackoverflow.com/questions/2992547/waiting-for-data-via-select-not-working
                        break;
                    }
                } // while select()

                // if there is some data send it
                if (superBuffer.length() > 0) 
                {
                    string response = createResponse(superBuffer.c_str());
                    websocket_outgoing_message send_websocket_msg;
                    string send_websocket_buffer(response);
                    send_websocket_msg.set_utf8_message(send_websocket_buffer);
#ifdef DEBUG
                    cout << endl << ">>>>> Sending Message" << endl;
                    cout << superBuffer << endl;
                    cout << send_websocket_buffer << endl;
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
            std::cerr << endl << "Communication error in receive handler." << endl;
            closesocket(s);
        }
    });

    // set close handler
    client.set_close_handler([&mx,&cv](websocket_close_status status, const utility::string_t &reason, const std::error_code &code) 
    {
        // websocket closed by server, notify main thread 
        cv.notify_all();
    });

    try
    {
        // Connect to web socket server; AMT activation server
        client.connect(utility::conversions::to_string_t(websocket_address)).wait();
    }
    catch (...)
    {
        std::cerr << "Unable to connect to websocket server. Please check url." << endl;
        exit(1);
    }

    try 
    {
        // Send activationParams to websocket
        websocket_outgoing_message out_msg;
        out_msg.set_utf8_message(activationInfo);
        
#ifdef DEBUG
        cout << endl << ">>>>> Sending Activiation Info" << endl;
        cout << activationInfo << endl;
#endif        
        client.send(out_msg).wait();
    }
    catch (...)
    {
        std::cerr << endl << "Unable to send message to websocket server." << endl;
        exit(1);
    }

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    timeoutTimer = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    std::thread timeoutThread(timeoutFunc, &cv, &mx);

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

void showUsage()
{
    cout << "Usage: " << PROJECT_NAME << " --url <url> --profile <name> [--proxy <addr>]" << endl;
    cout << "Required:" << endl;
    cout << "  --u, --url <url>                 websocket server" << endl;
    cout << "  --p, --profile <name>            server profile" << endl;
    cout << "Optional:" << endl;
    cout << "  --x, --proxy <addr>              proxy address and port" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "  " << PROJECT_NAME << " --url wss://localhost --profile profile1" << endl;
    cout << "  " << PROJECT_NAME << " --u wss://localhost --profile profile1 --proxy http://proxy.com:1000" << endl;
    cout << "  " << PROJECT_NAME << " --u wss://localhost --p profile1 --x http://proxy.com:1000" << endl;
    cout << endl;
}
