
// Copyright(c) Microsoft.All rights reserved.
// Licensed under the MIT license.See LICENSE file in the project root for full license information.

#include <boost/python.hpp>
#include <string>
#include <vector>
#include <list>


#include "iothub_client.h"
//#include "iothub_message.h"
//#include "crt_abstractions.h"
#include "iothubtransporthttp.h"
#include "iothubtransportamqp.h"
#include "iothubtransportmqtt.h"

enum _TRANSPORT_PROVIDER
{
    HTTP, AMQP, MQTT
};

class IoTHubClient
{

public:

    IOTHUB_CLIENT_HANDLE iotHubClientHandle;

    IoTHubClient()
    {
        iotHubClientHandle = NULL;
    }

    ~IoTHubClient()
    {
        Destroy();
    }

    static
    IoTHubClient const &CreateFromConnectionString(std::string connectionString, _TRANSPORT_PROVIDER _protocol)
    {
        IoTHubClient *iothubObject = new IoTHubClient();
        if (iothubObject != NULL)
        {
            IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = HTTP_Protocol;
            switch (_protocol)
            {
            case HTTP:
                protocol = HTTP_Protocol;
                break;
            case AMQP:
                protocol = AMQP_Protocol;
                break;
            case MQTT:
                protocol = MQTT_Protocol;
                break;
            }

            iothubObject->iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString.c_str(), protocol);
        }
        return *iothubObject;
    }

    static
    IoTHubClient const &Create(const IOTHUB_CLIENT_CONFIG* config)
    {
        IoTHubClient *iothubObject = new IoTHubClient();
        if (iothubObject != NULL)
        {
            iothubObject->iotHubClientHandle = IoTHubClient_Create(config);
        }
        return *iothubObject;
    }

    void Destroy()
    {
        if (iotHubClientHandle != NULL)
        {
            IoTHubClient_Destroy(iotHubClientHandle);
            iotHubClientHandle = NULL;
        }
    }

    IOTHUB_CLIENT_RESULT SendEventAsync(IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback)
    {
        return IoTHubClient_SendEventAsync(iotHubClientHandle, eventMessageHandle, eventConfirmationCallback, userContextCallback);
    }

    IOTHUB_CLIENT_RESULT GetSendStatus(IOTHUB_CLIENT_STATUS *iotHubClientStatus)
    {
        return IoTHubClient_GetSendStatus(iotHubClientHandle, iotHubClientStatus);
    }

    IOTHUB_CLIENT_RESULT SetMessageCallback(IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback)
    {
        return IoTHubClient_SetMessageCallback(iotHubClientHandle, messageCallback, userContextCallback);
    }

    IOTHUB_CLIENT_RESULT GetLastMessageReceiveTime(time_t &lastMessageReceiveTime)
    {
        return IoTHubClient_GetLastMessageReceiveTime(iotHubClientHandle, &lastMessageReceiveTime);
    }

    IOTHUB_CLIENT_RESULT SetOption(std::string optionName, boost::python::object& o)
    {
        int value = boost::python::extract<int>(o);
#ifdef VERIFY_PYTHON_TYPE
        std::string object_classname = boost::python::extract<std::string>(o.attr("__class__").attr("__name__"));
        printf("Object: %s %d\n", object_classname.c_str(), value);
#endif
        return IoTHubClient_SetOption(iotHubClientHandle, optionName.c_str(), &value);
    }
};

using namespace boost::python;

BOOST_PYTHON_MODULE(iothub)
{
    
    enum_<_TRANSPORT_PROVIDER>("transport_provider")
        .value("http", HTTP)
        .value("amqp", AMQP)
        .value("mqtt", MQTT)
        ;

    enum_<IOTHUB_CLIENT_RESULT>("iothub_client_result")
        .value("ok", IOTHUB_CLIENT_OK)
        .value("invalid_arg", IOTHUB_CLIENT_INVALID_ARG)
        .value("error", IOTHUB_CLIENT_ERROR)
        .value("invalid_size", IOTHUB_CLIENT_INVALID_SIZE)
        .value("indefinite_time", IOTHUB_CLIENT_INDEFINITE_TIME)
        ;

    class_<IoTHubClient>("IoTHubClient", no_init)
        .def("CreateFromConnectionString", &IoTHubClient::CreateFromConnectionString, return_value_policy<return_by_value>())
        .staticmethod("CreateFromConnectionString")
        .def("Create", &IoTHubClient::Create, return_value_policy<return_by_value>())
        .staticmethod("Create")
        .def("Destroy", &IoTHubClient::Destroy)
        //.def("SendEventAsync", &IoTHubClient::SendEventAsync)
        .def("GetSendStatus", &IoTHubClient::GetSendStatus)
        //.def("SetMessageCallback", &IoTHubClient::SetMessageCallback)
        .def("GetLastMessageReceiveTime", &IoTHubClient::GetLastMessageReceiveTime)
        .def("SetOption", &IoTHubClient::SetOption)
        ;
};

