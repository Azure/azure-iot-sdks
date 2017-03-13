#include <sstream>
#include <memory>
#include <iostream>

#include "iothub_message.h"

#include "iothub_map.hpp"
#include "iothub_message.hpp"

namespace azure { namespace iot {

    message_exception::message_exception(std::string _func, IOTHUB_MESSAGE_RESULT message) :
        exception("IoTHubMessageError", "IoTHubMessage",_func, decode_error()), result(message)
    {
    }

    std::string message_exception::decode_error() const
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

    message::message(const message& other)
    {
        auto handle = IoTHubMessage_Clone(other.mMessageHandle.get());
        if (handle == NULL)
        {
            throw message_exception(__func__, IOTHUB_MESSAGE_ERROR);
        }
        mMessageHandle = std::shared_ptr<IOTHUB_MESSAGE_HANDLE_DATA_TAG>(handle, IoTHubMessage_Destroy);
    }
        
    message::message(message&& other) noexcept :
        mMessageHandle(std::move(other.mMessageHandle)),
        iotHubMapProperties(std::move(other.iotHubMapProperties))
    {
        other.mMessageHandle = nullptr;
        other.iotHubMapProperties = nullptr;
    }

    message::message(IOTHUB_MESSAGE_HANDLE _iotHubMessageHandle)
    {
        if (_iotHubMessageHandle == NULL)
        {
            throw message_exception(__func__, IOTHUB_MESSAGE_ERROR);
        }
        
        auto handle = IoTHubMessage_Clone(_iotHubMessageHandle);
        mMessageHandle = std::shared_ptr<IOTHUB_MESSAGE_HANDLE_DATA_TAG>(handle, IoTHubMessage_Destroy);
    }

    message::message(std::string source)
    {
        auto handle = IoTHubMessage_CreateFromString(source.c_str());
        if (handle == NULL)
        {
            throw message_exception(__func__, IOTHUB_MESSAGE_ERROR);
        }
        mMessageHandle = std::shared_ptr<IOTHUB_MESSAGE_HANDLE_DATA_TAG>(handle, IoTHubMessage_Destroy);
    }

    const std::string message::getString() const
    {
        auto messageString = IoTHubMessage_GetString(handle());
        if (messageString != NULL)
        {
            return std::string(messageString);
        }

        return nullptr;
    }

    const unsigned char* message::getByteArray(size_t *size) const
    {
        const unsigned char* buffer;
        auto result = IoTHubMessage_GetByteArray(handle(), &buffer, size);
        if (result == IOTHUB_MESSAGE_OK)
        {
            return buffer;
        }

        return nullptr;
    }

    IOTHUBMESSAGE_CONTENT_TYPE message::getContentType() const
    {
        return IoTHubMessage_GetContentType(handle());
    }

    azure::iot::map message::properties()
    {
        if (iotHubMapProperties == nullptr)
        {
            iotHubMapProperties = std::make_shared<map>(map(IoTHubMessage_Properties(handle())));
        }
        return *iotHubMapProperties.get();
    }

    const std::string message::getMessageId() const
    {
        auto messageId = IoTHubMessage_GetMessageId(handle());
        if (messageId != nullptr)
        {
            return std::string(messageId);
        }
        return nullptr;
    }

    void message::setMessageId(std::string messageId)
    {
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetMessageId(handle(), messageId.c_str());
        if (result != IOTHUB_MESSAGE_OK)
        {
            throw message_exception(__func__, result);
        }
    }

    const std::string message::getCorrelationId() const
    {
        return std::string(IoTHubMessage_GetCorrelationId(handle()));
    }

    void message::setCorrelationId(std::string correlationId)
    {
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetCorrelationId(handle(), correlationId.c_str());
        if (result != IOTHUB_MESSAGE_OK)
        {
            throw message_exception(__func__, result);
        }
    }

    IOTHUB_MESSAGE_HANDLE message::handle() const
    {
        return mMessageHandle.get();
    }

}}
