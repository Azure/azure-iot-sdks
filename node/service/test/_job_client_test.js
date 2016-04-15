// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var JobClient = require('../lib/job_client.js');
var SimulatedHttp = require('./job_client_http_simulated.js');
var JobResponse = require('../lib/job_response.js');

var testDeviceId = "device-id-test";
var packageUri = "www.bing.com";
var timeout = 60;
var testjobId = 'job-id-test';
var testSysPropName = 'BatteryLevel';
var testValue = 'value';

function bulkTests(Transport, goodConnectionString, badConnectionString) {
    describe('JobClient', function () {
        describe('#scheduleDeviceConfigurationUpdate', function () {
            var testSubject = new JobClient({});
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_001: [ `scheduleDeviceConfigurationUpdate` shall throw `ReferenceError` if either of the `jobId` or `deviceIds` arguments are falsy. ]*/
            it('throws when jobId is falsy', function () {
                assert.throws(function () {
                    testSubject.scheduleDeviceConfigurationUpdate(null, 'deviceId', 'value');
                }, ReferenceError, 'Argument \'jobId\' is null');
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_001: [ `scheduleDeviceConfigurationUpdate` shall throw `ReferenceError` if either of the `jobId` or `deviceIds` arguments are falsy. ]*/
            it('throws when deviceId is falsy', function () {
                assert.throws(function () {
                    testSubject.scheduleDeviceConfigurationUpdate('jobId', null, 'value');
                }, ReferenceError, 'Argument \'deviceId\' is null');
            });

            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                jobClient.scheduleDeviceConfigurationUpdate(testjobId, testDeviceId, testValue, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.instanceOf(jobResp, JobResponse);
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });
        });

        describe('#fromConnectionString', function () {
            var connStr = 'HostName=a.b.c;SharedAccessKeyName=name;SharedAccessKey=key';

            /* Tests_SRS_NODE_IOTHUB_JOBCLIENT_07_002: [The fromConnectionString method shall throw ReferenceError if the value argument is falsy.] */
            it('throws when value is null', function () {
                assert.throws(function () {
                    JobClient.fromConnectionString(null);
                }, ReferenceError);
            });

            /* Tests_SRS_NODE_IOTHUB_JOBCLIENT_07_003: [Otherwise, it shall derive and transform the needed parts from the connection string in order to create a valid Config object to pass to the constructor.]*/
            it('correctly populates the config structure', function () {
                var client = JobClient.fromConnectionString(connStr);
                assert.equal(client._config.hubName, 'a');
                assert.equal(client._config.host, 'a.b.c');
                assert.match(client._config.sharedAccessSignature, /SharedAccessSignature sr=a\.b\.c&sig=.*&skn=name&se=.*/);
            });

            /* Tests_SRS_NODE_IOTHUB_JOBCLIENT_07_004: [The fromConnectionString method shall return a new instance of the Registry object, as by a call to new Registry(transport).]*/
            it('returns an instance of JobClient', function () {
                var client = JobClient.fromConnectionString(connStr);
                assert.instanceOf(client, JobClient);
            });
        });

        describe('#fromSharedAccessSignature', function () {
            var token = 'SharedAccessSignature sr=a.b.c&sig=signature&skn=keyname&se=expiry';

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_07_005: [The fromSharedAccessSignature method shall throw ReferenceError if the value argument is falsy.]*/
            it('throws when value is null', function () {
                assert.throws(function () {
                    JobClient.fromSharedAccessSignature(null);
                }, ReferenceError);
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_07_006: [Otherwise, it shall derive and transform the needed parts from the shared access signature in order to create a valid Config object to pass to the constructor.]*/
            it('correctly populates the config structure', function () {
                var client = JobClient.fromSharedAccessSignature(token);
                assert.equal(client._config.hubName, 'a');
                assert.equal(client._config.host, 'a.b.c');
                assert.equal(client._config.sharedAccessSignature, token);
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_07_007: [The fromSharedAccessSignature method shall return a new instance of the JobClient object, as by a call to new JobClient(config).]*/
            it('returns an instance of JobClient', function () {
                var client = JobClient.fromSharedAccessSignature(token);
                assert.instanceOf(client, JobClient);
            });
        });

        describe('#scheduleFirmwareUpdate', function (done) {
            it('throws when jobId is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                assert.throws(function () {
                    jobClient.scheduleFirmwareUpdate(null, testDeviceId, packageUri, timeout, done);
                }, ArgumentError);
            });

            it('throws when deviceId is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                assert.throws(function () {
                    jobClient.scheduleFirmwareUpdate(testjobId, null, packageUri, timeout, done);
                }, ArgumentError);
            });

            it('throws when packageUri is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                assert.throws(function () {
                    jobClient.scheduleFirmwareUpdate(testjobId, testDeviceId, null, timeout, done);
                }, ArgumentError);
            });

            it('throws when timeout is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                assert.throws(function () {
                    jobClient.scheduleFirmwareUpdate(testjobId, testDeviceId, packageUri, null, done);
                }, ArgumentError);
            });

            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                jobClient.scheduleFirmwareUpdate(testjobId, testDeviceId, packageUri, timeout, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.instanceOf(jobResp, JobResponse);
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });

            it('calls err when bad connection is encountered', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnectionString, Transport);
                jobClient.scheduleFirmwareUpdate(testjobId, testDeviceId, packageUri, timeout, function (err, jobResp) {
                    assert.instanceOf(err, Error);
                    assert.isUndefined(jobResp);
                    done();
                });
            });

        });

        describe('#scheduleSystemPropertyRead', function () {
            it('throws when jobId is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleSystemPropertyRead(null, testDeviceId, testSysPropName);
                }, ReferenceError);
            });

            it('throws when deviceIds is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleSystemPropertyRead(testjobId, null, testSysPropName);
                }, ReferenceError);
            });

            it('throws when propertyName is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleSystemPropertyRead(testjobId, testDeviceId, null);
                }, ReferenceError);
            });

            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);

                jobClient.scheduleSystemPropertyRead(testjobId, testDeviceId, testSysPropName, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.instanceOf(jobResp, JobResponse);
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });

            it('sets err when bad connection is encountered', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnectionString, Transport);
                jobClient.scheduleSystemPropertyRead(testjobId, testDeviceId, testSysPropName, function (err, jobResp) {
                    assert.instanceOf(err, Error);
                    assert.isUndefined(jobResp);
                    done();
                });
            });

        });

        describe('#getJob', function (done) {
            it('throws when jobId is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.getJob(null, done);
                }, ReferenceError);
            });

            it('doesNotThrow when done function is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.doesNotThrow(function () {
                    jobClient.getJob(testjobId, null);
                });
            });

            it('returns err if bad host is presented', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnectionString, Transport);
                jobClient.getJob(testjobId, function (err) {
                    assert.isNotNull(err);
                    done();
                });
            });

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
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_001: [queryJobHistory shall throw a ReferenceError if the query parameter is falsy]*/
            [null, undefined, ''].forEach(function (testParam) {
                it('throws a ReferenceError if query is \'' + testParam + '\'', function () {
                    var jobClient = JobClient.fromConnectionString(goodConnectionString);
                    assert.throws(function () {
                        jobClient.queryJobHistory(null, function () { });
                    }, ReferenceError);
                });
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_002: [queryJobHistory shall call the done callback with an `Error` object if the request fails.]*/
            it('calls the done callback with an Error object if the request fails', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnStrings, Transport);
                // setting everything to null is the equivalent of SELECT *
                var jobQuery = {
                    project: null,
                    aggregate: null,
                    filter: null,
                    sort: null
                };

                jobClient.queryJobHistory(jobQuery, function (err) {
                    assert.instanceOf(err, Error);
                    done();
                });
            });

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

function makeConnectionString(host, policy, key) {
    return 'HostName=' + host + ';SharedAccessKeyName=' + policy + ';SharedAccessKey=' + key;
}

var connectionString = makeConnectionString('host', 'policy', 'key');
var badConnStrings = makeConnectionString('bad', 'policy', 'key');

describe('Over simulated HTTPS', function () {
    bulkTests(SimulatedHttp, connectionString, badConnStrings);
});
