// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var uuid = require('uuid');
var debug = require('debug')('e2etests:JobClient');
var Registry = require('azure-iothub').Registry;
var ConnectionString = require('azure-iothub').ConnectionString;
var JobClient = require('azure-iothub').JobClient;
var DeviceClient = require('azure-iot-device').Client;
var DeviceMqtt = require('azure-iot-device-mqtt').Mqtt;

var runTests = function(hubConnectionString) {
  describe('JobClient', function() {
    this.timeout(120000);

    var testDeviceId = '0000node-e2etests-JobClient-' + uuid.v4();
    var testDevice = null;
    var testDeviceConnectionString = null;

    function continueWith(testCallback, next) {
      return function(err, result) {
        if (err) {
          testCallback(err);
        } else {
          next(result);
        }
      };
    }

    before(function(beforeCallback) {
      var registry = Registry.fromConnectionString(hubConnectionString);
      registry.create({ deviceId: testDeviceId }, continueWith(beforeCallback, function(result) {
        testDevice = result;
        var host = ConnectionString.parse(hubConnectionString).HostName;
        testDeviceConnectionString = 'HostName=' + host + ';DeviceId=' + testDevice.deviceId + ';SharedAccessKey=' + testDevice.authentication.symmetricKey.primaryKey;
        debug('created device with id: ' + testDevice.deviceId);
        beforeCallback();
      }));
    });

    after(function(afterCallback) {
      if(testDevice) {
        var registry = Registry.fromConnectionString(hubConnectionString);
        registry.delete(testDeviceId, function(err) {
          debug('deleted device with id: ' + testDevice.deviceId);
          afterCallback(err);
        });
      } else {
        // Device was not created, exit immediately.
        afterCallback();
      }
    });

    function waitForJobStatus(jobClient, jobId, desiredJobStatus, callback) {
      var waitInterval = setInterval(function(){
        jobClient.getJob(jobId, continueWith(callback, function(job) {
          debug('Got job ' + jobId + ' status: ' + job.status);
          if(job.status === desiredJobStatus) {
            clearInterval(waitInterval);
            callback(null, job);
          } else if (job.status === 'failed' || job.status === 'cancelled'){
            clearInterval(waitInterval);
            callback(new Error('Job ' + jobId + ' status is: ' + job.status));
          } else {
            debug('Job ' + jobId + ' current status: ' + job.status + ' waiting for ' + desiredJobStatus);
          }
        }));
      }, 1000);
    }

    describe('scheduleTwinUpdate', function() {
      it('schedules a twin update job with a sql query and succeeds', function(testCallback) {
        var jobClient = JobClient.fromConnectionString(hubConnectionString);
        var testJobId = uuid.v4();
        var testTwinPatch = {
          etag: '*',
          tags: {
            key: 'value2'
          },
          properties: { desired: {}, reported: {} }
        };

        debug('scheduling a twin update job with a sql query with the id: ' + testJobId);
        debug('Query: ' + 'deviceId = \'' + testDeviceId + '\'');
        jobClient.scheduleTwinUpdate(testJobId, 'deviceId = \'' + testDeviceId + '\'', testTwinPatch, new Date(Date.now()), 120, continueWith(testCallback, function() {
          waitForJobStatus(jobClient, testJobId, 'completed', continueWith(testCallback, function() {
            var registry = Registry.fromConnectionString(hubConnectionString);
            registry.getTwin(testDeviceId, continueWith(testCallback, function(twin) {
              assert.equal(twin.tags.key, testTwinPatch.tags.key);
              testCallback();
            }));
          }));
        }));
      });

      it('schedules a twin update job and cancels it', function(testCallback) {
        var jobClient = JobClient.fromConnectionString(hubConnectionString);
        var testJobId = uuid.v4();
        var fakePatch = { 
          etag: '*', 
          tags: {}, 
          properties: {
            desired: {},
            reported: {}
          }
        };
        jobClient.scheduleTwinUpdate(testJobId, 'deviceId = \'' + testDeviceId + '\'', fakePatch, new Date(Date.now() + 3600000), 120, continueWith(testCallback, function(job) {
          assert.strictEqual(job.jobId, testJobId);
          assert.strictEqual(job.status, 'queued');
          debug('cancelling job ' + testJobId);
          jobClient.cancelJob(testJobId, continueWith(testCallback, function() {
            waitForJobStatus(jobClient, testJobId, 'cancelled', continueWith(testCallback, function() {
              testCallback();
            }));
          }));
        }));
      });
    });

    describe('scheduleDeviceMethod', function() {
      var testDeviceMethod = {
        methodName: 'testMethod',
        payload: null,
        timeoutInSeconds: 30
      };

      it('schedules a device method job with a sql query and succeeds', function(testCallback) {
        var jobClient = JobClient.fromConnectionString(hubConnectionString);
        var testJobId = uuid.v4();
        var methodResponseWasSent = false;
        var deviceClient = DeviceClient.fromConnectionString(testDeviceConnectionString, DeviceMqtt);
        deviceClient.open(continueWith(testCallback, function() {
          deviceClient.onDeviceMethod(testDeviceMethod.methodName, function(request, response) {
            response.send(200, continueWith(testCallback, function() {
              methodResponseWasSent = true;
            }));
          });

          debug('scheduling a device method job with a sql query with the id: ' + testJobId);
          debug('Query: ' + 'deviceId = \'' + testDeviceId + '\'');
          jobClient.scheduleDeviceMethod(testJobId, 'deviceId = \'' + testDeviceId + '\'', testDeviceMethod, new Date(Date.now()), 120, continueWith(testCallback, function() {
            waitForJobStatus(jobClient, testJobId, 'completed', continueWith(testCallback, function() {
              var registry = Registry.fromConnectionString(hubConnectionString);
              registry.getTwin(testDeviceId, continueWith(testCallback, function() {
                assert.isTrue(methodResponseWasSent);
                testCallback();
              }));
            }));
          }));
        }));
      });

      it('schedules a device method job and cancels it', function(testCallback) {
        var jobClient = JobClient.fromConnectionString(hubConnectionString);
        var testJobId = uuid.v4();
        jobClient.scheduleDeviceMethod(testJobId, 'deviceId = \'' + testDeviceId + '\'', testDeviceMethod, new Date(Date.now() + 3600000), 120, continueWith(testCallback, function(job) {
          assert.strictEqual(job.jobId, testJobId);
          assert.strictEqual(job.status, 'queued');
          debug('cancelling job ' + testJobId);
          jobClient.cancelJob(testJobId, continueWith(testCallback, function() {
            waitForJobStatus(jobClient, testJobId, 'cancelled', continueWith(testCallback, function() {
              testCallback();
            }));
          }));
        }));
      });
    });
  });
};

module.exports = runTests;