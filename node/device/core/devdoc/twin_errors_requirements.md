# azure-iot-device-http.translateError Requirements

## Overview
`translateError` is a method that translates Twin errors into Azure IoT Hub errors, effectively abstracting the error that is returned to the SDK user of from the transport layer.

## Requirements

**SRS_NODE_DEVICE_TWIN_ERRORS_18_002: [** If the error code is unknown, `translateError` should return a generic Javascript `Error` object. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_003: [** `translateError` shall return an `ArgumentError` if the response status code is `400`. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_004: [** `translateError` shall return an `UnauthorizedError` if the response status code is `401`. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_005: [** `translateError` shall return an `IotHubQuotaExceededError` if the response status code is `403`. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_006: [** `translateError` shall return an `DeviceNotFoundError` if the response status code is `404`. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_007: [** `translateError` shall return an `MessageTooLargeError` if the response status code is `413`. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_008: [** `translateError` shall return an `InternalServerError` if the response status code is `500`. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_009: [** `translateError` shall return an `ServiceUnavailableError` if the response status code is `503`. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_011: [** `translateError` shall return an `ServiceUnavailableError` if the response status code is `504`. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_012: [** `translateError` shall return an `ThrottlingError` if the response status code is `429`. **]**

**SRS_NODE_DEVICE_TWIN_ERRORS_18_013: [** `translateError` shall return an `InvalidEtagError` if the response status code is `412`. **]**
