// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <csignal>
#include <future>
#include <unistd.h>

#include "azure_c_shared_utility/platform.h"
#include "iothub_client.hpp"
#include "iothub_message.hpp"

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const std::string _connectionString = "[device connection string]";

static double avgWindSpeed = 10.0;
static bool g_continueRunning = true;

azure::iot::message get_message(int i)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    auto createdAt = std::put_time(&tm, "%FT%T");

    std::stringstream msgText;
    msgText << "{\"deviceId\":\"myFirstDevice\",\"createdAt\":\""<< createdAt << "\",\"windSpeed\":" << avgWindSpeed + (rand() % 4 + 2) << "}";
    auto messageString = msgText.str();
    auto message = azure::iot::message(messageString);
    
    std::stringstream messageIdss;
    messageIdss << i;
    auto messageId = messageIdss.str();
    message.setMessageId(messageId);
    
    auto properties = message.properties();
    
    std::stringstream propText;
    propText << "PropMsg_" << i;
    properties.add("PropName", propText.str());

    std::cout << "Set PropName propertie to " << properties.getValueFromKey("PropName") << std::endl;

    std::cout << "Created message" << messageString << std::endl;

    return message;
}

void iothub_client_sample_amqp_run(std::string connectionString)
{
    std::cout << "Starting the IoTHub client sample AMQP..." << std::endl;
    

    try {
        if (platform_init() != 0)
        {
            throw new std::runtime_error("Failed to init platform");
        }

        std::cout << "Connecting to " << connectionString << std::endl;
        azure::iot::client iotClient(connectionString, azure::iot::transport::AMQP);

        std::cout << "Created iot client\n";

        iotClient.setOption("logtrace", true);

        iotClient.setMessageCallback([](azure::iot::message message, void* context) -> azure::iot::disposition_result {
            auto contentType = message.getContentType();
            if (contentType == IOTHUBMESSAGE_BYTEARRAY)
            {
                size_t size;
                auto byteArray = message.getByteArray(&size);
                std::cout << "received byte array message" << std::endl;
            }
            else if (contentType == IOTHUBMESSAGE_STRING)
            {
                std::cout << "Received message " << message.getString() << std::endl;
            }
            return azure::iot::ACCEPTED;
        }, nullptr);

        std::cout << "IoTHubClient_SetMessageCallback...successful.\r\n";

        for (int iterator = 0; iterator < 1; ++iterator)
        {
            auto message = get_message(iterator);
            
            iotClient.sendEventAsync(message, [=](const azure::iot::confirmation_result result, const void* context) {
                std::cout << "Send message result " << result << std::endl;
                std::cout << "For message " << message.getMessageId() << std::endl;
            }, nullptr);

            std::async([&]{
                iotClient.doWork();
            });
        }

        std::cout << "Waiting\n";

        while(g_continueRunning) {
            iotClient.doWork();
            sleep(1);
        }

    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    platform_deinit();
}

void signalHandler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received...shutting down\n";

    // cleanup and close up stuff here
    // terminate program

    g_continueRunning = false;
}

int main(int argc, char *argv[])
{
    (void)signal(SIGINT, signalHandler);
    (void)signal(SIGKILL, signalHandler);

    auto connectionString = (argc == 2) ? argv[1] : _connectionString;

    iothub_client_sample_amqp_run(connectionString);

    return 0;
}
