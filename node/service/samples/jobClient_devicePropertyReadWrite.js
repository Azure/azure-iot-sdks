// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');
var uuid = require('uuid');
var loadDevicesData = require('./utilities.js').loadDevicesData;

// This variable must be populated with a connection string that has the permissions to schedule jobs on the IoT Hub instance.
var connectionString = '[IoT Hub Connection String]';

// The jobClient is the object that we will use to schedule jobs for devices.
var jobClient = iothub.JobClient.fromConnectionString(connectionString);

var readDeviceTwin = function(deviceId, done) {
  var registry = iothub.Registry.fromConnectionString(connectionString);
  registry.get(deviceId, function(err, deviceInfo) {
    if (err) {
      done(err);
    } else {
      console.log('*** Reading from the device registry (shallow read):');
      console.log('--- ' + deviceInfo.deviceId + ' ---');
      var timezone = deviceInfo.deviceProperties.Timezone;
      var batteryLevel = deviceInfo.deviceProperties.BatteryLevel;
      console.log('    ' + 'BatteryLevel' + ': ' + (batteryLevel ? batteryLevel.value : ''));
      console.log('    ' + 'Timezone' + ': ' + (timezone? timezone.value : ''));
      console.log('-------------------------');
      done();
    }
  });
};

var printError = function(message, err) {
  console.log(message + ': ' + err.constructor.name + ': ' + err.message);
};

var waitForJob = function (jobId, done) {
  return function (err) {
    if (err) {
      console.error('Could not schedule job: ' + err.constructor.name + ': ' + err.message);
    } else {
      var jobWaitInterval = setInterval(function() {
        // Get the current job status
        jobClient.getJob(jobId, function (err, jobResp) {
          if (err) {
            done(err);
          } else {
            if (jobResp.status !== 'completed') {
              console.log(jobResp.jobId + ' is ' + jobResp.status);
            } else {
              console.log(jobResp.jobId + ' completed:' + jobResp.failureReason);
              console.log('\tStart time (UTC): ' + jobResp.startTimeUtc);
              console.log('\tEnd time (UTC): ' + jobResp.endTimeUtc);
              clearInterval(jobWaitInterval);
              done();
            }
          }
        });
      }, 5000);
    }
  };
};

loadDevicesData(function (err, deviceData) {
  if (err) {
    console.error('Could not load device data');
    process.exit(1);
  } else {
    var testDevice = deviceData[0];
    
    // Step 1: Do a "shallow" read: this is the state of the device as stored in the Device registry. 
    readDeviceTwin(testDevice.deviceId, function (err) {
      if (err) printError('Could not read device twin', err);
      else {
        // Step 2: Do a "deep read": this means scheduling a job for the device to return the current state of the desired property.
        var readJobId = uuid.v4();
        console.log('*** Starting a deep-read job (' + readJobId + ') to update the registry with the value from the device:');
        jobClient.scheduleDevicePropertyRead(readJobId, testDevice.deviceId, 'timezone', waitForJob(readJobId, function(err) {
          if (err) printError('Could schedule device property read');
          else {
            // Step 3: Read device twin again: it will be updated with the new property value from the deep-read.
            readDeviceTwin(testDevice.deviceId, function (err) {
              if (err) printError('Could not read device twin', err);
              else {
                // Step 4: Do a "deep write": this means scheduling a job for the device to change a property on the device itself.
                var writeJobId = uuid.v4();
                console.log('*** Starting a deep-write job (' + writeJobId + ') to update the property value on the device:');
                jobClient.scheduleDevicePropertyWrite(writeJobId, testDevice.deviceId, { timezone: 'PST' }, waitForJob(writeJobId, function(err) {
                  if (err) printError('Could schedule device property write', err);
                  else {
                    // Step 5: Now that the property has been updated we need to do a deep read again to update the property in the device twin.
                    var readAfterWriteJobId = uuid.v4();
                    console.log('*** Starting a deep-read job (' + readAfterWriteJobId + ') to update the registry with the new value from the device:');
                    jobClient.scheduleDevicePropertyRead(readAfterWriteJobId, testDevice.deviceId, 'timezone', waitForJob(readAfterWriteJobId, function(err) {
                      if (err) printError('Could schedule device property read');
                      else {
                        // Step 6: Read device twin again: it will be updated with the new property value from the deep-write/deep-read.
                        readDeviceTwin(testDevice.deviceId, function (err) {
                          if (err) printError('Could not read device twin', err);
                        });
                      }
                    }));
                  }
                }));
              }
            });
          }
        }));
      }
    });
  }
});