// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var errors = require('azure-iot-common').errors;
var translateError = require('../lib/registry_http_errors.js');

describe('translateError', function() {

  [
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_003: [`translateError` shall return an `ArgumentError` if the HTTP response status code is `400`.]*/
    { statusCode: 400, statusMessage: 'Bad request', expectedErrorType: errors.ArgumentError },
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_004: [`translateError` shall return an `UnauthorizedError` if the HTTP response status code is `401`.]*/
    { statusCode: 401, statusMessage: 'Unauthorized', expectedErrorType: errors.UnauthorizedError },
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_005: [`translateError` shall return an `TooManyDevicesError` if the HTTP response status code is `403`.]*/
    { statusCode: 403, statusMessage: 'Forbidden', expectedErrorType: errors.TooManyDevicesError },
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_006: [`translateError` shall return an `IotHubNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `IotHubNotFound`.*/
    { statusCode: 404, hubErrorCode: 'IotHubNotFound', statusMessage: 'Not found', expectedErrorType: errors.IotHubNotFoundError },
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_012: [`translateError` shall return an `DeviceNotFoundError` if the HTTP response status code is `404` and if the error code within the body of the error response is `DeviceNotFound`.*/
    { statusCode: 404, hubErrorCode: 'DeviceNotFound', statusMessage: 'Not found', expectedErrorType: errors.DeviceNotFoundError },
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_007: [`translateError` shall return an `DeviceAlreadyExistsError` if the HTTP response status code is `409`.]*/
    { statusCode: 409, hubErrorCode: 'DeviceAlreadyExists', statusMessage: 'Conflict', expectedErrorType: errors.DeviceAlreadyExistsError },
    /*Codes_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_013: [`translateError` shall return an `InvalidEtagError` if the HTTP response status code is `412`.]*/
    { statusCode: 412, hubErrorCode: 'InvalidEtag', statusMessage: 'Precondition failed', expectedErrorType: errors.InvalidEtagError },
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_011: [`translateError` shall return an `ThrottlingError` if the HTTP response status code is `429`.]*/
    { statusCode: 429, statusMessage: 'Too many requests', expectedErrorType: errors.ThrottlingError },
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_008: [`translateError` shall return an `InternalServerError` if the HTTP response status code is `500`.]*/
    { statusCode: 500, statusMessage: 'Internal Server Error', expectedErrorType: errors.InternalServerError },
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_009: [`translateError` shall return an `ServiceUnavailableError` if the HTTP response status code is `503`.]*/
    { statusCode: 503, statusMessage: 'Server Unavailable', expectedErrorType: errors.ServiceUnavailableError },
    /*Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_002: [If the HTTP error code is unknown, `translateError` should return a generic Javascript `Error` object.]*/
    { statusCode: 1337, statusMessage: 'unknown', expectedErrorType: Error}
  ].forEach(function(testParams) {
    it('returns a \'' + testParams.expectedErrorType.name + '\' if the response status code is \'' + testParams.statusCode + '\'', function(){
      var fakeReponse = {
        statusCode: testParams.statusCode,
        statusMessage: testParams.statusMessage,
      };

      var fakeResponseBody = '';
      if (testParams.hubErrorCode) {
        fakeResponseBody = "{\"Message\":\"ErrorCode:" + testParams.hubErrorCode + ";\\\"fake test error message\\\"\"}";
      } else {
        fakeResponseBody = testParams.statusCode + ': ' + testParams.statusMessage;
      }

      /* Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_010: [`translateError` shall accept 3 arguments:
       * - A custom error message to give context to the user.
       * - the body of  the HTTP response, containing the explanation of why the request failed
       * - the HTTP response object itself]
       */
      var err = translateError(fakeResponseBody, fakeReponse);
      assert.instanceOf(err, testParams.expectedErrorType);

      /* Tests_SRS_NODE_IOTHUB_REGISTRY_HTTP_ERRORS_16_001: [** Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 3 properties:
       *- `response` shall contain the `IncomingMessage` object returned by the HTTP layer.
       *- `reponseBody` shall contain the content of the HTTP response.
       *- `message` shall contain a human-readable error message]
       */
      assert.isOk(err.message);
      assert.equal(err.responseBody, fakeResponseBody);
      assert.equal(err.response, fakeReponse);
    });
  });
});