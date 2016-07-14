// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var errors = require('azure-iot-common').errors;
var Http = require('azure-iot-http-base').Http;

/* Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_010: [`translateError` shall accept 2 arguments:
 * - the body of  the HTTP response, containing the explanation of why the request failed
 * - the HTTP response object itself]
 */
var translateError = function translateError(body, response) {
  var error;
  var errorContent = Http.parseErrorBody(body);
  var message = errorContent ? errorContent.message : "Error: " + body;

  switch(response.statusCode) {
    case 400:
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_003: [`translateError` shall return an `ArgumentError` if the HTTP response status code is `400`.]*/
      error = new errors.ArgumentError(message);
      break;
    case 401:
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_004: [`translateError` shall return an `UnauthorizedError` if the HTTP response status code is `401`.]*/
      error = new errors.UnauthorizedError(message);
      break;
    case 403:
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_005: [`translateError` shall return an `IotHubQuotaExceededError` if the HTTP response status code is `403`.]*/
      error = new errors.TooManyDevicesError(message);
      break;
    case 404:
        /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_012: [`translateError` shall return an `DeviceNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `DeviceNotFound`.*/
      if (errorContent && errorContent.code === 'DeviceNotFound') {
        error = new errors.DeviceNotFoundError(message);
      } else if (errorContent && errorContent.code === 'IotHubNotFound') {
        /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_006: [`translateError` shall return an `IotHubNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `IotHubNotFound`.*/
        error = new errors.IotHubNotFoundError(message);
      } else {
        error = new Error('Not found');
      }
      break;
    case 409:
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_007: [`translateError` shall return an `DeviceAlreadyExistsError` if the HTTP response status code is `409`.]*/
      error = new errors.DeviceAlreadyExistsError(message);
      break;
    case 412:
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_013: [`translateError` shall return an `InvalidEtagError` if the HTTP response status code is `412`.]*/
      error = new errors.InvalidEtagError(message);
      break;
    case 429:
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_011: [`translateError` shall return an `ThrottlingError` if the HTTP response status code is `429`.]*/
      error = new errors.ThrottlingError(message);
      break;
    case 500:
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_008: [`translateError` shall return an `InternalServerError` if the HTTP response status code is `500`.]*/
      error = new errors.InternalServerError(message);
      break;
    case 503:
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_009: [`translateError` shall return an `ServiceUnavailableError` if the HTTP response status code is `503`.]*/
      error = new errors.ServiceUnavailableError(message);
      break;
    default:
      /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_002: [If the HTTP error code is unknown, `translateError` should return a generic Javascript `Error` object.]*/
      error = new Error(message);
  }

/* Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_001: [Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 3 properties:
 * - `response` shall contain the `IncomingMessage` object returned by the HTTP layer.
 * - `reponseBody` shall contain the content of the HTTP response.
 * - `message` shall contain a human-readable error message]
 */
  error.response = response;
  error.responseBody = body;
  return error;
};

module.exports = translateError;