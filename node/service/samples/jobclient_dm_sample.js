// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');

var connectionString = '[IoT Hub Connection String]';

console.log('**listing jobs...');

var jobClient = iothub.JobClient.fromConnectionString(connectionString);
// Setting everything to null is the same as doing a SELECT *
var jobQuery = {
  project: null,
  aggregate: null,
  filter: null,
  sort: null
};

jobClient.queryJobHistory(jobQuery, printJobResults);

function printJobResults(err, jobList, res) {
  if (err) console.log('error ' + err.responseBody);
  if (res) console.log('status: ' + res.statusCode + ' ' + res.statusMessage);
  if (jobList) {
    jobList.forEach(function (job) {
      console.log('Found job: ' + job.jobId);
      console.log('\tstatus: ' + job.status);
      console.log('\tstarted: ' + job.startTimeUtc);
      console.log('\tended: ' + job.endTimeUtc);
    });
  }
  else {
    console.log('No job info');
  }
}