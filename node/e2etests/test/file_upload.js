// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var fs = require('fs');
var assert = require('chai').assert;

var serviceSdk = require('azure-iothub');
var deviceSdk = require('azure-iot-device');

var runTests = function (deviceTransport, hubConnectionString, provisionedDevice) {
  describe('Device utilizing ' + provisionedDevice.authenticationDescription + ' authentication, connected over ' + deviceTransport.name + ':', function () {

    var serviceClient, deviceClient;

    beforeEach(function () {
      serviceClient = serviceSdk.Client.fromConnectionString(hubConnectionString);
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
              done();
            }
          });
        }
      });
    });
    describe('File Upload', function() {
      this.timeout(60000);
      it('device successfully upload a file and the notification is received by the service', function(done) {
        var testBlobName = 'e2eblob.txt';
        var filePath = './README.md';
        fs.stat(filePath, function (err, fileStats) {
          if(err) {
            done(err);
          } else {
            var testFileSize = fileStats.size;
            var fileStream = fs.createReadStream(filePath);

            serviceClient.open(function(err) {
              if (err) {
                done(err);
              } else {
                serviceClient.getFileNotificationReceiver(function(err, fileNotificationReceiver) {
                  if (err) {
                    done(err);
                  } else {
                    fileNotificationReceiver.on('message', function(msg) {
                      var notification = JSON.parse(msg.data.toString());
                      if (notification.deviceId === provisionedDevice.deviceId && notification.blobName === provisionedDevice.deviceId + '/' + testBlobName) {
                        assert.isString(notification.blobUri);
                        assert.equal(notification.blobSizeInBytes, testFileSize);
                        fileNotificationReceiver.complete(msg, function(err) {
                          done(err);
                        });
                      }
                    });

                    deviceClient.open(function(err) {
                      if (err) {
                        done(err);
                      } else {
                        deviceClient.uploadToBlob(testBlobName, fileStream, fileStats.size, function(err) {
                          if(err) {
                            done(err);
                          }
                        });
                      }
                    });
                  }
                });
              }
            });
          }
        });
      });
    });
  });
};

module.exports = runTests;