// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var iothub = require('azure-iothub');
var uuid = require('uuid');

var connectionString = '[IoT Connection String]';
var timeout = '2';
var jobId = 'jobid-'+uuid.v4();
var deviceId = '[device Id]';

var jobClient = iothub.JobClient.fromConnectionString(connectionString);

console.log("Schedule job: "+jobId);
jobClient.scheduleFirmwareUpdate(jobId, deviceId, 'http://www.bing.com/', timeout, function (err, jobResp, response) {
   if (err) {
      console.log('Schedule Firmware Update Failure: ' + err);
      console.log('Response: '+response);
   }
   else {
       console.log(JSON.stringify(jobResp));
       setInterval(function() {
           var jobStatus = '';
           jobClient.getJob(jobId, function (err, jobResp, response) {
           if (err) {
             console.log('getJob failure: '+err);
             console.log('Response: '+response);
           }
           else {
             if (jobResp !== 'completed') {
               console.log(JSON.stringify(jobResp));
             } else {
               clearInterval(jobStatus);
             }
           }
         });
       }, 2000);
   }
});
