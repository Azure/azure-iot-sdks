// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var JobClient = require('../lib/job_client.js');

var testDeviceId = "device-id-test";
var packageUri = "www.bing.com";
var timeout = 60;
var testjobId = 'job-id-test';
var testSysPropName = 'BatteryLevel';
var testValue = 'value';

function commonTests(Transport, goodConnectionString) {
    describe('JobClient', function () {
        describe('#scheduleDeviceConfigurationUpdate', function () {
            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                jobClient.scheduleDeviceConfigurationUpdate(testjobId, testDeviceId, testValue, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });
        });

        describe('#scheduleFirmwareUpdate', function () {
            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                jobClient.scheduleFirmwareUpdate(testjobId, testDeviceId, packageUri, timeout, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });
        });

        describe('#scheduleDevicePropertyRead', function () {
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_004: [When the request completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and an object representing the new job created on the IoT Hub.]*/
            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);

                jobClient.scheduleDevicePropertyRead(testjobId, testDeviceId, testSysPropName, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });
        });

        describe('#scheduleDevicePropertyWrite', function () {
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_004: [When the request completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and an object representing the new job created on the IoT Hub.]*/
            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);

                jobClient.scheduleDevicePropertyWrite(testjobId, testDeviceId, testSysPropName, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });
        });

        describe('#scheduleReboot', function () {
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_004: [When the request completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and an object representing the new job created on the IoT Hub.]*/
            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);

                jobClient.scheduleReboot(testjobId, testDeviceId, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });
        });

        describe('#scheduleFactoryReset', function () {
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_004: [When the request completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and an object representing the new job created on the IoT Hub.]*/
            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);

                jobClient.scheduleFactoryReset(testjobId, testDeviceId, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });
        });

        describe('#getJob', function () {
            it('calls done function if complete', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);

                jobClient.getJob(testjobId, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });
        });

        describe('#queryJobHistory', function () {
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_003: [queryJobHistory shall call the done callback with a null error object and an array of matching jobs if the query is a projection query.]*/
            it('calls the done callback with an array of matching jobs if the query is a projection query', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                // setting everything to null is the equivalent of SELECT *
                var jobQuery = {
                    project: null,
                    aggregate: null,
                    filter: null,
                    sort: null
                };

                jobClient.queryJobHistory(jobQuery, function (err, result) {
                    assert.isNull(err);
                    assert.isArray(result);
                    done();
                });
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_004: [queryJobHistory shall call the done callback with a null error object and an associative array containing the results if the query is an aggregation query. ]*/
            it('calls the done callback with an array of matching jobs if the query is a aggregation query', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                // setting everything to null is the equivalent of SELECT *
                var jobQuery = {
                    project: null,
                    aggregate: {
                        keys: [{
                            name: "Tags",
                            type: "default"
                        }],
                        properties: [{
                            operator: "count",
                            property: null,
                            columnName: "jobCount"
                        }]
                    },
                    filter: null,
                    sort: null
                };

                jobClient.queryJobHistory(jobQuery, function (err, result) {
                    assert.isNull(err);
                    assert.isOk(result);
                    done();
                });
            });
        });
    });
}

module.exports = commonTests;