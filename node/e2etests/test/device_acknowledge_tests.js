// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var serviceSdk = require('azure-iothub');
var createDeviceClient = require('./testUtils.js').createDeviceClient;
var closeDeviceServiceClients = require('./testUtils.js').closeDeviceServiceClients;

var assert = require('chai').assert;
var debug = require('debug')('e2etests');
var uuid = require('uuid');

var runTests = function (hubConnectionString, deviceTransport, provisionedDevice) {
  describe('Device utilizing ' + provisionedDevice.authenticationDescription + ' authentication, connected over ' + deviceTransport.name, function () {

    var serviceClient, deviceClient;

    beforeEach(function () {
      serviceClient = serviceSdk.Client.fromConnectionString(hubConnectionString);
      deviceClient = createDeviceClient(deviceTransport, provisionedDevice);
    });

    afterEach(function (done) {
      closeDeviceServiceClients(deviceClient, serviceClient, done);
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
                  if(err) {
                    done(err);
                  } else {
                    assert.equal(result.constructor.name, 'MessageAbandoned');
                  }
                });
              } else {
                debug('Complete the message with guid ' + msg.data);
                deviceClient.complete(msg, function (err, res) {
                  if(res) {
                    assert.equal(res.constructor.name, 'MessageCompleted');
                  }
                  done(err);
                });
              }
            } else {
              debug('not the message I\'m looking for, completing it to clean the queue (' + msg.data + ')');
              deviceClient.complete(msg, function (err, result) {
                if (err) {
                  done(err);
                } else {
                  if(result) {
                    assert.equal(result.constructor.name, 'MessageCompleted');
                  }
                }
              });
            }
          });
          serviceClient.open(function (serviceErr) {
            if (serviceErr) {
              done(serviceErr);
            } else {
              serviceClient.send(provisionedDevice.deviceId, guid, function (sendErr) {
                debug('Sent one message with guid: ' + guid);
                if (sendErr) {
                  done(sendErr);
                }
              });
            }
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
                  done(err);
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
          serviceClient.open(function (serviceErr) {
            if (serviceErr) {
              done(serviceErr);
            } else {
              serviceClient.send(provisionedDevice.deviceId, guid, function (sendErr) {
                debug('Sent one message with guid: ' + guid);
                if (sendErr) {
                  done(sendErr);
                }
              });
            }
          });
       }
      });
    });
  });
};

module.exports = runTests;