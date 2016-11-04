// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
'use strict';

var JobClient = require('azure-iothub').JobClient;

var connectionString = '<IoT Hub Connection String>';
var jobClient = JobClient.fromConnectionString(connectionString);

var query = jobClient.createQuery();
var onResults = function(err, results) {
  if (err) {
    console.error('Failed to fetch the results: ' + err.message);
  } else {
    // Do something with the results
    results.forEach(function(job) {
      console.log(JSON.stringify(job, null, 2));
    });

    if (query.hasMoreResults) {
        query.next(onResults);
    }
  }
};

query.next(onResults);