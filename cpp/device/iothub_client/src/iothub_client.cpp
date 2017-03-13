#include <string>
#include <vector>
#include <list>
#include <functional>
#include <exception>
#include <iostream>

#include "iothub_client.h"
#include "iothub_client_ll.h"
#include "iothubtransporthttp.h"
#include "iothubtransportamqp.h"
#include "iothubtransportmqtt.h"

#include "iothub_message.hpp"
#include "iothub_client.hpp"

namespace azure { namespace iot {

    using SendEventContext = CallbackContext<void, const azure::iot::confirmation_result, const void*>;
    using MessageContext = CallbackContext<azure::iot::disposition_result, azure::iot::message, void*>;

    client_exception::client_exception(const std::string& _func, const IOTHUB_CLIENT_RESULT _result) :
        azure::iot::exception("IoTHubClientError", "IoTHubClient", _func, client_exception::decode_error(_result))
    {}

    std::string client_exception::decode_error(IOTHUB_CLIENT_RESULT result) const
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

    static IOTHUB_CLIENT_TRANSPORT_PROVIDER GetProtocol(azure::iot::transport _protocol)
    {
        switch (_protocol)
        {
        case azure::iot::transport::HTTP: return HTTP_Protocol;
        case azure::iot::transport::AMQP: return AMQP_Protocol;
        case azure::iot::transport::MQTT: return MQTT_Protocol;
        default:
            throw "IoTHubTransportProvider set to unknown protocol";
            break;
        }
    }

    const IOTHUB_CLIENT_LL_HANDLE client::getHandle() const
    {
        return iotHubClientHandle.get();
    }

    client::client(std::string connectionString, azure::iot::transport _protocol) :
        protocol(_protocol)
    {
        auto handle = IoTHubClient_LL_CreateFromConnectionString(connectionString.c_str(), GetProtocol(_protocol));
        if (handle == NULL)
        {
            throw azure::iot::client_exception(__func__, IOTHUB_CLIENT_ERROR);
        }

        iotHubClientHandle = std::shared_ptr<IOTHUB_CLIENT_LL_HANDLE_DATA_TAG>(handle, IoTHubClient_LL_Destroy);
    }

    client::client(const azure::iot::client_config _config)
    {
        IOTHUB_CLIENT_CONFIG config;
        config.protocol = GetProtocol(_config.protocol);
        config.deviceId = _config.deviceId.c_str();
        config.deviceKey = _config.deviceKey.c_str();
        config.iotHubName = _config.iotHubName.c_str();
        config.iotHubSuffix = _config.iotHubSuffix.c_str();
        config.protocolGatewayHostName = _config.protocolGatewayHostName.c_str();

        auto handle = IoTHubClient_LL_Create(&config);
        if (handle == NULL)
        {
            throw azure::iot::client_exception(__func__, IOTHUB_CLIENT_ERROR);
        }

        // Wrap handle in shared pointer and define custom deleter
        iotHubClientHandle = std::shared_ptr<IOTHUB_CLIENT_LL_HANDLE_DATA_TAG>(handle, IoTHubClient_LL_Destroy);
    }

    static void SendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback)
    {
        auto sendContext = static_cast<SendEventContext *>(userContextCallback);

        azure::iot::confirmation_result confResult;
        switch (result)
        {
        case IOTHUB_CLIENT_CONFIRMATION_OK: confResult = confirmation_result::OK; break;
        case IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY: confResult = confirmation_result::BECAUSE_DESTROY; break;
        case IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT: confResult = confirmation_result::MESSAGE_TIMEOUT; break;
        case IOTHUB_CLIENT_CONFIRMATION_ERROR: confResult = confirmation_result::ERROR; break;
        }
        
        if (sendContext->messageCallback != nullptr)
        {
            sendContext->messageCallback(confResult, sendContext->userContext);
        }

        delete sendContext;
    }

    void client::sendEventAsync(azure::iot::message eventMessage, std::function<void (const azure::iot::confirmation_result, const void*)> messageCallback, void *userContext)
    {
        SendEventContext *sendContext = new SendEventContext;
        sendContext->messageCallback = messageCallback;
        sendContext->userContext = userContext;

        auto result = IoTHubClient_LL_SendEventAsync(getHandle(), eventMessage.handle(), SendCallback, sendContext);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw azure::iot::client_exception(__func__, result);
        }
    }

    static IOTHUBMESSAGE_DISPOSITION_RESULT InternalCallback(IOTHUB_MESSAGE_HANDLE messageHandle, void* userContextCallback)
    {
        auto receiveContext = static_cast<MessageContext*>(userContextCallback);
        auto messageCallback = receiveContext->messageCallback;
        auto userContext = receiveContext->userContext;

        auto iotMessage = message(messageHandle);
        auto result = messageCallback(iotMessage, userContext);

        switch (result)
        {
        case ABANDONED: return IOTHUBMESSAGE_ABANDONED;
        case ACCEPTED: return IOTHUBMESSAGE_ACCEPTED;
        case REJECTED: return IOTHUBMESSAGE_REJECTED;
        default: return IOTHUBMESSAGE_ABANDONED;
        }
    }

    void client::doWork()
    {
        IoTHubClient_LL_DoWork(getHandle());
    }

    void client::setMessageCallback(std::function<azure::iot::disposition_result (azure::iot::message message, void* userContextCallback)>messageCallback, void* userContext)
    {
        MessageContext *receiveContext = new MessageContext;
        receiveContext->messageCallback = messageCallback;
        receiveContext->userContext = userContext;

        auto result = IoTHubClient_LL_SetMessageCallback(getHandle(), azure::iot::InternalCallback, receiveContext);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw azure::iot::client_exception(__func__, result);
        }

        receiveMessageContext = std::unique_ptr<MessageContext>(receiveContext);
    }

    void client::setOptionInternal(std::string optionName, const void *value)
    {
        auto result = IoTHubClient_LL_SetOption(getHandle(), optionName.c_str(), value);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw azure::iot::client_exception(__func__, result);
        }
    }

    IOTHUB_CLIENT_STATUS client::getSendStatus() const
    {
        IOTHUB_CLIENT_STATUS iotHubClientStatus;
        auto result = IoTHubClient_LL_GetSendStatus(getHandle(), &iotHubClientStatus);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw azure::iot::client_exception(__func__, result);
        }
        return iotHubClientStatus;
    }

    time_t client::getLastMessageReceiveTime() const
    {
        time_t lastMessageReceiveTime;
        auto result = IoTHubClient_LL_GetLastMessageReceiveTime(getHandle(), &lastMessageReceiveTime);
        if (result != IOTHUB_CLIENT_OK)
        {
            throw azure::iot::client_exception(__func__, result);
        }
        return lastMessageReceiveTime;
    }

}}
