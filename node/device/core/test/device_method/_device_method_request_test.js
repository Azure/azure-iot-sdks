// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

require('es5-shim');
var assert = require('chai').assert;
var DeviceMethodRequest = require('../../lib/device_method').DeviceMethodRequest;

describe('DeviceMethodRequest', function() {
  describe('#constructor', function() {
  var inputs = [
    { val: undefined, err: ReferenceError },
    { val: null, err: ReferenceError },
    { val: '', err: Error }
  ];
  
  // Tests_SRS_NODE_DEVICE_METHOD_REQUEST_13_001: [ DeviceMethodRequest shall throw a ReferenceError if requestId is falsy or is not a string. ]
  // Tests_SRS_NODE_DEVICE_METHOD_REQUEST_13_002: [ DeviceMethodRequest shall throw an Error if requestId is an empty string. ]
  inputs.forEach(function(inp) {
    it('throws a ' + inp.err.name + ' if \'requestId\' is \'' + inp.val + '\'', function() {
      assert.throws(function() {
        return new DeviceMethodRequest(inp.val, 'Reboot');
      }, inp.err);
    });
  });

  // Tests_SRS_NODE_DEVICE_METHOD_REQUEST_13_003: [ DeviceMethodRequest shall throw a ReferenceError if methodName is falsy or is not a string. ]
  // Tests_SRS_NODE_DEVICE_METHOD_REQUEST_13_004: [ DeviceMethodRequest shall throw an Error if methodName is an empty string. ]
  inputs.forEach(function(inp) {
      it('throws a ' + inp.err.name + ' if \'methodName\' is \'' + inp.val + '\'', function() {
      assert.throws(function() {
          return new DeviceMethodRequest('1', inp.val);
        }, inp.err);
      });
    });
  });
});
