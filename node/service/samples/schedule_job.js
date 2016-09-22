// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

"use strict";

var uuid = require('uuid');
var JobClient = require('azure-iothub').JobClient;

var jobClient = JobClient.fromConnectionString(connectionString);

var connectionString = "[IoT Hub Connection String]";
var jobId = uuid.v4();
var startTime = new Date(Date.now() + 3600000); // an hour from now
var maxExecutionTimeInSeconds =  3600;

var methodParams = {
  methodName: '<method name>',
  payloadJson: null,
  timeoutInSeconds: 42
};

jobClient.scheduleDeviceMethod(jobId,
                               'SELECT * FROM devices',
                               methodParams,
                               startTime,
                               maxExecutionTimeInSeconds,
                               function(err) {
  if (err) {
    console.error('Could not schedule job: ' + err.message);
  } else {
    var jobMonitorInterval = setInterval(function() {
      jobClient.getJob(jobId, function(err, result) {
        if (err) {
          console.error('Could not get job status: ' + err.message);
        } else {
          console.log('Job status: ' + result.status);
          if (result.status === 'completed' || result.status === 'failed' || result.status === 'cancelled') {
            clearInterval(jobMonitorInterval);
          }
        }
      });
    }, 5000);
  }
});