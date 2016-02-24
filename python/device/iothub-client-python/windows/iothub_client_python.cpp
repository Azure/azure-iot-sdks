
// Copyright(c) Microsoft.All rights reserved.
// Licensed under the MIT license.See LICENSE file in the project root for full license information.

#include <boost/python.hpp>
#include <string>
#include <vector>
#include <list>

#include "iothub_client_ll.h"
#include "iothub_message.h"
#include "iothubtransporthttp.h"
#include "iothubtransportamqp.h"
#include "iothubtransportmqtt.h"

//#define VERIFY_PYTHON_TYPE 1

// todo 
// * switch to the thread safe iothub API's. Currently using LL api set.
// * exception handling, throw errors instead of return codes?
// * expose transport provider API

enum _TRANSPORT_PROVIDER
{
    HTTP, AMQP, MQTT
};

class Map
{

public:

    MAP_HANDLE mapHandle;

    Map(MAP_HANDLE _mapHandle):
        mapHandle(_mapHandle)
    {
    }

    ~Map()
    {
        // Do not call destroy here, the handle might not be owned
        // by an instance of this class 
    }

    static
    Map *Create(boost::python::object& mapFilterCallback)
    {
        // todo: implement filter callback
        MAP_FILTER_CALLBACK mapFilterFunc = NULL;
        return new Map(Map_Create(mapFilterFunc));
    }

    void Destroy()
    {
        if (mapHandle != NULL)
        {
            Map_Destroy(mapHandle);
            mapHandle = NULL;
        }
    }

    Map *Clone()
    {
        return new Map(Map_Clone(mapHandle));
    }

    MAP_RESULT Add(std::string key, std::string value)
    {
        return Map_Add( mapHandle, key.c_str(), value.c_str());
    }

    MAP_RESULT AddOrUpdate(std::string key, std::string value)
    {
        return Map_AddOrUpdate(mapHandle, key.c_str(), value.c_str());
    }

    MAP_RESULT Delete(std::string key)
    {
        return Map_Delete(mapHandle, key.c_str());
    }

    MAP_RESULT ContainsKey(std::string key, bool* keyExists)
    {
        return Map_ContainsKey(mapHandle, key.c_str(), keyExists);
    }

    MAP_RESULT ContainsValue(std::string value, bool* valueExists)
    {
        return Map_ContainsValue(mapHandle, value.c_str(), valueExists);
    }

    const char *GetValueFromKey(std::string key)
    {
        return Map_GetValueFromKey(mapHandle, key.c_str());
    }

    boost::python::dict GetInternals()
    {
        const char*const* keys;
        const char*const* values;
        boost::python::dict keyValuePair;
        size_t count;
        MAP_RESULT result = Map_GetInternals(mapHandle, &keys, &values, &count);
        if (result == MAP_OK)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                keyValuePair[keys[i]] = values[i];
            }
        }
        // todo: throw on error?
        return keyValuePair;
    }
};

class IoTHubMessage
{
public:

    IOTHUB_MESSAGE_HANDLE iotHubMessageHandle;

    IoTHubMessage(IOTHUB_MESSAGE_HANDLE _iotHubMessageHandle):
        iotHubMessageHandle(_iotHubMessageHandle)
    {
    }

    ~IoTHubMessage()
    {
    }

    static
    IoTHubMessage *CreateFromByteArray(PyObject *pyByteArray)
    {
        if (!PyBuffer_Check(pyByteArray)) {
            // todo: raise TypeError using standard boost::python mechanisms
            return NULL;
        }
        // todo: this conversion is not tested yet
        Py_buffer *py_buffer = (Py_buffer *)pyByteArray;
        const unsigned char* byteArray = (const unsigned char*)py_buffer->buf;
        size_t size = (size_t)py_buffer->len;
        IoTHubMessage *iothubObject = new IoTHubMessage(IoTHubMessage_CreateFromByteArray(byteArray, size));
        return iothubObject;
    }

    static
    IoTHubMessage *CreateFromString(std::string source)
    {
        IoTHubMessage *iothubObject = new IoTHubMessage(IoTHubMessage_CreateFromString(source.c_str()));
        return iothubObject;
    }

    IoTHubMessage *Clone()
    {
        IoTHubMessage *iothubObject = new IoTHubMessage(IoTHubMessage_Clone(iotHubMessageHandle));
        return iothubObject;
    }

    PyObject* GetByteArray()
    {
        const unsigned char* buffer;
        size_t size;
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_GetByteArray(iotHubMessageHandle, &buffer, &size);
        if (result == IOTHUB_MESSAGE_OK)
        {
            return PyBuffer_FromMemory((void*)buffer, size);
        }
        // todo: throw on error?
        return Py_None;
    }

    const char *GetString()
    {
        return IoTHubMessage_GetString(iotHubMessageHandle);
    }

    IOTHUBMESSAGE_CONTENT_TYPE GetContentType()
    {
        return IoTHubMessage_GetContentType(iotHubMessageHandle);
    }

    Map *Properties()
    {
        return new Map(IoTHubMessage_Properties(iotHubMessageHandle));
    }

    const char *GetMessageId()
    {
        return IoTHubMessage_GetMessageId(iotHubMessageHandle);
    }

    IOTHUB_MESSAGE_RESULT SetMessageId(std::string messageId)
    {
        return IoTHubMessage_SetMessageId( iotHubMessageHandle, messageId.c_str());
    }

    const char *GetCorrelationId()
    {
        return IoTHubMessage_GetCorrelationId(iotHubMessageHandle);
    }

    IOTHUB_MESSAGE_RESULT SetCorrelationId(std::string correlationId)
    {
        return IoTHubMessage_SetCorrelationId(iotHubMessageHandle, correlationId.c_str());
    }

    void Destroy()
    {
        if (iotHubMessageHandle != NULL)
        {
            IoTHubMessage_Destroy(iotHubMessageHandle);
            iotHubMessageHandle = NULL;
        }
    }

};

typedef struct
{
    boost::python::object messageCallback;
    boost::python::object userContext;
    IoTHubMessage *eventMessage;
} SendContext;

extern "C" 
static void 
SendConfirmationCallback(
    IOTHUB_CLIENT_CONFIRMATION_RESULT result, 
    void* sendContextCallback
    )
{
    SendContext *sendContext = (SendContext *)sendContextCallback;
    IoTHubMessage *eventMessage = sendContext->eventMessage;
    boost::python::object userContext = sendContext->userContext;
    boost::python::object messageCallback = sendContext->messageCallback;
#ifdef VERIFY_PYTHON_TYPE
    std::string object_classname = boost::python::extract<std::string>(userContext.attr("__class__").attr("__name__"));
    std::string message_classname = boost::python::extract<std::string>(messageCallback.attr("__class__").attr("__name__"));
    printf("SendConfirmationCallback: %s Object: %s\n", message_classname.c_str(), object_classname.c_str());
#endif
    messageCallback(eventMessage, result, userContext);
    delete sendContext;
    delete eventMessage;
}

typedef struct
{
    boost::python::object messageCallback;
    boost::python::object userContext;
} ReceiveContext;

extern "C"
static IOTHUBMESSAGE_DISPOSITION_RESULT 
ReceiveMessageCallback(
    IOTHUB_MESSAGE_HANDLE messageHandle, 
    void* receiveContextCallback
    )
{
    IOTHUBMESSAGE_DISPOSITION_RESULT result = IOTHUBMESSAGE_ACCEPTED;
    ReceiveContext *receiveContext = (ReceiveContext *)receiveContextCallback;
    boost::python::object userContext = receiveContext->userContext;
    boost::python::object messageCallback = receiveContext->messageCallback;
#ifdef VERIFY_PYTHON_TYPE
    std::string object_classname = boost::python::extract<std::string>(userContext.attr("__class__").attr("__name__"));
    std::string message_classname = boost::python::extract<std::string>(messageCallback.attr("__class__").attr("__name__"));
    printf("ReceiveConfirmationCallback: %s Object: %s\n", message_classname.c_str(), object_classname.c_str());
#endif
    IoTHubMessage *message = new IoTHubMessage(messageHandle);
    // todo: result = messageCallback
    boost::python::object returnObject = messageCallback(message, userContext);
    return result;
}

class IoTHubClient
{

public:

    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

    IoTHubClient()
    {
        iotHubClientHandle = NULL;
    }

    ~IoTHubClient()
    {
        Destroy();
    }

    static
    IoTHubClient const *CreateFromConnectionString(
        std::string connectionString, 
        _TRANSPORT_PROVIDER _protocol
        )
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
            iothubObject->iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString.c_str(), protocol);
        }
        return iothubObject;
    }

    // todo: function is not callable from python yet
    static
    IoTHubClient const *Create(
        const IOTHUB_CLIENT_CONFIG* config
        )
    {
        IoTHubClient *iothubObject = new IoTHubClient();
        if (iothubObject != NULL)
        {
            iothubObject->iotHubClientHandle = IoTHubClient_LL_Create(config);
        }
        return iothubObject;
    }

    void Destroy()
    {
        if (iotHubClientHandle != NULL)
        {
            IoTHubClient_LL_Destroy(iotHubClientHandle);
            iotHubClientHandle = NULL;
        }
    }

    IOTHUB_CLIENT_RESULT SendEventAsync(
        IoTHubMessage &eventMessage, 
        boost::python::object& messageCallback,
        boost::python::object& userContext
        )
    {
        SendContext *sendContext = new SendContext();
        sendContext->messageCallback = messageCallback;
        sendContext->userContext = userContext;
        sendContext->eventMessage = new IoTHubMessage(eventMessage.iotHubMessageHandle);
#ifdef VERIFY_PYTHON_TYPE
        std::string message_classname = boost::python::extract<std::string>(messageCallback.attr("__class__").attr("__name__"));
        std::string user_classname = boost::python::extract<std::string>(userContext.attr("__class__").attr("__name__"));
        printf("SendEventAsync: messageCallback: %s userContext: %s\n", message_classname.c_str(), user_classname.c_str());
#endif
        return IoTHubClient_LL_SendEventAsync(iotHubClientHandle, eventMessage.iotHubMessageHandle, SendConfirmationCallback, sendContext);
    }

    IOTHUB_CLIENT_RESULT GetSendStatus(
        IOTHUB_CLIENT_STATUS &iotHubClientStatus
        )
    {
        return IoTHubClient_LL_GetSendStatus(iotHubClientHandle, &iotHubClientStatus);
    }

    void DoWork()
    {
        IoTHubClient_LL_DoWork(iotHubClientHandle);
    }

    IOTHUB_CLIENT_RESULT SetMessageCallback(
        boost::python::object& messageCallback,
        boost::python::object& userContext
        )
    {
        ReceiveContext *receiveContext = new ReceiveContext();
        receiveContext->messageCallback = messageCallback;
        receiveContext->userContext = userContext;
        return IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, receiveContext);
    }

    IOTHUB_CLIENT_RESULT GetLastMessageReceiveTime(
        time_t &lastMessageReceiveTime
        )
    {
        // todo: return PyDateTime_FromDateAndTime
        return IoTHubClient_LL_GetLastMessageReceiveTime(iotHubClientHandle, &lastMessageReceiveTime);
    }

    IOTHUB_CLIENT_RESULT SetOption(
        std::string optionName, 
        boost::python::object& option
        )
    {
        int value = boost::python::extract<int>(option);
#ifdef VERIFY_PYTHON_TYPE
        std::string object_classname = boost::python::extract<std::string>(option.attr("__class__").attr("__name__"));
        printf("Object: %s %d\n", object_classname.c_str(), value);
#endif
        return IoTHubClient_LL_SetOption(iotHubClientHandle, optionName.c_str(), &value);
    }
};

using namespace boost::python;

BOOST_PYTHON_MODULE(iothub)
{
    
    enum_<_TRANSPORT_PROVIDER>("iothub_transport_provider")
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

    enum_<MAP_RESULT>("map_result")
        .value("ok", MAP_OK)
        .value("error", MAP_ERROR)
        .value("invalidarg", MAP_INVALIDARG)
        .value("keyexists", MAP_KEYEXISTS)
        .value("keynotfound", MAP_KEYNOTFOUND)
        .value("filter_reject", MAP_FILTER_REJECT)
        ;

    enum_<IOTHUB_CLIENT_STATUS>("iothub_client_status")
        .value("idle", IOTHUB_CLIENT_SEND_STATUS_IDLE)
        .value("busy", IOTHUB_CLIENT_SEND_STATUS_BUSY)
        ;

    enum_<IOTHUB_CLIENT_CONFIRMATION_RESULT>("iothub_client_confirmation_result")
        .value("ok", IOTHUB_CLIENT_CONFIRMATION_OK)
        .value("because_destroy", IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY)
        .value("error", IOTHUB_CLIENT_CONFIRMATION_ERROR)
        ;

    enum_<IOTHUBMESSAGE_DISPOSITION_RESULT>("iothubmessage_disposition_result")
        .value("accepted", IOTHUBMESSAGE_ACCEPTED)
        .value("rejected", IOTHUBMESSAGE_REJECTED)
        .value("abandoned", IOTHUBMESSAGE_ABANDONED)
        ;

    enum_<IOTHUB_MESSAGE_RESULT>("iothub_message_result")
        .value("ok", IOTHUB_MESSAGE_OK)
        .value("invalid_arg", IOTHUB_MESSAGE_INVALID_ARG)
        .value("invalid_type", IOTHUB_MESSAGE_INVALID_TYPE)
        .value("error", IOTHUB_MESSAGE_ERROR)
        ;

    class_<IoTHubClient>("IoTHubClient", no_init)
        .def("CreateFromConnectionString", &IoTHubClient::CreateFromConnectionString, return_value_policy<manage_new_object>())
        .staticmethod("CreateFromConnectionString")
        .def("Create", &IoTHubClient::Create, return_value_policy<manage_new_object>())
        .staticmethod("Create")
        .def("Destroy", &IoTHubClient::Destroy)
        .def("SendEventAsync", &IoTHubClient::SendEventAsync)
        .def("GetSendStatus", &IoTHubClient::GetSendStatus)
        .def("DoWork", &IoTHubClient::DoWork)
        .def("SetMessageCallback", &IoTHubClient::SetMessageCallback)
        .def("GetLastMessageReceiveTime", &IoTHubClient::GetLastMessageReceiveTime)
        .def("SetOption", &IoTHubClient::SetOption)
        ;

    class_<IoTHubMessage>("IoTHubMessage", no_init)
        .def("CreateFromByteArray", &IoTHubMessage::CreateFromByteArray, return_value_policy<manage_new_object>())
        .staticmethod("CreateFromByteArray")
        .def("CreateFromString", &IoTHubMessage::CreateFromString, return_value_policy<manage_new_object>())
        .staticmethod("CreateFromString")
        .def("Clone", &IoTHubMessage::Clone, return_value_policy<manage_new_object>())
        .def("GetByteArray", &IoTHubMessage::GetByteArray)
        .def("GetString", &IoTHubMessage::GetString)
        .def("GetContentType", &IoTHubMessage::GetContentType)
        .def("Properties", &IoTHubMessage::Properties, return_value_policy<manage_new_object>())
        .def("GetMessageId", &IoTHubMessage::GetMessageId)
        .def("SetMessageId", &IoTHubMessage::SetMessageId)
        .def("GetCorrelationId", &IoTHubMessage::GetCorrelationId)
        .def("SetCorrelationId", &IoTHubMessage::SetCorrelationId)
        .def("Destroy", &IoTHubMessage::Destroy)
        ;

    class_<Map>("Map", no_init)
        .def("Create", &Map::Create, return_value_policy<manage_new_object>())
        .staticmethod("Create")
        .def("Destroy", &Map::Destroy)
        .def("Clone", &Map::Clone, return_value_policy<manage_new_object>())
        .def("Add", &Map::Add)
        .def("AddOrUpdate", &Map::AddOrUpdate)
        .def("Delete", &Map::Delete)
        .def("ContainsKey", &Map::ContainsKey)
        .def("ContainsValue", &Map::ContainsValue)
        .def("GetValueFromKey", &Map::GetValueFromKey)
        .def("GetInternals", &Map::GetInternals)
        ;

};

