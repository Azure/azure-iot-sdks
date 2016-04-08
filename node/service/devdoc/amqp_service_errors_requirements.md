# azure-iothub.translateError Requirements

## Overview
`translateError` is a method that translates AMQP errors that are specific to the service SDK into Azure IoT Hub errors, effectively abstracting the error that is returned to the SDK user of from the transport layer.

## Requirements

**SRS_NODE_DEVICE_AMQP_SERVICE_ERRORS_16_001: [** `translateError` shall return an `DeviceMaximumQueueDepthExceededError` if the AMQP error condition is `amqp:resource-limit-exceeded`. **]**