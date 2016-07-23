// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var EventEmitter = require('events').EventEmitter;
var AmqpReceiver = require('../lib/amqp_receiver.js');
var Message = require('azure-iot-common').Message;

describe('AmqpReceiver', function () {
  describe('#constructor', function () {
    /* Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_004: [If the receiver object passed as an argument is falsy, the AmqpReceiver should throw a ReferenceError]*/
    it('throws if amqpReceiver is falsy', function () {
      assert.throws(function () {
        return new AmqpReceiver(null);
      }, ReferenceError, 'amqpReceiver');
    });
  });
  describe('#events', function () {
    /* Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_002: [The created AmqpReceiver object shall emit a ‘message’ event when a message is received.]*/
    /* Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_014: [The parameter to the message event emitted by the AmqpReceiver class should be an azure-iot-common:Message] */
    it('emits a message event when a message is received', function (done) {
      var messageEmitter = new EventEmitter();
      var receiver = new AmqpReceiver(messageEmitter);
      var fakeMessage = {
        body: 'foo',
        properties: {
          to: 'bar'
        }
      };

      receiver.on('message', function (msg) {
        assert.equal(msg.constructor.name, 'Message');
        done();
      });
      messageEmitter.emit('message', fakeMessage);
    });
    
    /* Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_003: [The created AmqpReceiver object shall emit a ‘errorReceived’ event when an error is received.]*/
    it('emits an errorReceived event when an error is received', function (done) {
      var errorEmitter = new EventEmitter();
      var receiver = new AmqpReceiver(errorEmitter);
      receiver.on('errorReceived', done);
      errorEmitter.emit('errorReceived', null);
    });

    /* Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_13_001: [ If the AMQP message has values in it's applicationProperties property then those shall be added to the properties property of the newly created message object. ] */
    it('copies application properties from AMQP message to Message properties', function (done) {
      var messageEmitter = new EventEmitter();
      var receiver = new AmqpReceiver(messageEmitter);
      var fakeMessage = {
        body: 'foo',
        properties: {
          to: 'bar'
        },
        applicationProperties: {
          'k1': 'v1',
          'k2': 'v2',
          'k3': 'v3'
        }
      };

      receiver.on('message', function (msg) {
        assert.isOk(msg.properties);
        var count = msg.properties.count();
        assert.strictEqual(count, 3);
        for(var i = 1; i <= count; ++i) {
          var key = 'k' + i.toString();
          var val = 'v' + i.toString();
          var item = msg.properties.getItem(i - 1);
          assert.isOk(item);
          assert.isOk(item.key);
          assert.isOk(item.value);
          assert.strictEqual(item.key, key);
          assert.strictEqual(item.value, val);
        }
        done();
      });
      messageEmitter.emit('message', fakeMessage);
    });
  });
  
  describe('#complete', function () {
    /*Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_006: [If the message object passed as an argument is falsy, a ReferenceError should be thrown] */
    it('throws if message is falsy', function () {
      var receiver = new AmqpReceiver(new EventEmitter());
      assert.throws(function () {
        receiver.complete(null);
      }, ReferenceError, 'Invalid message object.');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_011: [If the message is successfully settled the receiver should call the done callback with a null error object and a MessageCompleted result object] */
    it('calls the done() callback with a null error object and a result of type MessageCompleted', function (done) {
      var transport = new EventEmitter();
      transport.accept = function () { };
      var receiver = new AmqpReceiver(transport);
      var msg = new Message();
      receiver.complete(msg, function (err, result) {
        assert.isNull(err);
        assert.equal(result.constructor.name, 'MessageCompleted');
        done();
      });
    });
  });
  describe('#abandon', function () {
    /*Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_008: [If the message object passed as an argument is falsy, a ReferenceError should be thrown] */
	  	it('throws if message is falsy', function () {
      var receiver = new AmqpReceiver(new EventEmitter());
      assert.throws(function () {
        receiver.abandon(null);
      }, ReferenceError, 'Invalid message object.');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_012: [If the message is successfully settled the receiver should call the done callback with a null error object and a MessageAbandoned result object] */
    it('calls the done() callback with a null error object and a result of type MessageCompleted', function (done) {
      var transport = new EventEmitter();
      transport.release = function () { };
      var receiver = new AmqpReceiver(transport);
      var msg = new Message();
      receiver.abandon(msg, function (err, result) {
        assert.isNull(err);
        assert.equal(result.constructor.name, 'MessageAbandoned');
        done();
      });
    });
  });
  describe('#reject', function () {
    /*Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_010: [If the message object passed as an argument is falsy, a ReferenceError should be thrown] */
    it('throws if message is falsy', function () {
      var receiver = new AmqpReceiver(new EventEmitter());
      assert.throws(function () {
        receiver.reject(null);
      }, ReferenceError, 'Invalid message object.');
    });

    /*Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_013: [If the message is successfully settled the receiver should call the done callback with a null error object and a MessageRejected result object] */
    it('calls the done() callback with a null error object and a result of type MessageCompleted', function (done) {
      var transport = new EventEmitter();
      transport.reject = function () { };
      var receiver = new AmqpReceiver(transport);
      var msg = new Message();
      receiver.reject(msg, function (err, result) {
        assert.isNull(err);
        assert.equal(result.constructor.name, 'MessageRejected');
        done();
      });
    });
  });
});