// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var debug = require('debug')('e2etests');
var uuid = require('uuid');
var chalk = require('chalk');

var serviceSdk = require('azure-iothub');
var Message = require('azure-iot-common').Message;
var createDeviceClient = require('./testUtils.js').createDeviceClient;
var closeDeviceServiceClients = require('./testUtils.js').closeDeviceServiceClients;
var closeDeviceEventHubClients = require('./testUtils.js').closeDeviceEventHubClients;
var eventHubClient = require('azure-event-hubs').Client;
var errors = require('azure-iot-common').errors;
var numberOfc2dMessages = 1;

var runTests = function (hubConnectionString, deviceTransport, provisionedDevice) {
  describe('Device utilizing ' + provisionedDevice.authenticationDescription + ' authentication, connected over ' + deviceTransport.name + ' using device/service clients c2d', function () {

    var serviceClient, deviceClient;

    beforeEach(function () {
      this.timeout(20000);
      serviceClient = serviceSdk.Client.fromConnectionString(hubConnectionString);
      deviceClient = createDeviceClient(deviceTransport, provisionedDevice);
    });

    afterEach(function (done) {
      this.timeout(20000);
      closeDeviceServiceClients(deviceClient, serviceClient, done);
    });

    it('Service sends ' + numberOfc2dMessages + ' C2D messages and they are all received by the device', function (done) {
      this.timeout(180000);
      var foundTheMessage = false;
      var messageToSend = new Message(uuid.v4());
      messageToSend.expiryTimeUtc = Date.now() + 60000; // Expire 60s from now, to reduce the chance of us hitting the 50-message limit on the IoT Hub
      deviceClient.open(function (openErr) {
        debug('device has opened.');
        if (openErr) {
          done(openErr);
        } else {
          var myInterval = {};
          debug('about to connect a listener.');
          deviceClient.on('message', function (msg) {
            debug('msg delivered with payload: '+msg.data.toString());
            //
            // Make sure that the message we are looking at is one of the messages that we just sent.
            //
            if (msg.data.toString() === messageToSend.data.toString()) {
              foundTheMessage = true;
              deviceClient.removeAllListeners('message');
              clearInterval(myInterval);
            }
            //
            // It doesn't matter whether this was a message we want, complete it so that the message queue stays clean.
            //
            deviceClient.complete(msg, function (err, result) {
              if (err) {
                done(err);
              } else {
                assert.equal(result.constructor.name, 'MessageCompleted');
              }
            });

            if (foundTheMessage) {
              done();
            }
          });
          debug('about to open the service client');
          setTimeout( function() {
            serviceClient.open(function (serviceErr) {
              if (serviceErr) {
                done(serviceErr);
              } else {
                var msgSentCounter = 0;
                myInterval = setInterval(function () {
                  serviceClient.send(provisionedDevice.deviceId, messageToSend, function (sendErr) {
                    // If the message has been found already, these errors are from unsent "retry" messages so we can safely ignore them.
                    if (sendErr && !foundTheMessage) {
                      clearInterval(myInterval);
                      done(sendErr);
                    } else {
                      if (msgSentCounter < 7) {
                        msgSentCounter++;
                        debug('Sent ' + msgSentCounter + ' message(s)');
                      } else {
                        clearInterval(myInterval);
                        console.log(chalk.red('exceeding retries on c2d'));
                        done(new errors.NotConnectedError('exceeding retries on c2d'));
                      }
                    }
                  });
                }, 2000);
              }
            });
          } ,1000);
        }
      });
    });
  });

  describe('Device utilizing ' + provisionedDevice.authenticationDescription + ' authentication, connected over ' + deviceTransport.name + ' using device/eventhub clients - messaging', function () {

    var deviceClient, ehClient;

    beforeEach(function () {
      this.timeout(20000);
      ehClient = eventHubClient.fromConnectionString(hubConnectionString);
      deviceClient = createDeviceClient(deviceTransport, provisionedDevice);
    });

    afterEach(function (done) {
      this.timeout(20000);
      closeDeviceEventHubClients(deviceClient, ehClient, done);
    });

    it('Device sends a message of maximum size and it is received by the service', function (done) {
      this.timeout(120000);
      var bufferSize = 254*1024;
      var buffer = new Buffer(bufferSize);
      var uuidData = uuid.v4();
      buffer.fill(uuidData);
      ehClient.open()
              .then(ehClient.getPartitionIds.bind(ehClient))
              .then(function (partitionIds) {
                return partitionIds.map(function (partitionId) {
                  return ehClient.createReceiver('$Default', partitionId,{ 'startAfterTime' : Date.now()}).then(function(receiver) {
                    receiver.on('errorReceived', function(err) {
                      done(err);
                    });
                    receiver.on('message', function (eventData) {
                        if (eventData.systemProperties['iothub-connection-device-id'] === provisionedDevice.deviceId) {
                          if ((eventData.body.length === bufferSize) && (eventData.body.indexOf(uuidData) === 0)) {
                            receiver.removeAllListeners();
                            done();
                          } else {
                            debug('eventData.body: ' + eventData.body + ' doesn\'t match: ' + uuidData);
                          }
                        } else {
                          debug('Incoming device id is: '+eventData.systemProperties['iothub-connection-device-id']);
                        }
                      });
                  });
                });
              })
              .then(function () {
                deviceClient.open(function (openErr) {
                  if (openErr) {
                    done(openErr);
                  } else {
                    var message = new Message(buffer);
                    deviceClient.sendEvent(message, function (sendErr) {
                      if (sendErr) {
                        done(sendErr);
                      }
                    });
                  }
                });
              })
              .catch(done);
          });
  });
};

module.exports = runTests;