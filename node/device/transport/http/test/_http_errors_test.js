// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var errors = require('azure-iot-common').errors;
var translateError = require('../lib/http_errors.js');

describe('translateError', function() {

/*Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_002: [If the HTTP error code is unknown, `translateError` should return a generic Javascript `Error` object.]*/
/*Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_003: [`translateError` shall return an `ArgumentError` if the HTTP response status code is `400`.]*/
/*Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_004: [`translateError` shall return an `UnauthorizedError` if the HTTP response status code is `401`.]*/
/*Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_005: [`translateError` shall return an `IotHubQuotaExceededError` if the HTTP response status code is `403`.]*/
/*Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_006: [`translateError` shall return an `DeviceNotFoundError` if the HTTP response status code is `404`.]*/
/*Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_007: [`translateError` shall return an `MessageTooLargeError` if the HTTP response status code is `413`.]*/
/*Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_008: [`translateError` shall return an `InternalServerError` if the HTTP response status code is `500`.]*/
/*Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_009: [`translateError` shall return an `ServiceUnavailableError` if the HTTP response status code is `503`.]*/
  [
    { statusCode: 400, statusMessage: 'Bad request', errorMessage: 'Fake bad request', expectedErrorType: errors.ArgumentError },
    { statusCode: 401, statusMessage: 'Unauthorized', errorMessage: 'Fake unauthorized', expectedErrorType: errors.UnauthorizedError },
    { statusCode: 403, statusMessage: 'Forbidden', errorMessage: 'Fake forbidden', expectedErrorType: errors.IotHubQuotaExceededError },
    { statusCode: 404, statusMessage: 'Not found', errorMessage: 'Fake not found', expectedErrorType: errors.DeviceNotFoundError },
    { statusCode: 413, statusMessage: 'Request too large', errorMessage: 'Fake request too large', expectedErrorType: errors.MessageTooLargeError },
    { statusCode: 500, statusMessage: 'Internal Server Error', errorMessage: 'Fake internal server error', expectedErrorType: errors.InternalServerError },
    { statusCode: 503, statusMessage: 'Server Unavailable', errorMessage: 'Fake server unavailable', expectedErrorType: errors.ServiceUnavailableError }
  ].forEach(function(testParams) {
    it('returns an \'' + testParams.expectedErrorType.name + '\' if the response status code is \'' + testParams.statusCode + '\'', function(){
      var fake_response = {
      statusCode: testParams.statusCode,
      statusMessage: testParams.statusMessage,
    };
    var fake_response_body = testParams.statusCode + ': ' + testParams.statusMessage;

    /* Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_010: [`translateError` shall accept 3 arguments:
     * - A custom error message to give context to the user.
     * - the body of  the HTTP response, containing the explanation of why the request failed
     * - the HTTP response object itself]
     */
    var err = translateError(new Error(testParams.errorMessage), fake_response_body, fake_response);
    assert.instanceOf(err, testParams.expectedErrorType);

    /* Tests_SRS_NODE_DEVICE_HTTP_ERRORS_16_001: [Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 3 properties:
     * - `response` shall contain the `IncomingMessage` object returned by the HTTP layer.
     * - `reponseBody` shall contain the content of the HTTP response.
     * - `message` shall contain a human-readable error message]
     */
    assert.equal(err.message, 'Error: ' + testParams.errorMessage);
    assert.equal(err.responseBody, fake_response_body);
    assert.equal(err.response, fake_response);
    });
  });
});