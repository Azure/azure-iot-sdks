// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var errors = require('azure-iot-common').errors;

var translateError = function translateError(response, status) {
  var error;
  switch(status) {
    case 400:
      /*Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_003: [`translateError` shall return an `ArgumentError` if the response status code is `400`.]*/
      error = new errors.ArgumentError();
      break;
    case 401:
      /*Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_004: [`translateError` shall return an `UnauthorizedError` if the response status code is `401`.]*/
      error = new errors.UnauthorizedError();
      break;
    case 403:
      /*Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_005: [`translateError` shall return an `IotHubQuotaExceededError` if the response status code is `403`.]*/
      error = new errors.IotHubQuotaExceededError();
      break;
    case 404:
      /*Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_006: [`translateError` shall return an `DeviceNotFoundError` if the response status code is `404`.]*/
      error = new errors.DeviceNotFoundError();
      break;
    case 413:
      /*Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_007: [`translateError` shall return an `MessageTooLargeError` if the response status code is `413`.]*/
      error = new errors.MessageTooLargeError();
      break;
    case 500:
      /*Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_008: [`translateError` shall return an `InternalServerError` if the response status code is `500`.]*/
      error = new errors.InternalServerError();
      break;
    case 503:
      /*Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_009: [`translateError` shall return an `ServiceUnavailableError` if the response status code is `503`.]*/
      error = new errors.ServiceUnavailableError();
      break;
    case 504:
      /*Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_011: [`translateError` shall return an `ServiceUnavailableError` if the response status code is `504`.]*/
      error = new errors.ServiceUnavailableError();
      break;
    case 429:
      /* Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_012: [`translateError` shall return an `ThrottlingError` if the response status code is `429`.] */
      error = new errors.ThrottlingError();
      break;
    case 412:
      /* Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_013: [`translateError` shall return an `InvalidEtagError` if the response status code is `412`.] */
      error = new errors.InvalidEtagError();
      break;
    default:
      /*Codes_SRS_NODE_DEVICE_TWIN_ERRORS_18_002: [If the error code is unknown, `translateError` should return a generic Javascript `Error` object.]*/
      error = new Error("server returned error " + status);
  }
  
  error.response = response;
  return error;
};

module.exports = translateError;
