// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var JobClient = require('../lib/job_client.js');
var SimulatedHttp = require('./job_client_http_simulated.js');
var ConnectionString = require('../lib/connection_string.js');
var endpoint = require('azure-iot-common').endpoint;
var packageJson = require('../package.json');
var commonTests = require('./_job_client_common_testrun.js');

var testDeviceId = "device-id-test";
var packageUri = "www.bing.com";
var timeout = 60;
var testjobId = 'job-id-test';

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
                var testValue = 'value';
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
                }, ReferenceError);
            });

            it('throws when deviceId is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                assert.throws(function () {
                    jobClient.scheduleFirmwareUpdate(testjobId, null, packageUri, timeout, done);
                }, ReferenceError);
            });

            it('throws when packageUri is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                assert.throws(function () {
                    jobClient.scheduleFirmwareUpdate(testjobId, testDeviceId, null, timeout, done);
                }, ReferenceError);
            });

            it('throws when timeout is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);
                assert.throws(function () {
                    jobClient.scheduleFirmwareUpdate(testjobId, testDeviceId, packageUri, null, done);
                }, ReferenceError);
            });

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

            it('calls err when bad connection is encountered', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnectionString, Transport);
                jobClient.scheduleFirmwareUpdate(testjobId, testDeviceId, packageUri, timeout, function (err, jobResp) {
                    assert.instanceOf(err, Error);
                    assert.isUndefined(jobResp);
                    done();
                });
            });

        });

        describe('#scheduleDevicePropertyRead', function () {
            var testDevPropName = 'batteryLevel';

            /* Tests_SRS_NODE_IOTHUB_JOBCLIENT_07_015: [ scheduleDevicePropertyRead method shall throw ArgumentError if any argument contains a falsy value.] */
            it('throws when jobId is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleDevicePropertyRead(null, testDeviceId, testDevPropName);
                }, ReferenceError);
            });

            it('throws when deviceIds is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleDevicePropertyRead(testjobId, null, testDevPropName);
                }, ReferenceError);
            });

            it('throws when propertyName is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleDevicePropertyRead(testjobId, testDeviceId, null);
                }, ReferenceError);
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_004: [When the request completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and an object representing the new job created on the IoT Hub.]*/
            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);

                jobClient.scheduleDevicePropertyRead(testjobId, testDeviceId, testDevPropName, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_003: [If an error is encountered while sending the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (`err.message`).]*/
            it('sets err when bad connection is encountered', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnectionString, Transport);
                jobClient.scheduleDevicePropertyRead(testjobId, testDeviceId, testDevPropName, function (err, jobResp) {
                    assert.instanceOf(err, Error);
                    assert.isUndefined(jobResp);
                    done();
                });
            });
            
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_07_016: [`scheduleDevicePropertyRead` shall construct an HTTP request using information supplied by the caller, as follows:
            ```
                PUT <path>?api-version=<version> HTTP/1.1
                Authorization: <config.sharedAccessSignature>
                UserAgent: packageJson.name + '/' + packageJson.version
                Content-Type: application/json; charset=utf-8
                'Accept': 'application/json',
                {
                    "jobId":"<jobId>",
                    "jobParameters":{
                        "DevicePropertyNames":"<propertyNames>",
                        "DeviceIds":<deviceIds>,
                        "jobType":"readDeviceProperties"
                    }
                }
            ```]*/
            it('builds a valid HTTP request', function(done) {
                var SpyTransport = function() { };
                SpyTransport.prototype.sendHttpRequest = function (verb, path, headers, host, writeData) {
                    var cs = ConnectionString.parse(goodConnectionString);
                    assert.equal(verb, 'PUT');
                    assert.equal(host, cs.HostName);
                    assert.equal(path, '/jobs/v2/' + testjobId + endpoint.versionQueryString());
                    assert.equal(headers.Accept, 'application/json');
                    assert.equal(headers['Content-Type'], 'application/json; charset=utf-8');
                    assert.isOk(headers.Authorization);
                    assert.equal(headers.UserAgent, packageJson.name + '/' + packageJson.version);
                    var requestBody = JSON.parse(writeData);
                    assert.equal(requestBody.jobId, testjobId);
                    assert.isArray(requestBody.jobParameters.DeviceIds);
                    assert.isArray(requestBody.jobParameters.DevicePropertyNames);
                    assert.equal(requestBody.jobParameters.jobType, 'readDeviceProperties');
                    done();
                };
                
                var jobClient = JobClient.fromConnectionString(goodConnectionString, SpyTransport);
                jobClient.scheduleDevicePropertyRead(testjobId, testDeviceId, testDevPropName);
            });
        });

        describe('#scheduleDevicePropertyWrite', function () {
            var testDevProperty = { timezone: 'PST' };

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_005: [** `scheduleDevicePropertyWrite` method shall throw a `ReferenceError` if any argument except 'done' contains a falsy value.]*/
            it('throws when jobId is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleDevicePropertyWrite(null, testDeviceId, testDevProperty);
                }, ReferenceError);
            });

            it('throws when deviceIds is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleDevicePropertyWrite(testjobId, null, testDevProperty);
                }, ReferenceError);
            });

            it('throws when properties is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleDevicePropertyWrite(testjobId, testDeviceId, null);
                }, ReferenceError);
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_004: [When the request completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and an object representing the new job created on the IoT Hub.]*/
            it('calls done callback with Job Id', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);

                jobClient.scheduleDevicePropertyWrite(testjobId, testDeviceId, testDevProperty, function (err, jobResp) {
                    if (err) {
                        done(err);
                    } else {
                        assert.equal(jobResp.jobId, testjobId);
                        done();
                    }
                });
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_003: [If an error is encountered while sending the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (`err.message`).]*/
            it('sets err when bad connection is encountered', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnectionString, Transport);
                jobClient.scheduleDevicePropertyWrite(testjobId, testDeviceId, testDevProperty, function (err, jobResp) {
                    assert.instanceOf(err, Error);
                    assert.isUndefined(jobResp);
                    done();
                });
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_008: [If the `propertyNames` argument is not an array, then `scheduleDevicePropertyWrite` shall convert it to an array with one element before using it. ]*/
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_007: [If the `deviceIds` argument is not an array, then `scheduleDevicePropertyWrite` shall convert it to an array with one element before using it. ]*/
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_006: [`scheduleDevicePropertyWrite` shall construct an HTTP request using information supplied by the caller, as follows:
            ```
                PUT <path>?api-version=<version> HTTP/1.1
                Authorization: <config.sharedAccessSignature>
                UserAgent: packageJson.name + '/' + packageJson.version
                Content-Type: application/json; charset=utf-8
                'Accept': 'application/json',
                {
                    "jobId":"<jobId>",
                    "jobParameters":{
                        "DevicePropertyNames":"<propertyNames>",
                        "DeviceIds":<deviceIds>,
                        "jobType":"writeDeviceProperties"
                    }
                }
            ```]*/
            it('builds a valid HTTP request', function(done) {
                var SpyTransport = function() { };
                SpyTransport.prototype.sendHttpRequest = function (verb, path, headers, host, writeData) {
                    var cs = ConnectionString.parse(goodConnectionString);
                    assert.equal(verb, 'PUT');
                    assert.equal(host, cs.HostName);
                    assert.equal(path, '/jobs/v2/' + testjobId + endpoint.versionQueryString());
                    assert.equal(headers.Accept, 'application/json');
                    assert.equal(headers['Content-Type'], 'application/json; charset=utf-8');
                    assert.isOk(headers.Authorization);
                    assert.equal(headers.UserAgent, packageJson.name + '/' + packageJson.version);
                    var requestBody = JSON.parse(writeData);
                    assert.equal(requestBody.jobId, testjobId);
                    assert.isArray(requestBody.jobParameters.DeviceIds);
                    assert.isOk(requestBody.jobParameters.DeviceProperties);
                    assert.deepEqual(requestBody.jobParameters.DeviceProperties, testDevProperty);
                    assert.equal(requestBody.jobParameters.jobType, 'writeDeviceProperties');
                    done();
                };
                
                var jobClient = JobClient.fromConnectionString(goodConnectionString, SpyTransport);
                jobClient.scheduleDevicePropertyWrite(testjobId, testDeviceId, testDevProperty);
            });
        });

        describe('#scheduleReboot', function () {
            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_009: [`scheduleReboot` method shall throw a `ReferenceError` if any argument except 'done' contains a falsy value.]*/
            it('throws when jobId is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleReboot(null, testDeviceId);
                }, ReferenceError);
            });

            it('throws when deviceIds is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleReboot(testjobId, null);
                }, ReferenceError);
            });

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

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_003: [If an error is encountered while sending the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (`err.message`).]*/
            it('sets err when bad connection is encountered', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnectionString, Transport);
                jobClient.scheduleReboot(testjobId, testDeviceId, function (err, jobResp) {
                    assert.instanceOf(err, Error);
                    assert.isUndefined(jobResp);
                    done();
                });
            });

            /*Codes_Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_011: [If the `deviceIds` argument is not an array, then `scheduleReboot` shall convert it to an array with one element before using it.]*/
            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_010: [`scheduleReboot` shall construct an HTTP request using information supplied by the caller, as follows:
            ```
                PUT <path>?api-version=<version> HTTP/1.1
                Authorization: <config.sharedAccessSignature>
                UserAgent: packageJson.name + '/' + packageJson.version
                Content-Type: application/json; charset=utf-8
                'Accept': 'application/json',
                {
                    "jobId":"<jobId>",
                    "jobParameters":{
                        "DeviceIds":<deviceIds>,
                        "jobType":"rebootDevice"
                    }
                }
            ```]*/
            it('builds a valid HTTP request', function(done) {
                var SpyTransport = function() { };
                SpyTransport.prototype.sendHttpRequest = function (verb, path, headers, host, writeData) {
                    var cs = ConnectionString.parse(goodConnectionString);
                    assert.equal(verb, 'PUT');
                    assert.equal(host, cs.HostName);
                    assert.equal(path, '/jobs/v2/' + testjobId + endpoint.versionQueryString());
                    assert.equal(headers.Accept, 'application/json');
                    assert.equal(headers['Content-Type'], 'application/json; charset=utf-8');
                    assert.isOk(headers.Authorization);
                    assert.equal(headers.UserAgent, packageJson.name + '/' + packageJson.version);
                    var requestBody = JSON.parse(writeData);
                    assert.equal(requestBody.jobId, testjobId);
                    assert.isArray(requestBody.jobParameters.DeviceIds);
                    assert.equal(requestBody.jobParameters.jobType, 'rebootDevice');
                    done();
                };
                
                var jobClient = JobClient.fromConnectionString(goodConnectionString, SpyTransport);
                jobClient.scheduleReboot(testjobId, testDeviceId);
            });
        });

        describe('#scheduleFactoryReset', function () {
            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_012: [`scheduleFactoryReset` method shall throw a `ReferenceError` if any argument except 'done' contains a falsy value.]*/
            it('throws when jobId is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleFactoryReset(null, testDeviceId);
                }, ReferenceError);
            });

            it('throws when deviceIds is null', function () {
                var jobClient = JobClient.fromConnectionString(goodConnectionString);
                assert.throws(function () {
                    jobClient.scheduleFactoryReset(testjobId, null);
                }, ReferenceError);
            });

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

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_003: [If an error is encountered while sending the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (`err.message`).]*/
            it('sets err when bad connection is encountered', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnectionString, Transport);
                jobClient.scheduleFactoryReset(testjobId, testDeviceId, function (err, jobResp) {
                    assert.instanceOf(err, Error);
                    assert.isUndefined(jobResp);
                    done();
                });
            });

            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_014: [If the `deviceIds` argument is not an array, then `scheduleFactoryReset` shall convert it to an array with one element before using it.]*/
            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_013: [`scheduleFactoryReset` shall construct an HTTP request using information supplied by the caller, as follows:
            ```
                PUT <path>?api-version=<version> HTTP/1.1
                Authorization: <config.sharedAccessSignature>
                UserAgent: packageJson.name + '/' + packageJson.version
                Content-Type: application/json; charset=utf-8
                'Accept': 'application/json',
                {
                    "jobId":"<jobId>",
                    "jobParameters":{
                        "DeviceIds":<deviceIds>,
                        "jobType":"factoryResetDevice"
                    }
                }
            ```]*/
            it('builds a valid HTTP request', function(done) {
                var SpyTransport = function() { };
                SpyTransport.prototype.sendHttpRequest = function (verb, path, headers, host, writeData) {
                    var cs = ConnectionString.parse(goodConnectionString);
                    assert.equal(verb, 'PUT');
                    assert.equal(host, cs.HostName);
                    assert.equal(path, '/jobs/v2/' + testjobId + endpoint.versionQueryString());
                    assert.equal(headers.Accept, 'application/json');
                    assert.equal(headers['Content-Type'], 'application/json; charset=utf-8');
                    assert.isOk(headers.Authorization);
                    assert.equal(headers.UserAgent, packageJson.name + '/' + packageJson.version);
                    var requestBody = JSON.parse(writeData);
                    assert.equal(requestBody.jobId, testjobId);
                    assert.isArray(requestBody.jobParameters.DeviceIds);
                    assert.equal(requestBody.jobParameters.jobType, 'factoryResetDevice');
                    done();
                };
                
                var jobClient = JobClient.fromConnectionString(goodConnectionString, SpyTransport);
                jobClient.scheduleFactoryReset(testjobId, testDeviceId);
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

        describe('#getJobs', function() {  
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_016: [** `getJobs` shall call the `done` callback with an `Error` object if the request fails.]*/
            it('returns err if bad host is presented', function (done) {
                var jobClient = JobClient.fromConnectionString(badConnectionString, Transport);
                jobClient.getJobs(function (err) {
                    assert.isNotNull(err);
                    done();
                });
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_16_017: [** `getJobs` shall call the `done` callback with a `null` error object and an array of jobs if the request succeeds.]*/
            it('calls done function if complete', function (done) {
                var jobClient = JobClient.fromConnectionString(goodConnectionString, Transport);

                jobClient.getJobs(function (err, jobs) {
                    if (err) {
                        done(err);
                    } else {
                        assert.isArray(jobs);
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
    commonTests(SimulatedHttp, connectionString);
});
