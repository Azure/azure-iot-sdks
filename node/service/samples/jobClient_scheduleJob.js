// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');
var uuid = require('uuid');
var loadDevicesData = require('./utilities.js').loadDevicesData;

// This variable must be populated with a connection string that has the permissions to schedule jobs on the IoT Hub instance.
var connectionString = '[IoT Hub Connection String]';

// The timeout value (in minutes) indicate how much time should the client wait for the job to be scheduled.
var timeoutInMinutes = '25';

// The job id must be a unique identifier that will allow you to query for the job status later.
var jobId = uuid.v4();

// The jobClient is the object that we will use to schedule jobs for devices.
var jobClient = iothub.JobClient.fromConnectionString(connectionString);

console.log("Scheduling job: " + jobId);

loadDevicesData(function (err, deviceData) {
  if (err) {
    console.error('Could not load device data');
    process.exit(1);
  } else {
    var deviceIds = deviceData.map(function(device) { return device.deviceId; });
    /**
     * Every scheduled job API uses the same convention:
     * - it takes the jobId as a first argument
     * - it takes a device ID (or array of device IDs) as second argument
     * - the following arguments are job parameters (be it URL, properties...)
     * - it takes a callback function as the last argument
     */
    //jobClient.scheduleReboot(jobId, deviceId, function(err, result) {...});
    //jobClient.scheduleFactoryReset(jobId, deviceId, function(err, result) {...});
    //jobClient.scheduleDeviceConfigurationUpdate(jobId, deviceId, value, function(err, result) {...});
    //jobClient.scheduleDevicePropertyRead(jobId, deviceId, propertyName, function(err, result) {...});
    //jobClient.scheduleDevicePropertyWrite(jobId, deviceId, propertyName, function(err, result) {...});
    jobClient.scheduleFirmwareUpdate(jobId, deviceIds, 'http://url.to.firmware.update', timeoutInMinutes, function (err, jobResp) {
      if (err) {
        // If there's an error object at this point, the job was not scheduled: there was a problem with the request, that should be explained by the error message
        console.log('Schedule Firmware Update Failure: ' + err.message);
      } else {
        // The job has been scheduled successfully: log the job description returned by the server
        console.log(JSON.stringify(jobResp));

        // Then start a timer that will check for the job status every 2 seconds
        var jobInterval = setInterval(function() {
          // Get the current job status
          jobClient.getJob(jobId, function (err, jobResp) {
            if (err) {
              // If an error happens here, it means that the job status couldn't be retrieved, not that the job failed. The error message should explain why.
              console.log('getJob failure: ' + err.message);
            } else {
              if (jobResp.status !== 'completed') {
                console.log(jobResp.jobId + ' is ' + jobResp.status);
              } else {
                if(jobResp.failureReason) {
                  console.log(jobResp.jobId + ' failed: ' + jobResp.failureReason);
                } else {
                  console.log(jobResp.jobId + ' succeeded.');
                  console.log('\tStart time (UTC): ' + jobResp.startTimeUtc);
                  console.log('\tEnd time (UTC): ' + jobResp.endTimeUtc);
                }
                clearInterval(jobInterval);
              }
            }
          });
        }, 5000);
      }
    });
  }
});