// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

require('es5-shim');
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var sinon = require('sinon');
var assert = require('chai').assert;
var DeviceMethodResponse = require('../../lib/device_method').DeviceMethodResponse;

describe('DeviceMethodResponse', function() {
  var MockReceiver = function() {
    EventEmitter.call(this);

    this.onDeviceMethod = function(methodName, callback) {
      this.on('method_' + methodName, callback);
    };

    // causes a mock method event to be raised
    this.emitMethodCall = function(methodName) {
      this.emit('method_' + methodName, {
        methods: { methodName: methodName },
        properties: {},
        body: '',
        requestId: 42
      });
    };
  };
  util.inherits(MockReceiver, EventEmitter);

  var MockTransport = function(config) {
    EventEmitter.call(this);
    this.config = config;
    this.receiver = new MockReceiver();

    this.getReceiver = function(callback) {
      callback(null, this.receiver);
    };

    this.sendMethodResponse = function(response, done) {
      response = response; // suppress js lint errors about unused params
      if(!!done && typeof(done) === 'function') {
        done(null);
      }
    };
  };
  util.inherits(MockTransport, EventEmitter);

  describe('#constructor', function() {
    var inputs = [
      { val: undefined, err: ReferenceError },
      { val: null, err: ReferenceError },
      { val: '', err: Error }
    ];

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_001: [ DeviceMethodRequest shall throw a ReferenceError if requestId is falsy or is not a string. ]
    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_002: [ DeviceMethodRequest shall throw an Error if requestId is an empty string. ]
    inputs.forEach(function(inp) {
      it('throws a ' + inp.err.name + ' if \'requestId\' is \'' + inp.val + '\'', function() {
        assert.throws(function() {
          return new DeviceMethodResponse(inp.val, {});
        }, inp.err);
      });
    });

    // Codes_SRS_NODE_DEVICE_METHOD_RESPONSE_13_006: [ DeviceMethodResponse shall throw a ReferenceError if transport is falsy. ]
    [null, undefined].forEach(function(inp) {
      it('throws a ReferenceError if \'transport\' is \'' + inp + '\'', function() {
        assert.throws(function() {
          return new DeviceMethodResponse('1', inp);
        }, ReferenceError);
      });
    });
  });

  describe('#write', function() {
    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_003: [ DeviceMethodResponse.write shall throw a ReferenceError if data is falsy. ]
    [undefined, null, '', 0].forEach(function (data) {
      it('throws if data passed to write is "' + data + '"', function() {
        // setup
        var transport = new MockTransport();
        var res = new DeviceMethodResponse('1', transport);

        // test & assert
        assert.throws(function() {
          res.write(data);
        }, ReferenceError);
      });
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_004: [ The DeviceMethodResponse.write method shall accumulate the data passed to it as internal state. ]
    it('response accumulates data into internal state', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);

      // test & assert
      var b1 = new Buffer([1, 2, 3, 4, 5]);
      res.write(b1);
      assert.strictEqual(res.bodyParts.length, 1);

      var b2 = new Buffer([6, 7, 8, 9, 10]);
      res.write(b2);
      assert.strictEqual(res.bodyParts.length, 2);
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_005: [ DeviceMethodResponse.write shall throw an Error object if it is invoked after DeviceMethodResponse.end has been called ].
    it('throws if response.write is called after response.end', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);

      // test & assert
      res.end(200);
      assert.throws(function() {
        res.write(new Buffer([1, 2]));
      }, Error);
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_007: [ DeviceMethodResponse.end shall throw a ReferenceError if status is undefined or not a number. ]
    [undefined, null, 'not_a_number'].forEach(function (status) {
      it('throws if status passed to response.end is "' + status + '"', function() {
        // setup
        var transport = new MockTransport();
        var res = new DeviceMethodResponse('1', transport);

        // test & assert
        assert.throws(function() {
          res.end(status);
        }, ReferenceError);
      });
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_008: [ DeviceMethodResponse.end shall notify the service and supply the response for the request along with the status by calling sendMethodResponse on the underlying transport object. ]
    it('shall send response to transport\'s sendMethodResponse function', function() {
      // setup
      var transport = new MockTransport();
      var spy = sinon.spy(transport, 'sendMethodResponse');
      var res = new DeviceMethodResponse('1', transport);
      var props = {
        "k1": "v1",
        "k2": "v2"
      };

      // test
      res.properties = props;
      res.write(new Buffer([1, 2, 3]));
      res.end(200);

      // assert
      assert.isTrue(spy.calledOnce);
      assert.strictEqual(res.bodyParts.length, 1);
      assert.strictEqual(res.status, 200);
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_009: [ DeviceMethodResponse.end shall throw an Error object if it is called more than once for the same request. ]
    it('throws if response.end is called more than once', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);

      // test & assert
      res.end(200);
      assert.throws(function() {
        res.end(202);
      }, Error);
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_010: [ DeviceMethodResponse.end shall invoke the callback specified by done if it is not falsy. ]
    it('calls the callback passed to response.end', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);
      var callback = sinon.spy();

      // test
      res.end(200, callback);

      // assert
      assert.isTrue(callback.calledOnce);
      assert.isNull(callback.args[0][0]);
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_011: [ DeviceMethodResponse.end shall pass the status of sending the response to the service to done. ]
    it('calls the callback passed to response.end with error if transport fails', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);
      var callback = sinon.spy();
      var sendMethodResponseStub = sinon.stub(transport, 'sendMethodResponse');
      sendMethodResponseStub.callsArgWith(1, new Error('boo'));

      // test
      res.end(200, callback);

      // assert
      assert.isTrue(callback.calledOnce);
      assert.isNotNull(callback.args[0][0]);
    });
  });
});
