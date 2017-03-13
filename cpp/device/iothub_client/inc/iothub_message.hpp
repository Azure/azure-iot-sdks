#ifndef IOTHUB_MESSAGE_HPP
#define IOTHUB_MESSAGE_HPP

#include <string>
#include <memory>

#include "iothub_message.h"
#include "iothub_exception.hpp"
#include "iothub_map.hpp"

namespace azure { namespace iot {

    class message_exception : public azure::iot::exception {
    public:
        message_exception(std::string _func, IOTHUB_MESSAGE_RESULT message);

        IOTHUB_MESSAGE_RESULT result;

        std::string decode_error() const;
    };

    class message
    {
        std::shared_ptr<IOTHUB_MESSAGE_HANDLE_DATA_TAG> mMessageHandle;
        std::shared_ptr<azure::iot::map> iotHubMapProperties;
    public:
        message(const azure::iot::message& message);
        message(azure::iot::message&& mesage) noexcept;
        message(IOTHUB_MESSAGE_HANDLE _iotHubMessageHandle);
        message(std::string source);

        const std::string getString() const;
        const unsigned char* getByteArray(size_t *size) const;
        IOTHUBMESSAGE_CONTENT_TYPE getContentType() const;
        azure::iot::map properties();
        const std::string getMessageId() const;
        void setMessageId(std::string messageId);
        const std::string getCorrelationId() const;
        void setCorrelationId(std::string correlationId);
        IOTHUB_MESSAGE_HANDLE handle() const;
    };

}}

#endif
