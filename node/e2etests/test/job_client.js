// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var uuid = require('uuid');
var assert = require('chai').assert;
var Promise = require('bluebird');
var Registry = require('azure-iothub').Registry;
var JobClient = require('azure-iothub').JobClient;

module.exports = function(hubConnectionString) {
    describe('JobClient', function() {
        this.timeout(60000);

        describe('#scheduleDeviceConfigurationUpdate', function() {
            var deviceIds = [uuid.v4(), uuid.v4(), uuid.v4()];
            var registry = Promise.promisifyAll(Registry.fromConnectionString(hubConnectionString));
            
            before(function() {
                return Promise.map(deviceIds, function (deviceId) {
                    var device = { deviceId: deviceId, serviceProperties: { etag: null, properties: { }, tags: [] } };
                    return registry.createAsync(device);
                });
            });
            
            after(function() {
                return Promise.map(deviceIds, function (deviceId) {
                    return registry.deleteAsync(deviceId);
                });
            });

            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_005: [ If the `deviceIds` argument is not an array, then `scheduleDeviceConfigurationUpdate` shall convert it to an array with one element before using it. ]*/
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_002: [ `scheduleDeviceConfigurationUpdate` shall construct an HTTPS request using information supplied by the caller, as follows:  
            PUT <config.host>/jobs/v2/<jobId>?api-version=<version> HTTP/1.1
            Authorization: <config.sharedAccessSignature>
            UserAgent: <user-agent-string>
            Accept: application/json
            Content-Type: application/json; charset=utf-8
            Host: <config.host>

            {"jobId":"<jobId>","jobParameters":{"Value":"<value>","DeviceIds":<deviceIds>,"jobType":"updateDeviceConfiguration"}}]*/
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_003: [ If `scheduleDeviceConfigurationUpdate` encounters an error before it can send the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (`err.message`). ]*/
            /*Tests_SRS_NODE_IOTHUB_JOBCLIENT_05_004: [ When `scheduleDeviceConfigurationUpdate` completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and a `JobResponse` object representing the new job created on the IoT Hub. */
            [
                { ids: deviceIds[0], descriptionFragment: 'one device' },
                { ids: deviceIds.slice(1), descriptionFragment: 'two devices' }
            ].forEach(function (testArgs) {
                it('sends a request to the server to update Config.value on ' + testArgs.descriptionFragment, function(done) {
                    var jobId = uuid.v4();
                    var client = JobClient.fromConnectionString(hubConnectionString);
                    client.scheduleDeviceConfigurationUpdate(jobId, testArgs.ids, 'value', function(err, response) {
                        if (err) done(err);
                        else {
                            assert.equal(response.jobId, jobId);
                            assert.equal(response.type, 'updateDeviceConfiguration');
                            assert.notInclude(['failed', 'cancelled'], response.status);
                            done();
                        }
                    });
                });
            });
        });
    });
};
