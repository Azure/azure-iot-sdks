# azure-iot-amqp-base.translateError Requirements

## Overview
`translateError` is a method that translates AMQP errors into Azure IoT Hub errors, effectively abstracting the error that is returned to the SDK user of from the transport layer.

## Requirements

**SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_001: [** Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 2 properties:
- `amqpError` shall contain the error object returned by the AMQP layer.
- `message` shall contain a human-readable error message **]**

**SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_002: [** If the AMQP error code is unknown, `translateError` should return a generic Javascript `Error` object. **]**

**SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_003: [** `translateError` shall return an `ArgumentError` if the AMQP error condition is `com.microsoft:argument-out-of-range`. **]**

**SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_004: [** `translateError` shall return an `UnauthorizedError` if the AMQP error condition is `amqp:unauthorized-access`. **]**

**SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_006: [** `translateError` shall return an `DeviceNotFoundError` if the AMQP error condition is `amqp:not-found`. **]**

**SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_007: [** `translateError` shall return an `MessageTooLargeError` if the AMQP error condition is `amqp:link-message-size-exceeded`. **]**

**SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_008: [** `translateError` shall return an `InternalServerError` if the AMQP error condition is `amqp:internal-error`. **]**

**SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_009: [** `translateError` shall return an `ServiceUnavailableError` if the AMQP error condition is `com.microsoft:timeout`. **]**

**SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_010: [** `translateError` shall accept 2 argument:
- A custom error message to give context to the user.
- the AMQP error object itself **]**