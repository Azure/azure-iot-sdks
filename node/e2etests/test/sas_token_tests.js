// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var uuid = require('uuid');
var assert = require('chai').assert;

var serviceSdk = require('azure-iothub');
var createDeviceClient = require('./testUtils.js').createDeviceClient;
var closeDeviceServiceClients = require('./testUtils.js').closeDeviceServiceClients;
var eventHubClient = require('azure-event-hubs').Client;
var Client = require('azure-iot-device').Client;
var Message = require('azure-iot-common').Message;

function waitForEventHubMessages(ehClient, deviceId, callback) {
  var monitorStartTime = Date.now();
  ehClient.open()
    .then(ehClient.getPartitionIds.bind(ehClient))
    .then(function (partitionIds) {
      return partitionIds.map(function (partitionId) {
        return ehClient.createReceiver('$Default', partitionId,{ 'startAfterTime' : monitorStartTime}).then(function(receiver) {
          receiver.on('errorReceived', function(err) {
            callback(err);
            callback = null;
          });
          receiver.on('message', function (eventData) {
            if (eventData.systemProperties['iothub-connection-device-id'] === deviceId) {
              receiver.removeAllListeners();
              callback(null, eventData);
              callback = null;
            }
          });
        });
      });
    })
    .catch(function (error) {
      callback(error.message);
      callback = null;
    });
}
 

var runTests = function (hubConnectionString, deviceTransport, provisionedDevice) {
  describe.skip('Device utilizing ' + provisionedDevice.authenticationDescription + ' authentication and ' + deviceTransport.name, function () {

    var serviceClient, deviceClient, ehClient;
    var oldSasRenewalInterval;

    before(function() {
      oldSasRenewalInterval = Client.sasRenewalInterval;
      this.timeout(500);
    });

    beforeEach(function () {
      this.timeout(500);
      Client.sasRenewalInterval = 5000;

      serviceClient = serviceSdk.Client.fromConnectionString(hubConnectionString);
      deviceClient = createDeviceClient(deviceTransport, provisionedDevice);
      ehClient = eventHubClient.fromConnectionString(hubConnectionString);
    });

    afterEach(function (done) {
      this.timeout(20000);
      
      var closeError;
      closeDeviceServiceClients(deviceClient, serviceClient, function(err) {
        closeError = err;
        if (ehClient) {
          ehClient.close()
            .then(function() {
              done(closeError);
            })
            .catch(function(err) {
              done(closeError || err);
            });
        } else {
          done(closeError);
        }
      });
     });

    after(function() {
      this.timeout(500);

      Client.sasRenewalInterval = oldSasRenewalInterval;
    });

    it('Device renews SAS after connection and is still able to receive C2D', function (done) {
      this.timeout(20000);
      var messageToSend = new Message(uuid.v4());
      messageToSend.expiryTimeUtc = Date.now() + 60000; // Expire 60s from now, to reduce the chance of us hitting the 50-message limit on the IoT Hub

      deviceClient.open(function (err) {
        if (err) return done(err);
        // Immediately resets the interval to avoid piling on renewals
        Client.sasRenewalInterval = oldSasRenewalInterval;

        var foundTheMessage;
        
        deviceClient.on('message', function (msg) {
            if (msg.data.toString() === messageToSend.data.toString()) {
              foundTheMessage = true;
              deviceClient.removeAllListeners('message');
            }

            deviceClient.complete(msg, function (err, result) {
              if (err) return done(err);
              assert.equal(result.constructor.name, 'MessageCompleted');
              if (foundTheMessage) {
                done();
              }
            });
        });

        deviceClient.on('_sharedAccessSignatureUpdated', function() { 
          deviceClient.removeAllListeners('_sharedAccessSignatureUpdated');
          serviceClient.open(function (err) {
            if (err) return done(err);
            serviceClient.send(provisionedDevice.deviceId, messageToSend, function (err) {
              if (err) return done(err);
            });
          });
        });
      });
    });

    it('Device renews SAS after connection and is still able to send messages', function(done) {
      // For Amqp, the test passes, but deviceClient.sendEvent never calls it's done function
      // and the afterEach function times out.
      this.timeout(20000);
      var bufferSize = 1024;
      var buffer = new Buffer(bufferSize);
      var uuidData = uuid.v4();
      buffer.fill(uuidData);

      deviceClient.open(function (err) {
        if (err) return done(err);

        // Immediately resets the interval to avoid piling on renewals
        Client.sasRenewalInterval = oldSasRenewalInterval;
        waitForEventHubMessages(ehClient, provisionedDevice.deviceId, function(err, eventData) {
          if (err) return done(err);
          if ((eventData.body.length === bufferSize) && (eventData.body.indexOf(uuidData) === 0)) {
            done();
          }
        });

        deviceClient.on('_sharedAccessSignatureUpdated', function() { 
          deviceClient.removeAllListeners('_sharedAccessSignatureUpdated');

          var message = new Message(buffer);
          deviceClient.sendEvent(message, function (sendErr) {
            if (sendErr) {
              done(sendErr);
            }
          });
        });
      });
    });
  });
    

};
   
module.exports = runTests;


