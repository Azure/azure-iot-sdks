// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var serviceSdk = require('azure-iothub');
var deviceSdk = require('azure-iot-device');
var Message = require('azure-iot-common').Message;
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

    it('Service sends 1 C2D message and it is re-sent until the device completes it', function (done) {
      this.timeout(60000);
      var guid = uuid.v4();

      var abandonnedOnce = false;
      deviceClient.open(function (openErr) {
        if (openErr) {
          done(openErr);
        } else {
          deviceClient.on('message', function (msg) {
            debug('Received a message with guid: ' + msg.data);
            if (msg.data.toString() === guid) {
              if (!abandonnedOnce) {
                debug('Abandon the message with guid ' + msg.data);
                abandonnedOnce = true;
                deviceClient.abandon(msg, function (err, result) {
                  assert.isNull(err);
                  assert.equal(result.constructor.name, 'MessageAbandoned');
                });
              } else {
                debug('Complete the message with guid ' + msg.data);
                deviceClient.complete(msg, function (err, res) {
                  assert.isNull(err);
                  assert.equal(res.constructor.name, 'MessageCompleted');
                  done();
                });
              }
            } else {
              debug('not the message I\'m looking for, completing it to clean the queue (' + msg.data + ')');
              deviceClient.complete(msg, function (err, result) {
                assert.isNull(err);
                assert.equal(result.constructor.name, 'MessageCompleted');
              });
            }
          });
        }
      });

      serviceClient.open(function (serviceErr) {
        if (serviceErr) {
          done(serviceErr);
        } else {
          serviceClient.send(provisionedDevice.deviceId, guid, function (sendErr) {
            assert.isNull(sendErr);
            debug('Sent one message with guid: ' + guid);
          });
        }
      });
    });

    it('Service sends 1 C2D message and it is re-sent until the device rejects it', function (done) {
      this.timeout(60000);
      var guid = uuid.v4();

      var abandonnedOnce = false;
      deviceClient.open(function (openErr) {
        if (openErr) {
          done(openErr);
        } else {
          deviceClient.on('message', function (msg) {
            debug('Received a message with guid: ' + msg.data);
            if (msg.data.toString() === guid) {
              if (!abandonnedOnce) {
                debug('Abandon the message with guid ' + msg.data);
                abandonnedOnce = true;
                deviceClient.abandon(msg, function (err, result) {
                  assert.isNull(err);
                  assert.equal(result.constructor.name, 'MessageAbandoned');
                });
              } else {
                debug('Rejects the message with guid ' + msg.data);
                deviceClient.reject(msg, function (err, res) {
                  assert.isNull(err);
                  assert.equal(res.constructor.name, 'MessageRejected');
                  done();
                });
              }
            } else {
              debug('not the message I\'m looking for, completing it to clean the queue (' + msg.data + ')');
              deviceClient.complete(msg, function (err, result) {
                assert.isNull(err);
                assert.equal(result.constructor.name, 'MessageCompleted');
              });
            }
          });
        }
      });

      serviceClient.open(function (serviceErr) {
        if (serviceErr) {
          done(serviceErr);
        } else {
          serviceClient.send(provisionedDevice.deviceId, guid, function (sendErr) {
            assert.isNull(sendErr);
            debug('Sent one message with guid: ' + guid);
          });
        }
      });
    });
  });
};

module.exports = runTests;