// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var serviceSdk = require('azure-iothub');
var deviceSdk = require('azure-iot-device');
var Message = require('azure-iot-common').Message;
var deviceSas = require('azure-iot-device').SharedAccessSignature;
var serviceSas = require('azure-iothub').SharedAccessSignature;
var anHourFromNow = require('azure-iot-common').anHourFromNow;

var EventHubClient = require('azure-event-hubs').Client;

var assert = require('chai').assert;
var debug = require('debug')('e2etests');
var uuid = require('uuid');

var runTests = function (deviceTransport, hubConnectionString, provisionedDevice) {
  describe('Device utilizing ' + provisionedDevice.authenticationDescription + ' authentication, connected over ' + deviceTransport.name + ':', function () {

    var serviceClient, deviceClient, ehClient;

    beforeEach(function () {
      serviceClient = serviceSdk.Client.fromConnectionString(hubConnectionString);
      ehClient = EventHubClient.fromConnectionString(hubConnectionString);
      if (provisionedDevice.hasOwnProperty('primaryKey')) {
        deviceClient = deviceSdk.Client.fromConnectionString(provisionedDevice.connectionString, deviceTransport);
      } else if (provisionedDevice.hasOwnProperty('certificate')) {
        deviceClient = deviceSdk.Client.fromConnectionString(provisionedDevice.connectionString, deviceTransport);
        var options = {
          cert: provisionedDevice.certificate,
          key: provisionedDevice.clientKey,
        };
        deviceClient.setOptions(options);
      } else {
        deviceClient = deviceSdk.Client.fromSharedAccessSignature(provisionedDevice.connectionString, deviceTransport)
      }
    });

    afterEach(function (done) {
      serviceClient.close(function (err) {
        if (err) {
          done(err);
        } else {
          serviceClient = null;
          deviceClient.close(function (err) {
            if (err) {
              done(err);
            } else {
              deviceClient = null;
              ehClient.close().then(function () {
                ehClient = null;
                done();
              }).catch(function (err) {
                done(err);
              });
            }
          });
        }
      });
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
              assert.isNull(err);
              assert.equal(result.constructor.name, 'MessageCompleted');
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
            assert.isNull(sendErr);
            msgSentCounter++;
            debug('Sent ' + msgSentCounter + ' message(s)');
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

    it('Device sends a message of maximum size and it is received by the service', function (done) {
      this.timeout(120000);
      var startTime = Date.now() - 5000;
      var bufferSize = 254 * 1024;
      var buffer = new Buffer(bufferSize);
      buffer.fill('a');
      deviceClient.open(function (openErr) {
        if (openErr) {
          done(openErr);
        } else {
          var message = new Message(buffer);
          deviceClient.sendEvent(message, function (sendErr) {
            assert.isNull(sendErr);
            debug('Message sent at ' + Date.now());
          });
        }
      });

      ehClient.open()
              .then(ehClient.getPartitionIds.bind(ehClient))
              .then(function (partitionIds) {
                return partitionIds.map(function (partitionId) {
                  return ehClient.createReceiver('$Default', partitionId, { 'startAfterTime' : startTime}).then(function(receiver) {
                    receiver.on('errorReceived', done);
                    receiver.on('message', function (eventData) {
                        if (eventData.systemProperties['iothub-connection-device-id'] === provisionedDevice.deviceId) {
                          debug('Event received: ' + eventData.body);
                          if (eventData.body.length === bufferSize) {
                            receiver.removeAllListeners();
                            ehClient.close();
                            done();
                          } else {
                            debug('eventData.body.length: ' + eventData.body.length + ' doesn\'t match bufferSize: ' + bufferSize);
                          }
                        }
                      });
                  });
                });
              })
              .catch(done);
    });

    it('Device can connect and send a smalll message', function(done) {
      this.timeout(60000);
      var startTime = Date.now() - 5000;
      var bufferSize = 1024;
      var buffer = new Buffer(bufferSize);
      buffer.fill('s');
      deviceClient.open(function (openErr) {
        if (openErr) {
          done(openErr);
        } else {
          var message = new Message(buffer);
          deviceClient.sendEvent(message, function (sendErr) {
            assert.isNull(sendErr);
            debug('Message sent at ' + Date.now());
          });
        }
      });

      ehClient.open()
              .then(ehClient.getPartitionIds.bind(ehClient))
              .then(function (partitionIds) {
                return partitionIds.map(function (partitionId) {
                  return ehClient.createReceiver('$Default', partitionId, { 'startAfterTime' : startTime}).then(function(receiver) {
                    receiver.on('errorReceived', done);
                    receiver.on('message', function (eventData) {
                        if (eventData.systemProperties['iothub-connection-device-id'] === provisionedDevice.deviceId) {
                          debug('Event received: ' + eventData.body);
                          if (eventData.body.length === bufferSize) {
                            receiver.removeAllListeners();
                            ehClient.close();
                            done();
                          } else {
                            debug('eventData.body.length: ' + eventData.body.length + ' doesn\'t match bufferSize: ' + bufferSize);
                          }
                        }
                      });
                  });
                });
              })
              .catch(done);
    });

    it('Service can connect', function(done) {
      this.timeout(60000);
      var connStr = serviceSdk.ConnectionString.parse(hubConnectionString);
      var sas = serviceSas.create(connStr.HostName, connStr.SharedAccessKeyName, connStr.SharedAccessKey, anHourFromNow()).toString();
      var client = serviceSdk.Client.fromSharedAccessSignature(sas);
      client.open(function(err, result) {
        if(err) {
          done(err);
        } else {
          assert.equal(result.constructor.name, 'Connected');
          client.close(function(err) {
            if (err) {
              done(err);
            } else {
              done();
            }
          });
        }
      });
    });
  });
};

module.exports = runTests;