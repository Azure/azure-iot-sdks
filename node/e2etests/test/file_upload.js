// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var fs = require('fs');
var assert = require('chai').assert;

var serviceSdk = require('azure-iothub');
var deviceSdk = require('azure-iot-device');
var deviceAmqp = require('azure-iot-device-amqp').Amqp;

var fileUploadTests = function(hubConnectionString, deviceConnectionString, deviceId) {
  describe('File Upload', function() {
    this.timeout(60000);
    it('device successfully upload a file and the notification is received by the service', function(done) {
      var deviceClient = deviceSdk.Client.fromConnectionString(deviceConnectionString, deviceAmqp);
      var serviceClient = serviceSdk.Client.fromConnectionString(hubConnectionString);
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
                    if (notification.deviceId === deviceId && notification.blobName === deviceId + '/' + testBlobName) {
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
};

module.exports = fileUploadTests;