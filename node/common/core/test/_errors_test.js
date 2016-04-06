// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var errors = require('../lib/errors.js');

describe('errors', function() {

  [
    errors.ArgumentError,
    errors.DeviceMaximumQueueDepthExceededError,
    errors.DeviceNotFoundError,
    errors.FormatError,
    errors.InternalServerError,
    errors.IotHubQuotaExceededError,
    errors.MessageTooLargeError,
    errors.NotConnectedError,
    errors.NotImplementedError,
    errors.ServiceUnavailableError,
    errors.UnauthorizedError
  ].forEach(function(ErrorCtor) {
    /*Tests_SRS_NODE_COMMON_ERRORS_16_001: All custom error types shall inherit from the standard Javascript error object.*/
    it(ErrorCtor.name + ' inherits from the standard javascript \'Error\' object', function() {
      var err = new ErrorCtor('test');
      assert.instanceOf(err, Error);
    });

    /*Tests_SRS_NODE_COMMON_ERRORS_16_002: All custom error types shall contain a valid call stack.*/
    it(ErrorCtor.name + ' contains a valid call stack', function () {
      var err = new ErrorCtor('test');
      assert.isOk(err.stack);
    });

    /*Tests_SRS_NODE_COMMON_ERRORS_16_003: All custom error types shall accept a message as an argument of the constructor and shall populate their message property with it.*/
    it(ErrorCtor.name + ' accepts a message as a constructor argument and populates its message property with it', function() {
      var errorMessage = 'test error message';
      var err = new ErrorCtor(errorMessage);
      assert.equal(err.message, errorMessage);
    });
  });
});