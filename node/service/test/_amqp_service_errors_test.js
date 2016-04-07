// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var errors = require('azure-iot-common').errors;
var translateError = require('../lib/amqp_service_errors.js');

describe('translateError', function() {
  /*Tests_SRS_NODE_DEVICE_AMQP_SERVICE_ERRORS_16_001: [ `translateError` shall return an `DeviceMaximumQueueDepthExceededError` if the AMQP error condition is `amqp:resource-limit-exceeded`.] */
  [
    { errorDescription: 'amqp:resource-limit-exceeded', errorMessage: 'Fake forbidden', expectedErrorType: errors.DeviceMaximumQueueDepthExceededError },
  ].forEach(function(testParams) {
    it('returns an \'' + testParams.expectedErrorType.name + '\' if the amqp error description is \'' + testParams.errorDescription + '\'', function(){
      var AMQPError = function AMQPError() {
        Error.call(this);
      };

      var fake_error = new AMQPError();
      fake_error.condition = { contents: testParams.errorDescription };

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
  });
});