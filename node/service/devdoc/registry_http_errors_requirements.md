# azure-iothub.translateError Requirements

## Overview
`translateError` is a method that translates HTTP errors into Azure IoT Hub errors, effectively abstracting the error that is returned to the SDK user of from the transport layer.

## Requirements

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_001: [** Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 3 properties:
- `response` shall contain the `IncomingMessage` object returned by the HTTP layer.
- `reponseBody` shall contain the content of the HTTP response.
- `message` shall contain a human-readable error message **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_002: [** If the HTTP error code is unknown, `translateError` should return a generic Javascript `Error` object. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_003: [** `translateError` shall return an `ArgumentError` if the HTTP response status code is `400`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_004: [** `translateError` shall return an `UnauthorizedError` if the HTTP response status code is `401`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_005: [** `translateError` shall return an `TooManyDevicesError` if the HTTP response status code is `403`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_006: [** `translateError` shall return an `IotHubNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `IotHubNotFound`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_012: [** `translateError` shall return an `DeviceNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `DeviceNotFound`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_007: [** `translateError` shall return an `DeviceAlreadyExistsError` if the HTTP response status code is `409`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_013: [** `translateError` shall return an `InvalidEtagError` if the HTTP response status code is `412`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_011: [** `translateError` shall return an `ThrottlingError` if the HTTP response status code is `429`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_008: [** `translateError` shall return an `InternalServerError` if the HTTP response status code is `500`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_009: [** `translateError` shall return an `ServiceUnavailableError` if the HTTP response status code is `503`. **]**

**SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_010: [** `translateError` shall accept 2 arguments:
- the body of  the HTTP response, containing the explanation of why the request failed
- the HTTP response object itself **]**