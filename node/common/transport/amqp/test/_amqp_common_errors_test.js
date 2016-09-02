// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var errors = require('azure-iot-common').errors;
var translateError = require('../lib/amqp_common_errors.js');

describe('translateError', function() {
  /*Tests_SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_002: [If the AMQP error code is unknown, `translateError` should return a generic Javascript `Error` object.]*/
  /*Tests_SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_003: [`translateError` shall return an `ArgumentError` if the AMQP error condition is `com.microsoft:argument-out-of-range`.]*/
  /*Tests_SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_004: [`translateError` shall return an `UnauthorizedError` if the AMQP error condition is `amqp:unauthorized-access`.]*/
  /*Tests_SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_006: [`translateError` shall return an `DeviceNotFoundError` if the AMQP error condition is `amqp:not-found`.]*/
  /*Tests_SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_007: [`translateError` shall return an `MessageTooLargeError` if the AMQP error condition is `amqp:link-message-size-exceeded`.]*/
  /*Tests_SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_008: [`translateError` shall return an `InternalServerError` if the AMQP error condition is `amqp:internal-error`.]*/
  /*Tests_SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_011: [`translateError` shall return an `IotHubQuotaExceededError` if the AMQP error condition is `amqp:resource-limit-exceeded`.]*/
  /*Tests_SRS_NODE_DEVICE_AMQP_COMMON_ERRORS_16_009: [`translateError` shall return an `ServiceUnavailableError` if the AMQP error condition is `com.microsoft:timeout`.]*/
  [
    { errorDescription: 'com.microsoft:argument-out-of-range', errorMessage: 'Fake bad request', expectedErrorType: errors.ArgumentError },
    { errorDescription: 'amqp:unauthorized-access', errorMessage: 'Fake unauthorized', expectedErrorType: errors.UnauthorizedError },
    { errorDescription: 'amqp:not-found', errorMessage: 'Fake not found', expectedErrorType: errors.DeviceNotFoundError },
    { errorDescription: 'amqp:link-message-size-exceeded', errorMessage: 'Fake request too large', expectedErrorType: errors.MessageTooLargeError },
    { errorDescription: 'amqp:internal-error', errorMessage: 'Fake internal server error', expectedErrorType: errors.InternalServerError },
    { errorDescription: 'amqp:resource-limit-exceeded', errorMessage: 'Fake quota exceeded error', expectedErrorType: errors.IotHubQuotaExceededError },
    { errorDescription: 'com.microsoft:timeout', errorMessage: 'Fake server unavailable', expectedErrorType: errors.ServiceUnavailableError },
    { errorDescription: 'unknown', errorMessage: 'Unknown error', expectedErrorType: Error }
  ].forEach(function(testParams) {
    it('returns an \'' + testParams.expectedErrorType.name + '\' if the amqp error description is \'' + testParams.errorDescription + '\'', function(){
      var AMQPError = function AMQPError() {
        Error.call(this);
      };

      var fake_error = new AMQPError();
      fake_error.condition = testParams.errorDescription;

      /*Tests_SRS_NODE_DEVICE_AMQP_ERRORS_16_010: [ `translateError` shall accept 2 argument:
      *- A custom error message to give context to the user.
      *- the AMQP error object itself]
      */
      var err = translateError(new Error(testParams.errorMessage), fake_error);
      assert.instanceOf(err, testParams.expectedErrorType);

      /*Tests_SRS_NODE_DEVICE_AMQP_ERRORS_16_001: [Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 2 properties:
      *- `amqpError` shall contain the error object returned by the AMQP layer.
      *- `message` shall contain a human-readable error message]
      */
      assert.equal(err.message, 'Error: ' + testParams.errorMessage);
      assert.equal(err.amqpError, fake_error);
    });

    it('returns a generic error object if the error type is unknown', function(){
      var error = new Error('unknown reason');
      var message = 'unknown error type';
      var err = translateError(message, error);
      
      /*Tests_SRS_NODE_DEVICE_AMQP_ERRORS_16_001: [Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 2 properties:
      *- `amqpError` shall contain the error object returned by the AMQP layer.
      *- `message` shall contain a human-readable error message]
      */
      assert.equal(err.message, message);
      assert.equal(err.amqpError, error);
    });
  });
});