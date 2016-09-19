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
    var deviceId = 'testDevice-node-' + Math.random();

    before('prepare test subject', function (done) {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_008: [The open method shall open a connection to the IoT Hub that was identified when the Client object was created (e.g., in Client.fromConnectionString).]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_009: [When the open method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - standard JavaScript Error object (or subclass)]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_010: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
      opts.registry.create({ deviceId: deviceId, status: "enabled" }, function(err) {
        if (err) {
          done(err);
        } else {
          testSubject = Client.fromConnectionString(opts.connectionString, opts.transport);
          testSubject.open(function (err) { done(err); });
        }
      });
    });

    after('close the connection', function (done) {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_021: [The close method shall close the connection.]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_022: [When the close method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
      err - standard JavaScript Error object (or subclass)]*/
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_023: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
      testSubject.close(function (closeErr) { 
        opts.registry.delete(deviceId, function (registryErr) {
          if (closeErr || registryErr) {
            var aggregateError = new Error('failed to tear down the environment');
            aggregateError.closeError = closeErr;
            aggregateError.registryError = registryErr;
            done(aggregateError); 
          } else {
            done();
          }
        });
      });

      
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
      /*Tests_SRS_NODE_IOTHUB_CLIENT_05_027: [When the `getFeedbackReceiver` method completes, the callback function (indicated by the `done` argument) shall be invoked with the following arguments:
      - `err` - standard JavaScript `Error` object (or subclass): `null` if the operation was successful
      - `receiver` - an `AmqpReceiver` instance: `undefined` if the operation failed]*/
      it('calls the `done` callback with a null error object and an AmqpReceiver object if the operation succeeds', function (done) {
        testSubject.getFeedbackReceiver(function (err, receiver) {
            if (err) done(err);
            else {
              assert.instanceOf(receiver, AmqpReceiver);
              done();
            }
        });
      });
    });

    describe('#getFileNotificationReceiver', function () {
      /*Tests_SRS_NODE_IOTHUB_CLIENT_16_001: [When the `getFileNotificationReceiver` method completes, the callback function (indicated by the `done` argument) shall be invoked with the following arguments:
      - `err` - standard JavaScript `Error` object (or subclass): `null` if the operation was successful
      - `receiver` - an `AmqpReceiver` instance: `undefined` if the operation failed]*/
      it('calls the `done` callback with a null error object and an AmqpReceiver object if the operation succeeds', function (done) {
        testSubject.getFileNotificationReceiver(function (err, receiver) {
          if (err) done(err);
          else {
            assert.instanceOf(receiver, AmqpReceiver);
            done();
          }
        });
      });
    });
  });
}

module.exports = transportSpecificTests;