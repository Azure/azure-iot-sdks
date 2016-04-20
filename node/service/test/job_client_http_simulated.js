// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

function Response(statusCode) {
  this.statusCode = statusCode;
}

function SimulatedHttpAbstract() {
  this.handleRequest = function (host, done, body) {
    if (host === 'bad') {                      // bad host
      done(new Error('getaddrinfo ENOTFOUND bad'), null, "ENOTFOUND");
    }
    else {
      done(null, body, new Response(204));
    }
  };
}

SimulatedHttpAbstract.prototype.sendHttpRequest = function (verb, path, headers, host, writeData, done) {
  var jobResponse = null;
  var jobDesc = {
    jobId: 'job-id-test',
    startTimeUtc: '2015-08-20T18:08:49.9738417',
    endTimeUtc: '2015-08-20T18:08:49.9738417',
    failureReason: '',
    type: 'firmwareUpdate',
    status: 'enqueued',
    progress: 35
  };
  var pathComponents = path.split('?')[0].split('/');
  if (pathComponents[3] === 'query') {
    var request = JSON.parse(writeData);
    if (request.aggregate) {
      jobResponse = {
        Result: null,
        AggregateResult: {
          jobCount: 42
        }
      };
    } else {
      jobResponse = {
        Result: [jobDesc],
        AggregateResult: null
      };
    }
  } else if (!pathComponents[3]) {
    jobResponse = [jobDesc];
  } else {
    jobResponse = jobDesc;
  }

  this.handleRequest(host, done, JSON.stringify(jobResponse));
};

module.exports = SimulatedHttpAbstract;