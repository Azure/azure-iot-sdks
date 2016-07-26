// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var debug = require('debug')('e2etests');
var uuid = require('uuid');

var serviceSdk = require('azure-iothub');
var Message = require('azure-iot-common').Message;
var createDeviceClient = require('./testUtils.js').createDeviceClient;
var closeDeviceServiceClients = require('./testUtils.js').closeDeviceServiceClients;
var closeDeviceEventHubClients = require('./testUtils.js').closeDeviceEventHubClients;
var eventHubClient = require('azure-event-hubs').Client;

var runTests = function (hubConnectionString, deviceTransport, provisionedDevice) {
  describe('Device utilizing ' + provisionedDevice.authenticationDescription + ' authentication, connected over ' + deviceTransport.name + ' using device/service clients c2d', function () {

    var serviceClient, deviceClient;

    beforeEach(function () {
      serviceClient = serviceSdk.Client.fromConnectionString(hubConnectionString);
      deviceClient = createDeviceClient(deviceTransport, provisionedDevice);
    });

    afterEach(function (done) {
      closeDeviceServiceClients(deviceClient, serviceClient, done);
    });

    it('Service sends 5 C2D messages and they are all received by the device', function (done) {
      this.timeout(120000);
      var deviceMessageCounter = 0;

      deviceClient.open(function (openErr) {
        if (openErr) {
          done(openErr);
        } else {
          deviceClient.on('message', function (msg) {
            deviceMessageCounter++;
            debug('Received ' + deviceMessageCounter + ' message(s)');
            deviceClient.complete(msg, function (err, result) {
              if (err) {
                done(err);
              } else {
                assert.equal(result.constructor.name, 'MessageCompleted');
              }
            });

            if (deviceMessageCounter === 5) {
              done();
            }
          });
        }
      });

      serviceClient.open(function (serviceErr) {
        if (serviceErr) {
          done(serviceErr);
        } else {
          var msgSentCounter = 0;
          var sendCallback = function (sendErr) {
            if (sendErr) {
              done(sendErr);
            } else {
              msgSentCounter++;
              debug('Sent ' + msgSentCounter + ' message(s)');
            }
          };

          for (var i = 0; i < 5; i++) {
            debug('Sending message #' + i);
            var msg = new Message({ 'counter': i });
            msg.expiryTimeUtc = Date.now() + 10000; // Expire 10s from now, to reduce the chance of us hitting the 50-message limit on the IoT Hub
            serviceClient.send(provisionedDevice.deviceId, msg, sendCallback);
          }
        }
      });
    });
  });

  describe('Device utilizing ' + provisionedDevice.authenticationDescription + ' authentication, connected over ' + deviceTransport.name + ' using device/eventhub clients - messaging', function () {

    var deviceClient, ehClient;

    beforeEach(function () {
      ehClient = eventHubClient.fromConnectionString(hubConnectionString);
      deviceClient = createDeviceClient(deviceTransport, provisionedDevice);
    });

    afterEach(function (done) {
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