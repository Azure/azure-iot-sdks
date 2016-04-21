// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');

// The connection string used here must have 'registry read' permissions to be able to use the query APIs.
var connectionString = '[IoT Hub Connection String]';

// The JobClient objects has method to schedule jobs, and query existing jobs and job history.
var jobClient = iothub.JobClient.fromConnectionString(connectionString);

// A query with all parameters set to null is an equivalent of SELECT *
var jobQuery = {
  project: null,
  aggregate: null,
  filter: null,
  sort: null
};

// This aggregate query will count the number of jobs for each job type.
var aggregateQuery = {
  project: null,
  aggregate: {
    keys: [{
      name: 'Type',
      type: 'default'
    }],
    properties: [{
      operator: 'count',
      property: {
        name: 'Type',
        type: 'default'
      },
      columnName: 'JobCount'
    }]
  }
};

// First we run the aggregation query, see how many jobs per type we have in the history
jobClient.queryJobHistory(aggregateQuery, function (err, result) {
  if (err) {
    console.error(err.message);
  } else {
    // result is an array of aggregated results, one of each key (in this case, the type of job)
    result.forEach(function (jobCountResult) {
      console.log('Found ' + jobCountResult.JobCount + ' jobs of type ' + jobCountResult.Type);
    });
    
    // this second query is a projection, we just list all jobs in the history
    jobClient.queryJobHistory(jobQuery, function (err, jobList) {
      if (err) {
        console.log(err.message);
      } else {
        if (jobList) {
          jobList.forEach(function (job) {
            console.log('Job ID: ' + job.jobId);
            console.log('\ttype: ' + job.type);
            console.log('\tstatus: ' + job.status);
            console.log('\tstarted: ' + job.startTimeUtc);
            console.log('\tended: ' + job.endTimeUtc);
          });
        }
        else {
          console.log('No job matching the query in the history');
        }
      }
    });
  }
});