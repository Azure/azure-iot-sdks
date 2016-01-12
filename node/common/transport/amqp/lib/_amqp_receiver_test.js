// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var EventEmitter = require('events');
var AmqpReceiver = require('./amqp_receiver.js');

describe('AmqpReceiver', function () {
	describe('#constructor', function () {
		/* Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_004: [If the receiver object passed as an argument is falsy, the AmqpReceiver should throw a ReferenceError]*/
		it('throws if amqpReceiver is falsy', function() {
			assert.throws(function() {
				return new AmqpReceiver(null);
			}, ReferenceError, 'amqpReceiver');
		});
	});
	describe('#events', function () {
		/* Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_002: [The created AmqpReceiver object shall emit a ‘message’ event when a message is received.]*/
		it('emits a message event when a message is received', function(done) {
			var messageEmitter = new EventEmitter();
			var receiver = new AmqpReceiver(messageEmitter);
			receiver.on('message', done);
			messageEmitter.emit('message', null);
		});
		/* Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_003: [The created AmqpReceiver object shall emit a ‘errorReceived’ event when an error is received.]*/
		it('emits an errorReceived event when an error is received', function(done) {
			var errorEmitter = new EventEmitter();
			var receiver = new AmqpReceiver(errorEmitter);
			receiver.on('errorReceived', done);
			errorEmitter.emit('errorReceived', null);
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
	});
  	describe('#abandon', function () {  
		/*Tests_SRS_NODE_IOTHUB_AMQPRECEIVER_16_008: [If the message object passed as an argument is falsy, a ReferenceError should be thrown] */
	  	it('throws if message is falsy', function () {
			var receiver = new AmqpReceiver(new EventEmitter());
      		assert.throws(function () {
				receiver.abandon(null);
      		}, ReferenceError, 'Invalid message object.');
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
  });
});