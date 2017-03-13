#ifndef IOTHUB_CLIENT_HPP
#define IOTHUB_CLIENT_HPP

#include <functional>
#include <string>
#include <sstream>

#include "iothub_client.h"
#include "iothub_client_ll.h"
#include "iothub_message.hpp"
#include "iothub_exception.hpp"

namespace azure { namespace iot {

    class client_exception : public azure::iot::exception {
    public:
        client_exception(const std::string& _func, const IOTHUB_CLIENT_RESULT _result);

        std::string decode_error(IOTHUB_CLIENT_RESULT result) const;
    };

    enum transport {
        HTTP,
        AMQP,
        MQTT
    };

    enum disposition_result {
        ACCEPTED,
        REJECTED,
        ABANDONED
    };

    enum confirmation_result {
        OK,
        BECAUSE_DESTROY,
        MESSAGE_TIMEOUT,
        ERROR
    };

    typedef struct client_config {
        azure::iot::transport protocol;
        std::string deviceId;
        std::string deviceKey;
        std::string iotHubName;
        std::string iotHubSuffix;
        std::string protocolGatewayHostName;
    } client_config;

    template <class R, class... Args>
    struct CallbackContext {
        std::function<R(Args...)> messageCallback;
        void* userContext;
    };

    class client
    {
        std::shared_ptr<IOTHUB_CLIENT_LL_HANDLE_DATA_TAG> iotHubClientHandle;
        const IOTHUB_CLIENT_LL_HANDLE getHandle() const;
        std::unique_ptr<CallbackContext<azure::iot::disposition_result, azure::iot::message, void*>> receiveMessageContext;
        azure::iot::transport protocol;
        void setOptionInternal(std::string optionName, const void *value);
    public:

        client(IOTHUB_CLIENT_LL_HANDLE _iotHubClientHandle, azure::iot::transport _protocol);
        client(std::string connectionString, azure::iot::transport _protocol);
        client(const azure::iot::client_config config);

        void doWork();

        void sendEventAsync(azure::iot::message eventMessage, std::function<void (const azure::iot::confirmation_result, const void*)> messageCallback, void *userContext);

        void setMessageCallback(std::function<azure::iot::disposition_result (azure::iot::message message, void* userContextCallback)>messageCallback, void* userContext);
        template<typename T>
        void setOption(std::string optionName, T value)
        {
            setOptionInternal(optionName, &value);
        }
        IOTHUB_CLIENT_STATUS getSendStatus() const;
        time_t getLastMessageReceiveTime() const;
    };

}}

#endif
