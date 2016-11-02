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

  describe('#send', function() {
    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_007: [ DeviceMethodResponse.send shall throw a ReferenceError if status is undefined or not a number. ]
    [undefined, null, 'not_a_number'].forEach(function (status) {
      it('throws if status passed to response.send is "' + status + '"', function() {
        // setup
        var transport = new MockTransport();
        var res = new DeviceMethodResponse('1', transport);

        // test & assert
        assert.throws(function() {
          res.send(status);
        }, ReferenceError);
      });
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_008: [ DeviceMethodResponse.send shall notify the service and supply the response for the request along with the status by calling sendMethodResponse on the underlying transport object. ]
    it('shall send response to transport\'s sendMethodResponse function', function() {
      // setup
      var transport = new MockTransport();
      var spy = sinon.spy(transport, 'sendMethodResponse');
      var res = new DeviceMethodResponse('1', transport);

      // test
      res.send(200);

      // assert
      assert.isTrue(spy.calledOnce);
      assert.strictEqual(res.status, 200);
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_009: [ DeviceMethodResponse.send shall throw an Error object if it is called more than once for the same request. ]
    it('throws if response.send is called more than once', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);

      // test & assert
      res.send(200);
      assert.throws(function() {
        res.send(202);
      }, Error);
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_010: [ DeviceMethodResponse.send shall invoke the callback specified by done if it is not falsy. ]
    it('calls the callback passed to response.send when no payload is passed.', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);
      var callback = sinon.spy();

      // test
      res.send(200, callback);

      // assert
      assert.isTrue(callback.calledOnce);
      assert.isNull(callback.args[0][0]);
    });

    it('calls the callback passed to response.send when a payload is passed.', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);
      var callback = sinon.spy();

      // test
      res.send(200, 'payload', callback);

      // assert
      assert.isTrue(callback.calledOnce);
      assert.isNull(callback.args[0][0]);
    });

    it('throws if the callback is not the last argument', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);

      // test and assert
      assert.throws(function() {
        res.send(200, function() {}, 'payload');
      });
    });

    // Tests_SRS_NODE_DEVICE_METHOD_RESPONSE_13_011: [ DeviceMethodResponse.send shall pass the status of sending the response to the service to done. ]
    it('calls the callback passed to response.send with error if transport fails', function() {
      // setup
      var transport = new MockTransport();
      var res = new DeviceMethodResponse('1', transport);
      var callback = sinon.spy();
      var sendMethodResponseStub = sinon.stub(transport, 'sendMethodResponse');
      sendMethodResponseStub.callsArgWith(1, new Error('boo'));

      // test
      res.send(200, callback);

      // assert
      assert.isTrue(callback.calledOnce);
      assert.isNotNull(callback.args[0][0]);
    });
  });
});
