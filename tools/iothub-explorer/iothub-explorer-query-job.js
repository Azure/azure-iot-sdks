#!/usr/bin/env node
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var program = require('commander');
var prettyjson = require('prettyjson');
var serviceError = require('./common.js').serviceError;
var getSas = require('./common.js').getSas;
var JobClient = require('azure-iothub').JobClient;

program
  .description('Query existing jobs')
  .usage('[options] [job-type] [job-status]')
  .option('-l, --login <connection-string>', 'use the connection string provided as argument to use to authenticate with your IoT Hub instance')
  .option('-r, --raw', 'use this flag to return raw output instead of pretty-printed output')
  .parse(process.argv);

var jobType = program.args[0];
var jobStatus = program.args[1];
var sas = getSas(program.login);

var jobClient =  JobClient.fromSharedAccessSignature(sas);
var query = jobClient.createQuery(jobType, jobStatus);
var onNewResults = function(err, results) {
  if (err) {
    serviceError(err);
  } else {
    results.forEach(function(job) {
      var output = program.raw ? JSON.stringify(job) : prettyjson.render(job);
      console.log(output);
    });

    if(query.hasMoreResults) {
      query.next(onNewResults);
    }
  }
};
query.next(onNewResults);