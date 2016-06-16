// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var sinon = require('sinon');

var Message = require('azure-iot-common').Message;
var Amqp = require('../lib/amqp.js');

describe('Amqp', function () {
  var transport = null;
  var receiver = null;
  var testMessage = new Message();
  var testCallback = function () { };

  beforeEach(function () {
    var DummyReceiver = function () {
      this.complete = sinon.spy();
      this.reject = sinon.spy();
      this.abandon = sinon.spy();
    };

    receiver = new DummyReceiver();
    var DummyAmqp = function () {
      this.getReceiver = function (endpoint, callback) {
        assert.isOk(endpoint);
        callback(null, receiver);
      };
    };

    transport = new Amqp({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
    transport._amqp = new DummyAmqp();
  });

  afterEach(function () {
    transport = null;
    receiver = null;
  });

  describe('#complete', function () {
    /*Tests_SRS_NODE_DEVICE_AMQP_16_013: [The ‘complete’ method shall call the ‘complete’ method of the receiver object and pass it the message and the callback given as parameters.] */
    it('calls the receiver `complete` method', function () {
      transport.complete(testMessage, testCallback);
      assert(receiver.complete.calledWith(testMessage, testCallback));
    });
  });

  describe('#reject', function () {
    /*Tests_SRS_NODE_DEVICE_AMQP_16_014: [The ‘reject’ method shall call the ‘reject’ method of the receiver object and pass it the message and the callback given as parameters.] */
    it('calls the receiver `reject` method', function () {
      transport.reject(testMessage, testCallback);
      assert(receiver.reject.calledWith(testMessage, testCallback));
    });
  });

  describe('#abandon', function () {
    /*Tests_SRS_NODE_DEVICE_AMQP_16_012: [The ‘abandon’ method shall call the ‘abandon’ method of the receiver object and pass it the message and the callback given as parameters.] */
    it('calls the receiver `abandon` method', function () {
      transport.abandon(testMessage, testCallback);
      assert(receiver.abandon.calledWith(testMessage, testCallback));
    });
  });

  describe('#setOptions', function () {
    var testOptions = {
      http: {
        receivePolicy: {interval: 1}
      }
    };
    /*Tests_SRS_NODE_DEVICE_AMQP_06_001: [The `setOptions` method shall throw a ReferenceError if the `options` parameter has not been supplied.]*/
    [undefined, null, ''].forEach(function (badOptions){
      it('throws if options is \'' + badOptions +'\'', function () {
        var transport = new Amqp({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
        assert.throws(function () {
          transport.setOptions(badOptions);
        }, ReferenceError, '');
      });
    });

    /*Tests_SRS_NODE_DEVICE_AMQP_06_002: [If `done` has been specified the `setOptions` method shall call the `done` callback with no arguments when successful.]*/
    it('calls the done callback with no arguments', function(done) {
      var transport = new Amqp({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
      transport.setOptions(testOptions, done);
    });

    /*Tests_SRS_NODE_DEVICE_AMQP_06_003: [`setOptions` should not throw if `done` has not been specified.]*/
    it('does not throw if `done` is not specified', function() {
      var transport = new Amqp({ host: 'hub.host.name', hubName: 'hub', deviceId: 'deviceId', sas: 'sas.key' });
      assert.doesNotThrow(function() {
        transport.setOptions({});
      });
    });
  });

});