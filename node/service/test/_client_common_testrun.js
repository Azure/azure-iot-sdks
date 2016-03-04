// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var Client = require('../lib/client.js');
var errors = require('azure-iot-common').errors;
var Message = require('azure-iot-common').Message;
var AmqpReceiver = require('azure-iot-amqp-base').AmqpReceiver;

function transportSpecificTests(opts) {
  describe('Client', function () {
    var testSubject;
    var deviceId;

    before('prepare test subject', function (done) {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_008: [The open method shall open a connection to the IoT Hub that was identified when the Client object was created (e.g., in Client.fromConnectionString).]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_009: [When the open method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - standard JavaScript Error object (or subclass)]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_010: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
      if (opts.transport && opts.connectionString) assert.fail('test setup error');
      if (opts.transport) testSubject = new Client(opts.transport());
      else testSubject = Client.fromConnectionString(opts.connectionString);
      deviceId = opts.id;
      testSubject.open(function (err) { done(err); });
    });

    after('close the connection', function (done) {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_021: [The close method shall close the connection.]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_022: [When the close method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - standard JavaScript Error object (or subclass)]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_023: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
      testSubject.close(function (err) { done(err); });
    });

    describe('#send', function () {
      function createTestMessage() {
        var msg = new Message('msg');
        msg.expiryTimeUtc = Date.now() + 5000; // Expire 5s from now, to reduce the chance of us hitting the 50-message limit on the IoT Hub
        return msg;
      }

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_016: [When the send method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - standard JavaScript Error object (or subclass)
      response - an implementation-specific response object returned by the underlying protocol, useful for logging and troubleshooting]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_017: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
      it('sends the message', function (done) {
        testSubject.send(deviceId, createTestMessage(), function (err, state) {
            if (!err) {
            assert.equal(state.constructor.name, "MessageEnqueued");
            }
            done(err);
        });
      });

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_014: [The send method shall convert the message object to type azure-iot-common.Message if necessary.]*/
      it('accepts any message that is convertible to type Message', function (done) {
        var message = 'msg';
        testSubject.send(deviceId, message, function (err, state) {
            if (!err) {
            assert.equal(state.constructor.name, "MessageEnqueued");
            }
            done(err);
        });
      });

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_019: [If the deviceId has not been registered with the IoT Hub, send shall return an instance of DeviceNotFoundError.]*/
      it('returns DeviceNotFoundError when sending to an unregistered deviceId', function (done) {
        var unregisteredDeviceId = 'no-device' + Math.random();
        testSubject.send(unregisteredDeviceId, new Message('msg'), function (err) {
            assert.instanceOf(err, errors.DeviceNotFoundError);
            done();
        });
      });
    });

    describe('#getFeedbackReceiver', function () {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_027: [When the getFeedbackReceiver method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - standard JavaScript Error object (or subclass)
      receiver - an instance of Client.FeedbackReceiver]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_028: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
      it('returns a FeedbackReceiver object', function (done) {
        testSubject.getFeedbackReceiver(function (err, receiver) {
            if (err) done(err);
            else {
            assert.instanceOf(receiver, AmqpReceiver);
            done();
            }
        });
      });

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_033: [getFeedbackReceiver shall return the same instance of Client.FeedbackReceiver every time it is called with a given instance of Client.]*/
      it('always returns the same FeedbackReceiver', function (done) {
        testSubject.getFeedbackReceiver(function (err, receiver1) {
            if (err) done(err);
            testSubject.getFeedbackReceiver(function (err, receiver2) {
            if (err) done(err);
            assert.equal(receiver1, receiver2);
            done();
            });
        });
      });

      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_030: [The FeedbackReceiver class shall inherit EventEmitter to provide consumers the ability to listen for (and stop listening for) events.]*/
      it('FeedbackReceiver inherits EventEmitter', function (done) {
        testSubject.getFeedbackReceiver(function (err, receiver) {
            if (err) done(err);
            var EventEmitter = require('events').EventEmitter;
            assert.instanceOf(receiver, EventEmitter);
            done();
        });
      });

      function sendShortLivedC2dMessage(deviceId, messageId, done) {
        var msg = new Message('msg');
        msg.messageId = messageId;
        msg.ack = 'full';

        // Set up the c2d message to expire immediately, so that this test
        // doesn't have to receive the message (acting as the device)--the
        // feedback message will be generated due to message expiration.
        msg.expiryTimeUtc = Date.now() + 100; // 100ms from now

        testSubject.send(deviceId, msg, function (err) {
          if (err) done(err);
        });
      }

      function findFeedbackRecord(deviceId, messageId, records) {
        return records.body.some(function (record) {
          return (record.originalMessageId === messageId && record.deviceId === deviceId);
        });
      }

      // There is one "global" feedback message queue per IoT Hub, and it is
      // designed to be read by only one entity. Multiple readers can swallow
      // each others' messages (i.e., one reader will receive+complete messages
      // which the other reader is looking for). So to run #getFeedbackReceiver
      // tests, we'd need to spin up a new IoT Hub just for those tests, which
      // adds time and adds new potential points of failure. Another option is to
      // create a service that is the *only* reader for our test Hub, then query
      // it from the tests, but that also introduces new potential points of
      // failure. We'll skip this test for now...
      //
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_026: [If the connection has not already been opened (e.g., by a call to open), the getFeedbackReceiver method shall open the connection.]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_031: [FeedbackReceiver shall expose the 'errorReceived' event, whose handler shall be called with the following arguments:
      err – standard JavaScript Error object (or subclass)]*/
      /*SRS_NODE_IOTHUB_CLIENT_05_032: [FeedbackReceiver shall expose the 'message' event, whose handler shall be called with the following arguments when a new feedback message is received from the IoT Hub:
      message – a JavaScript object containing a batch of one or more feedback records]*/
      it.skip('returns a receiver that emits feedback messages', function (done) {
        this.timeout(30000);
        testSubject.getFeedbackReceiver(function (err, receiver) {
          if (err) done(err);
          else {
            var messageId = 'mid-' + Math.random();
            receiver.on('errorReceived', function (err) { done(err); });
            receiver.on('message', function (feedbackRecords) {
              if (findFeedbackRecord(deviceId, messageId, feedbackRecords)) {
                done();
              }
            });

            sendShortLivedC2dMessage(deviceId, messageId, done);
          }
        });
      });
    });
  });
}

module.exports = transportSpecificTests;