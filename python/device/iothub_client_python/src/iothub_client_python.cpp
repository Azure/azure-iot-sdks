
// Copyright(c) Microsoft.All rights reserved.
// Licensed under the MIT license.See LICENSE file in the project root for full license information.

#include <boost/python.hpp>
#include <string>
#include <vector>
#include <list>

#include "azure_c_shared_utility/platform.h"
#include "iothub_client.h"
#include "iothub_client_version.h"
#include "iothub_message.h"
#include "iothubtransporthttp.h"
#include "iothubtransportamqp.h"
#include "iothubtransportmqtt.h"

#ifndef IMPORT_NAME
#define IMPORT_NAME iothub_client
#endif

#if PY_MAJOR_VERSION >= 3
#define IS_PY3
#endif

//#define SUPPORT___STR__

// helper for debugging py objects
#define PRINT_PYTHON_OBJECT_NAME(obj)\
{\
    std::string object_classname = boost::python::extract<std::string>(obj.attr("__class__").attr("__name__"));\
    printf("Object: %s\n", object_classname.c_str());\
}

//
// note: all new allocations throw a std::bad_alloc exception which trigger a MemoryError in Python
//

// helper to suppress AMQP console messages, comment function to get the output to console
extern "C"
void consolelogger_log(unsigned int options, char* format, ...)
{}

// helper classes for transitions between Python and C++ layer

class ScopedGILAcquire
{
public:
    inline ScopedGILAcquire()
    {
        gil_state = PyGILState_Ensure();
    }

    inline ~ScopedGILAcquire()
    {
        PyGILState_Release(gil_state);
    }
private:
    PyGILState_STATE gil_state;
};

class ScopedGILRelease
{
public:
    inline ScopedGILRelease()
    {
        thread_state = PyEval_SaveThread();
    }

    inline ~ScopedGILRelease()
    {
        PyEval_RestoreThread(thread_state);
        thread_state = NULL;
    }
private:
    PyThreadState * thread_state;
};

//
// iothubtransportxxxx.h
//

enum IOTHUB_TRANSPORT_PROVIDER
{
    HTTP, AMQP, MQTT
};

//
//  IotHubError exception handler base class
//

PyObject* iotHubErrorType = NULL;

class IoTHubError : public std::exception
{
public:
    IoTHubError(
        std::string _exc,
        std::string _cls,
        std::string _func
        )
    {
        exc = _exc;
        cls = _cls;
        // only display class names in camel case
        func = (_func[0] != 'I') ? CamelToPy(_func) : _func;
    }

    std::string exc;
    std::string cls;
    std::string func;

    std::string str() const
    {
        std::stringstream s;
        s << cls << "." << func << ", " << decode_error();
        return s.str();
    }

    std::string repr() const
    {
        std::stringstream s;
        s << exc << "(" << str() << ")";
        return s.str();
    }

    virtual std::string decode_error() const = 0;

private:

    std::string CamelToPy(std::string &func)
    {
        std::string py;
        std::string::size_type len = func.length();
        for (std::string::size_type i = 0; i < len; i++)
        {
            char ch = func[i];
            if ((i > 0) && isupper(ch))
            {
                py.push_back('_');
            }
            py.push_back(tolower(ch));
        }
        return py;
    }

};

PyObject*
createExceptionClass(
    const char* name,
    PyObject* baseTypeObj = PyExc_Exception
    )
{
    namespace bp = boost::python;
    using std::string;
    string scopeName = bp::extract<string>(bp::scope().attr("__name__"));
    string qualifiedName0 = scopeName + "." + name;
    char* qualifiedName1 = const_cast<char*>(qualifiedName0.c_str());

    PyObject* typeObj = PyErr_NewException(qualifiedName1, baseTypeObj, 0);
    if (!typeObj) bp::throw_error_already_set();
    bp::scope().attr(name) = bp::handle<>(bp::borrowed(typeObj));

    return typeObj;
}

//
//  map.h
//

//
//  IoTHubMapError
//

PyObject *iotHubMapErrorType = NULL;

class IoTHubMapError : public IoTHubError
{
public:
    IoTHubMapError(
        std::string _func,
        MAP_RESULT _result
        ) :
        IoTHubError(
            "IoTHubMapError",
            "IoTHubMap",
            _func
            )
    {
        result = _result;
    }

    MAP_RESULT result;

    virtual std::string decode_error() const
    {
        std::stringstream s;
        s << "IoTHubMapResult.";
        switch (result)
        {
        case MAP_OK: s << "OK"; break;
        case MAP_ERROR: s << "ERROR"; break;
        case MAP_INVALIDARG: s << "INVALIDARG"; break;
        case MAP_KEYEXISTS: s << "KEYEXISTS"; break;
        case MAP_KEYNOTFOUND: s << "KEYNOTFOUND"; break;
        case MAP_FILTER_REJECT: s << "FILTER_REJECT"; break;
        }
        return s.str();
    }

};

void iotHubMapError(const IoTHubMapError& x)
{
    boost::python::object pythonExceptionInstance(x);
    PyErr_SetObject(iotHubMapErrorType, pythonExceptionInstance.ptr());
};

// callback function
boost::python::object mapFilterCallback;

extern "C"
int
MapFilterCallback(
    const char* mapProperty,
    const char* mapValue
    )
{
    boost::python::object returnObject;
    {
        ScopedGILAcquire acquire;
        try {
            returnObject = mapFilterCallback(mapProperty, mapValue);
        }
        catch (const boost::python::error_already_set)
        {
            // Catch and ignore exception that is thrown in Python callback.
            // There is nothing we can do about it here.
            PyErr_Print();
        }
    }
    return boost::python::extract<int>(returnObject);
}

class IoTHubMap
{

    bool filter;
    MAP_HANDLE mapHandle;

public:

    IoTHubMap(const IoTHubMap& map) :
        filter(false)
    {
        mapHandle = Map_Clone(map.mapHandle);
        if (mapHandle == NULL)
        {
            throw IoTHubMapError(__func__, MAP_ERROR);
        }
    }

    IoTHubMap() :
        filter(false)
    {
        mapHandle = Map_Create(NULL);
        if (mapHandle == NULL)
        {
            throw IoTHubMapError(__func__, MAP_ERROR);
        }
    }

    IoTHubMap(boost::python::object &_mapFilterCallback) :
        filter(false),
        mapHandle(NULL)
    {
        MAP_FILTER_CALLBACK mapFilterFunc = NULL;
        if (PyCallable_Check(_mapFilterCallback.ptr()))
        {
            if (PyCallable_Check(mapFilterCallback.ptr()))
            {
                PyErr_SetString(PyExc_TypeError, "Filter already in use");
                boost::python::throw_error_already_set();
                return;
            }
            else
            {
                mapFilterCallback = _mapFilterCallback;
                mapFilterFunc = &MapFilterCallback;
                filter = true;
            }
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "expected type callable");
            boost::python::throw_error_already_set();
            return;
        }
        mapHandle = Map_Create(mapFilterFunc);
        if (mapHandle == NULL)
        {
            throw IoTHubMapError(__func__, MAP_ERROR);
        }
    }

    IoTHubMap(MAP_HANDLE _mapHandle) :
        filter(false),
        mapHandle(_mapHandle)
    {
        if (mapHandle == NULL)
        {
            throw IoTHubMapError(__func__, MAP_ERROR);
        }
    }

    ~IoTHubMap()
    {
        Destroy();
        if (filter)
        {
            mapFilterCallback = boost::python::object();
        }
    }

    static IoTHubMap *Create(boost::python::object& _mapFilterCallback)
    {
        MAP_FILTER_CALLBACK mapFilterFunc = NULL;
        if (PyCallable_Check(_mapFilterCallback.ptr()))
        {
            mapFilterCallback = _mapFilterCallback;
            mapFilterFunc = &MapFilterCallback;
        }
        else if (Py_None != _mapFilterCallback.ptr())
        {
            PyErr_SetString(PyExc_TypeError, "Create expected type callable or None");
            boost::python::throw_error_already_set();
            return NULL;
        }
        return new IoTHubMap(Map_Create(mapFilterFunc));
    }

    void Destroy()
    {
        if (mapHandle != NULL)
        {
            Map_Destroy(mapHandle);
            mapHandle = NULL;
        }
    }

    IoTHubMap *Clone()
    {
        return new IoTHubMap(Map_Clone(mapHandle));
    }

    void Add(std::string key, std::string value)
    {
        MAP_RESULT result = Map_Add(mapHandle, key.c_str(), value.c_str());
        if (result != MAP_OK)
        {
            throw IoTHubMapError(__func__, result);
        }
    }

    void AddOrUpdate(std::string key, std::string value)
    {
        MAP_RESULT result = Map_AddOrUpdate(mapHandle, key.c_str(), value.c_str());
        if (result != MAP_OK)
        {
            throw IoTHubMapError(__func__, result);
        }
    }

    void Delete(std::string key)
    {
        MAP_RESULT result = Map_Delete(mapHandle, key.c_str());
        if (result != MAP_OK)
        {
            throw IoTHubMapError(__func__, result);
        }
    }

    bool ContainsKey(std::string key)
    {
        MAP_RESULT result;
        bool keyExists;
        result = Map_ContainsKey(mapHandle, key.c_str(), &keyExists);
        if (result != MAP_OK)
        {
            throw IoTHubMapError(__func__, result);
        }
        return keyExists;
    }

    bool ContainsValue(std::string value)
    {
        bool valueExists;
        MAP_RESULT result = Map_ContainsValue(mapHandle, value.c_str(), &valueExists);
        if (result != MAP_OK)
        {
            throw IoTHubMapError(__func__, result);
        }
        return valueExists;
    }

    const char *GetValueFromKey(std::string key)
    {
        const char *result = Map_GetValueFromKey(mapHandle, key.c_str());
        if (result == NULL)
        {
            throw IoTHubMapError(__func__, MAP_KEYNOTFOUND);
        }
        return result;
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
        else
        {
            throw IoTHubMapError(__func__, result);
        }
        return keyValuePair;
    }

#ifdef SUPPORT___STR__
    std::string str() const
    {
        std::stringstream s;
        // todo
        return s.str();
    }

    std::string repr() const
    {
        std::stringstream s;
        s << "IoTHubMap(" << str() << ")";
        return s.str();
    }
#endif
};

//
//  iothub_message.h
//

PyObject *iothubMessageErrorType = NULL;

class IoTHubMessageError : public IoTHubError
{
public:
    IoTHubMessageError(
        std::string _func,
        IOTHUB_MESSAGE_RESULT _result
        ) :
        IoTHubError(
            "IoTHubMessageError",
            "IoTHubMessage",
            _func
            )
    {
        result = _result;
    }

    IOTHUB_MESSAGE_RESULT result;

    std::string decode_error() const
    {
        std::stringstream s;
        s << "IoTHubMessageResult.";
        switch (result)
        {
        case IOTHUB_MESSAGE_OK: s << "OK"; break;
        case IOTHUB_MESSAGE_INVALID_ARG: s << "INVALID_ARG"; break;
        case IOTHUB_MESSAGE_INVALID_TYPE: s << "INVALID_TYPE"; break;
        case IOTHUB_MESSAGE_ERROR: s << "ERROR"; break;
        }
        return s.str();
    }

};

void iothubMessageError(const IoTHubMessageError& x)
{
    boost::python::object pythonExceptionInstance(x);
    PyErr_SetObject(iothubMessageErrorType, pythonExceptionInstance.ptr());
};

class IoTHubMessage
{

    IOTHUB_MESSAGE_HANDLE iotHubMessageHandle;

public:

    IoTHubMessage(const IoTHubMessage& message)
    {
        iotHubMessageHandle = IoTHubMessage_Clone(message.iotHubMessageHandle);
        if (iotHubMessageHandle == NULL)
        {
            throw IoTHubMessageError(__func__, IOTHUB_MESSAGE_ERROR);
        }
    }

    IoTHubMessage(IOTHUB_MESSAGE_HANDLE _iotHubMessageHandle) :
        iotHubMessageHandle(_iotHubMessageHandle)
    {
        if (_iotHubMessageHandle == NULL)
        {
            throw IoTHubMessageError(__func__, IOTHUB_MESSAGE_ERROR);
        }
    }

    IoTHubMessage(std::string source)
    {
        iotHubMessageHandle = IoTHubMessage_CreateFromString(source.c_str());
        if (iotHubMessageHandle == NULL)
        {
            throw IoTHubMessageError(__func__, IOTHUB_MESSAGE_ERROR);
        }
    }

    IoTHubMessage(PyObject *pyObject)
    {
        if (!PyByteArray_Check(pyObject)) {
            PyErr_SetString(PyExc_TypeError, "expected type bytearray");
            boost::python::throw_error_already_set();
            return;
        }
        PyByteArrayObject *pyByteArray = (PyByteArrayObject *)pyObject;
        const unsigned char* byteArray = (const unsigned char*)pyByteArray->ob_bytes;
        size_t size = Py_SIZE(pyByteArray);
        iotHubMessageHandle = IoTHubMessage_CreateFromByteArray(byteArray, size);
        if (iotHubMessageHandle == NULL)
        {
            throw IoTHubMessageError(__func__, IOTHUB_MESSAGE_ERROR);
        }
    }

    ~IoTHubMessage()
    {
        Destroy();
    }

    static IoTHubMessage *CreateFromByteArray(PyObject *pyObject)
    {
        if (!PyByteArray_Check(pyObject)) {
            PyErr_SetString(PyExc_TypeError, "CreateFromByteArray expected type bytearray");
            boost::python::throw_error_already_set();
            return NULL;
        }
        PyByteArrayObject *pyByteArray = (PyByteArrayObject *)pyObject;
        const unsigned char* byteArray = (const unsigned char*)pyByteArray->ob_bytes;
        size_t size = Py_SIZE(pyByteArray);
        return new IoTHubMessage(IoTHubMessage_CreateFromByteArray(byteArray, size));
    }

    static IoTHubMessage *CreateFromString(std::string source)
    {
        return new IoTHubMessage(IoTHubMessage_CreateFromString(source.c_str()));
    }

    IoTHubMessage *Clone()
    {
        return new IoTHubMessage(IoTHubMessage_Clone(iotHubMessageHandle));
    }

    PyObject* GetBytearray()
    {
        const unsigned char* buffer;
        size_t size;
        PyObject* pyObject = Py_None;
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_GetByteArray(iotHubMessageHandle, &buffer, &size);
        if (result == IOTHUB_MESSAGE_OK)
        {
            pyObject = PyByteArray_FromStringAndSize((char*)buffer, size);
        }
        else
        {
            throw IoTHubMessageError(__func__, result);
        }
        return pyObject;
    }

    const char *GetString()
    {
        return IoTHubMessage_GetString(iotHubMessageHandle);
    }

    IOTHUBMESSAGE_CONTENT_TYPE GetContentType()
    {
        return IoTHubMessage_GetContentType(iotHubMessageHandle);
    }

    IoTHubMap *Properties()
    {
        return new IoTHubMap(Map_Clone(IoTHubMessage_Properties(iotHubMessageHandle)));
    }

    const char *GetMessageId()
    {
        return IoTHubMessage_GetMessageId(iotHubMessageHandle);
    }

    void SetMessageId(std::string messageId)
    {
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetMessageId(iotHubMessageHandle, messageId.c_str());
        if (result != IOTHUB_MESSAGE_OK)
        {
            throw IoTHubMessageError(__func__, result);
        }
    }

    const char *GetCorrelationId()
    {
        return IoTHubMessage_GetCorrelationId(iotHubMessageHandle);
    }

    void SetCorrelationId(std::string correlationId)
    {
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetCorrelationId(iotHubMessageHandle, correlationId.c_str());
        if (result != IOTHUB_MESSAGE_OK)
        {
            throw IoTHubMessageError(__func__, result);
        }
    }

    void Destroy()
    {
        if (iotHubMessageHandle != NULL)
        {
            IoTHubMessage_Destroy(iotHubMessageHandle);
            iotHubMessageHandle = NULL;
        }
    }

    IOTHUB_MESSAGE_HANDLE Handle()
    {
        return iotHubMessageHandle;
    }

#ifdef SUPPORT___STR__
    std::string str() const
    {
        std::stringstream s;
        return s.str();
    }

    std::string repr() const
    {
        std::stringstream s;
        s << "IoTHubMessage(" << str() << ")";
        return s.str();
    }
#endif
};

//
//  iothub_client.h
//

PyObject *iothubClientErrorType = NULL;

class IoTHubClientError : public IoTHubError
{
public:
    IoTHubClientError(
        std::string _func,
        IOTHUB_CLIENT_RESULT _result
        ) :
        IoTHubError(
            "IoTHubClientError",
            "IoTHubClient",
            _func
            )
    {
        result = _result;
    }

    IOTHUB_CLIENT_RESULT result;

    virtual std::string decode_error() const
    {
        std::stringstream s;
        s << "IoTHubClientResult.";
        switch (result)
        {
        case IOTHUB_CLIENT_OK: s << "OK"; break;
        case IOTHUB_CLIENT_INVALID_ARG: s << "INVALID_ARG"; break;
        case IOTHUB_CLIENT_ERROR: s << "ERROR"; break;
        case IOTHUB_CLIENT_INVALID_SIZE: s << "INVALID_SIZE"; break;
        case IOTHUB_CLIENT_INDEFINITE_TIME: s << "INDEFINITE_TIME"; break;
        }
        return s.str();
    }

};

void iothubClientError(const IoTHubClientError& x)
{
    boost::python::object pythonExceptionInstance(x);
    PyErr_SetObject(iothubClientErrorType, pythonExceptionInstance.ptr());
};


// class exposed to py used as parameter for Create

struct IOTHUB_CONFIG
{
    IOTHUB_CONFIG(IOTHUB_TRANSPORT_PROVIDER _protocol) :
        protocol(_protocol)
    {
    }

    IOTHUB_TRANSPORT_PROVIDER protocol;
    std::string deviceId;
    std::string deviceKey;
    std::string iotHubName;
    std::string iotHubSuffix;
    std::string protocolGatewayHostName;
};

// callbacks

typedef struct
{
    boost::python::object messageCallback;
    boost::python::object userContext;
    IoTHubMessage *eventMessage;
} SendContext;

extern "C"
void
SendConfirmationCallback(
    IOTHUB_CLIENT_CONFIRMATION_RESULT result,
    void* sendContextCallback
    )
{
    SendContext *sendContext = (SendContext *)sendContextCallback;
    IoTHubMessage *eventMessage = sendContext->eventMessage;
    boost::python::object userContext = sendContext->userContext;
    boost::python::object messageCallback = sendContext->messageCallback;
    {
        ScopedGILAcquire acquire;
        try {
            messageCallback(eventMessage, result, userContext);
        }
        catch (const boost::python::error_already_set)
        {
            // Catch and ignore exception that is thrown in Python callback.
            // There is nothing we can do about it here.
            PyErr_Print();
        }
    }
    delete sendContext;
    delete eventMessage;
}

typedef struct
{
    boost::python::object messageCallback;
    boost::python::object userContext;
} ReceiveContext;

extern "C"
IOTHUBMESSAGE_DISPOSITION_RESULT
ReceiveMessageCallback(
    IOTHUB_MESSAGE_HANDLE messageHandle,
    void* receiveContextCallback
    )
{
    boost::python::object returnObject;
    ReceiveContext *receiveContext = (ReceiveContext *)receiveContextCallback;
    boost::python::object userContext = receiveContext->userContext;
    boost::python::object messageCallback = receiveContext->messageCallback;
    IoTHubMessage *message = new IoTHubMessage(IoTHubMessage_Clone(messageHandle));
    {
        ScopedGILAcquire acquire;
        try {
            returnObject = messageCallback(message, userContext);
        }
        catch (const boost::python::error_already_set)
        {
            // Catch and ignore exception that is thrown in Python callback.
            // There is nothing we can do about it here.
            PyErr_Print();
        }
    }
    delete message;
    return boost::python::extract<IOTHUBMESSAGE_DISPOSITION_RESULT>(returnObject);
}


class IoTHubClient
{

    static IOTHUB_CLIENT_TRANSPORT_PROVIDER
        GetProtocol(IOTHUB_TRANSPORT_PROVIDER _protocol)
    {
        IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = NULL;
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
        default:
            PyErr_SetString(PyExc_TypeError, "IoTHubTransportProvider set to unknown protocol");
            boost::python::throw_error_already_set();
            break;
        }
        return protocol;
    }

    IOTHUB_CLIENT_HANDLE iotHubClientHandle;

public:

    IOTHUB_TRANSPORT_PROVIDER protocol;

    IoTHubClient(const IoTHubClient& client)
    {
        throw IoTHubClientError(__func__, IOTHUB_CLIENT_ERROR);
    }

    IoTHubClient(
        IOTHUB_CLIENT_HANDLE _iotHubClientHandle,
        IOTHUB_TRANSPORT_PROVIDER _protocol
        ) :
        iotHubClientHandle(_iotHubClientHandle),
        protocol(_protocol)
    {
        if (_iotHubClientHandle == NULL)
        {
            throw IoTHubClientError(__func__, IOTHUB_CLIENT_ERROR);
        }
    }

    IoTHubClient(
        std::string connectionString,
        IOTHUB_TRANSPORT_PROVIDER _protocol
        ) :
        protocol(_protocol)
    {
        iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString.c_str(), GetProtocol(_protocol));
        if (iotHubClientHandle == NULL)
        {
            throw IoTHubClientError(__func__, IOTHUB_CLIENT_ERROR);
        }
    }

    IoTHubClient(
        const IOTHUB_CONFIG &_config
        )
    {
        IOTHUB_CLIENT_CONFIG config;
        config.protocol = GetProtocol(_config.protocol);
        config.deviceId = _config.deviceId.c_str();
        config.deviceKey = _config.deviceKey.c_str();
        config.iotHubName = _config.iotHubName.c_str();
        config.iotHubSuffix = _config.iotHubSuffix.c_str();
        config.protocolGatewayHostName = _config.protocolGatewayHostName.c_str();
        protocol = _config.protocol;
        iotHubClientHandle = IoTHubClient_Create(&config);
        if (iotHubClientHandle == NULL)
        {
            throw IoTHubClientError(__func__, IOTHUB_CLIENT_ERROR);
        }
    }

    ~IoTHubClient()
    {
        Destroy();
    }

    static IoTHubClient const *CreateFromConnectionString(
        std::string connectionString,
        IOTHUB_TRANSPORT_PROVIDER _protocol
        )
    {
        return new IoTHubClient(IoTHubClient_CreateFromConnectionString(connectionString.c_str(), GetProtocol(_protocol)), _protocol);
    }

    static IoTHubClient const *Create(
        const IOTHUB_CONFIG* _config
        )
    {
        IOTHUB_CLIENT_CONFIG config;
        config.protocol = GetProtocol(_config->protocol);
        config.deviceId = _config->deviceId.c_str();
        config.deviceKey = _config->deviceKey.c_str();
        config.iotHubName = _config->iotHubName.c_str();
        config.iotHubSuffix = _config->iotHubSuffix.c_str();
        config.protocolGatewayHostName = _config->protocolGatewayHostName.c_str();
        return new IoTHubClient(IoTHubClient_Create(&config), _config->protocol);
    }

    void Destroy()
    {
        if (iotHubClientHandle != NULL)
        {
            IoTHubClient_Destroy(iotHubClientHandle);
            iotHubClientHandle = NULL;
        }
    }

    void SendEventAsync(
        IoTHubMessage &eventMessage,
        boost::python::object& messageCallback,
        boost::python::object& userContext
        )
    {
        if (!PyCallable_Check(messageCallback.ptr()))
        {
            PyErr_SetString(PyExc_TypeError, "send_event_async expected type callable");
            boost::python::throw_error_already_set();
            return;
        }
        IOTHUB_CLIENT_RESULT result;
        SendContext *sendContext = new SendContext();
        sendContext->messageCallback = messageCallback;
        sendContext->userContext = userContext;
        sendContext->eventMessage = eventMessage.Clone();
        {
            ScopedGILRelease release;
            result = IoTHubClient_SendEventAsync(iotHubClientHandle, eventMessage.Handle(), SendConfirmationCallback, sendContext);
        }
        if (result != IOTHUB_CLIENT_OK)
        {
            throw IoTHubClientError(__func__, result);
        }
    }

    IOTHUB_CLIENT_STATUS GetSendStatus()
    {
        IOTHUB_CLIENT_STATUS iotHubClientStatus;
        IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
        {
            ScopedGILRelease release;
            result = IoTHubClient_GetSendStatus(iotHubClientHandle, &iotHubClientStatus);
        }
        if (result != IOTHUB_CLIENT_OK)
        {
            throw IoTHubClientError(__func__, result);
        }
        return iotHubClientStatus;
    }

    void SetMessageCallback(
        boost::python::object& messageCallback,
        boost::python::object& userContext
        )
    {
        if (!PyCallable_Check(messageCallback.ptr()))
        {
            PyErr_SetString(PyExc_TypeError, "set_message_callback expected type callable");
            boost::python::throw_error_already_set();
            return;
        }
        ReceiveContext *receiveContext = new ReceiveContext();
        receiveContext->messageCallback = messageCallback;
        receiveContext->userContext = userContext;
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, receiveContext);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw IoTHubClientError(__func__, result);
        }
    }

    time_t GetLastMessageReceiveTime()
    {
        time_t lastMessageReceiveTime;
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetLastMessageReceiveTime(iotHubClientHandle, &lastMessageReceiveTime);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw IoTHubClientError(__func__, result);
        }
        return lastMessageReceiveTime;
    }

    void SetOption(
        std::string optionName,
        boost::python::object& option
        )
    {
        IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
#ifdef IS_PY3
        if (PyUnicode_Check(option.ptr()))
        {
            std::string stringValue = boost::python::extract<std::string>(option);
            result = IoTHubClient_SetOption(iotHubClientHandle, optionName.c_str(), stringValue.c_str());
        }
        else if (PyLong_Check(option.ptr()))
        {
            long value = boost::python::extract<long>(option);
            result = IoTHubClient_SetOption(iotHubClientHandle, optionName.c_str(), &value);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "set_option expected type long or unicode");
            boost::python::throw_error_already_set();
        }
#else
        if (PyString_Check(option.ptr()))
        {
            std::string stringValue = boost::python::extract<std::string>(option);
            result = IoTHubClient_SetOption(iotHubClientHandle, optionName.c_str(), stringValue.c_str());
        }
        else if (PyInt_Check(option.ptr()))
        {
            int value = boost::python::extract<int>(option);
            result = IoTHubClient_SetOption(iotHubClientHandle, optionName.c_str(), &value);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "set_option expected type int or string");
            boost::python::throw_error_already_set();
        }
#endif
        if (result != IOTHUB_CLIENT_OK)
        {
            throw IoTHubClientError(__func__, result);
        }
    }

#ifdef SUPPORT___STR__
    std::string str() const
    {
        std::stringstream s;
        return s.str();
    }

    std::string repr() const
    {
        std::stringstream s;
        s << "IoTHubClient(" << str() << ")";
        return s.str();
    }
#endif
};

using namespace boost::python;

static const char* iothub_client_docstring =
"iothub_client is a Python module for communicating with the Azure IoT Hub";

BOOST_PYTHON_MODULE(IMPORT_NAME)
{
    // init threads for callbacks
    PyEval_InitThreads();
    platform_init();

    // by default enable user, py docstring options, disable c++ signatures
    bool show_user_defined = true;
    bool show_py_signatures = true;
    bool show_cpp_signatures = false;
    docstring_options doc_options(show_user_defined, show_py_signatures, show_cpp_signatures);
    scope().attr("__doc__") = iothub_client_docstring;
    scope().attr("__version__") = IOTHUB_SDK_VERSION;

    // exception handlers
    class_<IoTHubMapError>IoTHubMapErrorClass("IoTHubMapErrorArg", init<std::string, MAP_RESULT>());
    IoTHubMapErrorClass.def_readonly("result", &IoTHubMapError::result);
    IoTHubMapErrorClass.def_readonly("func", &IoTHubMapError::func);
    IoTHubMapErrorClass.def("__str__", &IoTHubMapError::str);
    IoTHubMapErrorClass.def("__repr__", &IoTHubMapError::repr);

    class_<IoTHubMessageError>IotHubMessageErrorClass("IoTHubMessageErrorArg", init<std::string, IOTHUB_MESSAGE_RESULT>());
    IotHubMessageErrorClass.def_readonly("result", &IoTHubMessageError::result);
    IotHubMessageErrorClass.def_readonly("func", &IoTHubMessageError::func);
    IotHubMessageErrorClass.def("__str__", &IoTHubMessageError::str);
    IotHubMessageErrorClass.def("__repr__", &IoTHubMessageError::repr);

    class_<IoTHubClientError>IotHubClientErrorClass("IoTHubClientErrorArg", init<std::string, IOTHUB_CLIENT_RESULT>());
    IotHubClientErrorClass.def_readonly("result", &IoTHubClientError::result);
    IotHubClientErrorClass.def_readonly("func", &IoTHubClientError::func);
    IotHubClientErrorClass.def("__str__", &IoTHubClientError::str);
    IotHubClientErrorClass.def("__repr__", &IoTHubClientError::repr);

    register_exception_translator<IoTHubMapError>(iotHubMapError);
    register_exception_translator<IoTHubMessageError>(iothubMessageError);
    register_exception_translator<IoTHubClientError>(iothubClientError);

    // iothub errors derived from BaseException --> IoTHubError --> IoTHubXXXError
    iotHubErrorType = createExceptionClass("IoTHubError");
    iotHubMapErrorType = createExceptionClass("IoTHubMapError", iotHubErrorType);
    iothubMessageErrorType = createExceptionClass("IoTHubMessageError", iotHubErrorType);
    iothubClientErrorType = createExceptionClass("IoTHubClientError", iotHubErrorType);

    // iothub return codes
    enum_<MAP_RESULT>("IoTHubMapResult")
        .value("OK", MAP_OK)
        .value("ERROR", MAP_ERROR)
        .value("INVALIDARG", MAP_INVALIDARG)
        .value("KEYEXISTS", MAP_KEYEXISTS)
        .value("KEYNOTFOUND", MAP_KEYNOTFOUND)
        .value("FILTER_REJECT", MAP_FILTER_REJECT)
        ;

    enum_<IOTHUB_MESSAGE_RESULT>("IoTHubMessageResult")
        .value("OK", IOTHUB_MESSAGE_OK)
        .value("INVALID_ARG", IOTHUB_MESSAGE_INVALID_ARG)
        .value("INVALID_TYPE", IOTHUB_MESSAGE_INVALID_TYPE)
        .value("ERROR", IOTHUB_MESSAGE_ERROR)
        ;

    enum_<IOTHUB_CLIENT_RESULT>("IoTHubClientResult")
        .value("OK", IOTHUB_CLIENT_OK)
        .value("INVALID_ARG", IOTHUB_CLIENT_INVALID_ARG)
        .value("ERROR", IOTHUB_CLIENT_ERROR)
        .value("INVALID_SIZE", IOTHUB_CLIENT_INVALID_SIZE)
        .value("INDEFINITE_TIME", IOTHUB_CLIENT_INDEFINITE_TIME)
        ;

    enum_<IOTHUB_CLIENT_STATUS>("IoTHubClientStatus")
        .value("IDLE", IOTHUB_CLIENT_SEND_STATUS_IDLE)
        .value("BUSY", IOTHUB_CLIENT_SEND_STATUS_BUSY)
        ;

    enum_<IOTHUB_CLIENT_CONFIRMATION_RESULT>("IoTHubClientConfirmationResult")
        .value("OK", IOTHUB_CLIENT_CONFIRMATION_OK)
        .value("BECAUSE_DESTROY", IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY)
        .value("MESSAGE_TIMEOUT", IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT)
        .value("ERROR", IOTHUB_CLIENT_CONFIRMATION_ERROR)
        ;

    enum_<IOTHUBMESSAGE_DISPOSITION_RESULT>("IoTHubMessageDispositionResult")
        .value("ACCEPTED", IOTHUBMESSAGE_ACCEPTED)
        .value("REJECTED", IOTHUBMESSAGE_REJECTED)
        .value("ABANDONED", IOTHUBMESSAGE_ABANDONED)
        ;

    enum_<IOTHUBMESSAGE_CONTENT_TYPE>("IoTHubMessageContent")
        .value("BYTEARRAY", IOTHUBMESSAGE_BYTEARRAY)
        .value("STRING", IOTHUBMESSAGE_STRING)
        .value("UNKNOWN", IOTHUBMESSAGE_UNKNOWN)
        ;

    enum_<IOTHUB_TRANSPORT_PROVIDER>("IoTHubTransportProvider")
        .value("HTTP", HTTP)
        .value("AMQP", AMQP)
        .value("MQTT", MQTT)
        ;

    // classes
    class_<IoTHubMap>("IoTHubMap")
        .def(init<>())
        .def(init<object &>())
        .def("add", &IoTHubMap::Add)
        .def("add_or_update", &IoTHubMap::AddOrUpdate)
        .def("delete", &IoTHubMap::Delete)
        .def("contains_key", &IoTHubMap::ContainsKey)
        .def("contains_value", &IoTHubMap::ContainsValue)
        .def("get_value_from_key", &IoTHubMap::GetValueFromKey)
        .def("get_internals", &IoTHubMap::GetInternals)
        // Python helpers
#ifdef SUPPORT___STR__
        .def("__str__", &IoTHubMap::str)
        .def("__repr__", &IoTHubMap::repr)
#endif
        ;

    class_<IoTHubMessage>("IoTHubMessage", no_init)
        .def(init<PyObject *>())
        .def(init<std::string>())
        .def("get_bytearray", &IoTHubMessage::GetBytearray)
        .def("get_string", &IoTHubMessage::GetString)
        .def("get_content_type", &IoTHubMessage::GetContentType)
        .def("properties", &IoTHubMessage::Properties, return_value_policy<manage_new_object>())
        .add_property("message_id", &IoTHubMessage::GetMessageId, &IoTHubMessage::SetMessageId)
        .add_property("correlation_id", &IoTHubMessage::GetCorrelationId, &IoTHubMessage::SetCorrelationId)
        // Python helpers
#ifdef SUPPORT___STR__
        .def("__str__", &IoTHubMessage::str)
        .def("__repr__", &IoTHubMessage::repr)
#endif
        ;

    class_<IoTHubClient, boost::noncopyable>("IoTHubClient", no_init)
        .def(init<std::string, IOTHUB_TRANSPORT_PROVIDER>())
        .def("send_event_async", &IoTHubClient::SendEventAsync)
        .def("set_message_callback", &IoTHubClient::SetMessageCallback)
        .def("set_option", &IoTHubClient::SetOption)
        .def("get_send_status", &IoTHubClient::GetSendStatus)
        .def("get_last_message_receive_time", &IoTHubClient::GetLastMessageReceiveTime)
        // attributes
        .def_readonly("protocol", &IoTHubClient::protocol)
        // Python helpers
#ifdef SUPPORT___STR__
        .def("__str__", &IoTHubClient::str)
        .def("__repr__", &IoTHubClient::repr)
#endif
        ;
};

