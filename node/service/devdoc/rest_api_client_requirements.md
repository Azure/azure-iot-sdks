# RestApiClient requirements

## Overview
The `RestApiClient` class provides common methods for top-level clients that use REST APIs.

## Usage
```js
var restClient = new RestApiClient({ host: 'host', sharedAccessSignature: 'sharedAccessSignature'});
restClient.executeApiCall('GET', '/twins?api-version=2016-11-14', null, null, function(err, result, response) {
  if (err) {
    console.error(restClient.translateError(result, response).toString());
  } else {
    console.log(JSON.stringify(result, null, 2));
  }
});
```

## Public Interface

### RestApiClient(config, httpBase)
The `RestApiClient` constructor initializes a new instance of a `RestApiClient` object that is used to communicate with the REST APIs of the IoT Hub service

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_001: [** The `RestApiClient` constructor shall throw a `ReferenceError` if config is falsy. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_002: [** The `RestApiClient` constructor shall throw an `ArgumentError` if config is missing a `host` or `sharedAccessSignature` property. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_003: [** The `RestApiClient` constructor shall use `azure-iot-common.Http` as the internal HTTP client if the `httpBase` argument is `undefined`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_004: [** The `RestApiClient` constructor shall use the value of the `httpBase` argument as the internal HTTP client if present. **]**

### executeApiCall(method, path, headers, requestBody, timeout, done)
The `executeApiCall` method builds the HTTP request using the passed arguments and calls the `done` callback with the result of the API call.

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_005: [** The `executeApiCall` method shall throw a `ReferenceError` if the `method` argument is falsy. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_006: [** The `executeApiCall` method shall throw a `ReferenceError` if the `path` argument is falsy. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_029: [** If `done` is `undefined` and the `timeout` argument is a function, `timeout` should be used as the callback. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_030: [** If `timeout` is defined and is not a function, the HTTP request timeout shall be adjusted to match the value of the argument. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_007: [** The `executeApiCall` method shall add the following headers to the request:
- Authorization: <this.sharedAccessSignature>
- Request-Id: <guid>
- User-Agent: <version string> **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_008: [** The `executeApiCall` method shall build the HTTP request using the arguments passed by the caller. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_009: [** If the HTTP request is successful the `executeApiCall` method shall parse the JSON response received and call the `done` callback with a `null` first argument, the parsed result as a second argument and the HTTP response object itself as a third argument. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_010: [** If the HTTP request fails with an error code >= 300 the `executeApiCall` method shall translate the HTTP error into a transport-agnostic error using the `translateError` method and call the `done` callback with the resulting error as the only argument. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_011: [** If the HTTP request fails without an HTTP error code the `executeApiCall` shall call the `done` callback with the error itself as the only argument. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_031: [** If there's is a `Content-Type` header and its value is `application/json; charset=utf-8` and the `requestBody` argument is not a `string`, the body of the request shall be stringified using `JSON.stringify()`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_035: [** If there's is a `Content-Type` header and its value is `application/json; charset=utf-8` and the `requestBody` argument is a `string` it shall be used as is as the body of the request. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_032: [** If there's is a `Content-Type` header and its value is `text/plain; charset=utf-8`, the `requestBody` argument shall be used as is. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_033: [** The `executeApiCall` shall throw a `TypeError` if there's is a `Content-Type` header and its value is `text/plain; charset=utf-8` and the `requestBody` argument is not a string. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_036: [** The `executeApiCall` shall set the `Content-Length` header to the length of the serialized value of `requestBody` if it is truthy. **]**

### translateError(result, response)
The `translateError` method translates HTTP errors into Azure IoT Hub errors, effectively abstracting the error that is returned to the SDK user from the transport layer.

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_012: [** Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 3 properties:
- `response` shall contain the `IncomingMessage` object returned by the HTTP layer.
- `reponseBody` shall contain the content of the HTTP response.
- `message` shall contain a human-readable error message. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_013: [** If the HTTP error code is unknown, `translateError` should return a generic Javascript `Error` object. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_014: [** `translateError` shall return an `ArgumentError` if the HTTP response status code is `400`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_015: [** `translateError` shall return an `UnauthorizedError` if the HTTP response status code is `401`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_016: [** `translateError` shall return an `TooManyDevicesError` if the HTTP response status code is `403`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_017: [** `translateError` shall return an `IotHubNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `IotHubNotFound`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_018: [** `translateError` shall return an `DeviceNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `DeviceNotFound`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_019: [** `translateError` shall return a `DeviceTimeoutError` if the HTTP response status code is `408`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_020: [** `translateError` shall return an `DeviceAlreadyExistsError` if the HTTP response status code is `409`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_021: [** `translateError` shall return an `InvalidEtagError` if the HTTP response status code is `412`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_022: [** `translateError` shall return an `ThrottlingError` if the HTTP response status code is `429`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_023: [** `translateError` shall return an `InternalServerError` if the HTTP response status code is `500`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_024: [** `translateError` shall return a `BadDeviceResponseError` if the HTTP response status code is `502`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_025: [** `translateError` shall return an `ServiceUnavailableError` if the HTTP response status code is `503`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_026: [** `translateError` shall return a `GatewayTimeoutError` if the HTTP response status code is `504`. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_027: [** `translateError` shall accept 2 arguments:
- the body of  the HTTP response, containing the explanation of why the request failed.
- the HTTP response object itself. **]**

### updateSharedAccessSignature(sharedAccessSignature)

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_034: [** The `updateSharedAccessSignature` method shall throw a `ReferenceError` if the `sharedAccessSignature` argument is falsy. **]**

**SRS_NODE_IOTHUB_REST_API_CLIENT_16_028: [** The `updateSharedAccessSignature` method shall update the `sharedAccessSignature` configuration parameter that is used in the `Authorization` header of all HTTP requests. **]**
