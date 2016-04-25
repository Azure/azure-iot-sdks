// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var anHourFromNow = require('azure-iot-common').anHourFromNow;
var ConnectionString = require('./connection_string.js');
var endpoint = require('azure-iot-common').endpoint;
var packageJson = require('../package.json');
var SharedAccessSignature = require('./shared_access_signature.js');
var DefaultTransport = require('./job_client_http.js');

/**
 * @class           module:azure-iothub.JobClient
 * @classdesc
 * @param {Object}  config      object with three properties:
 *                              host - (string) the fully-qualified DNS hostname of an IoT hub
 *                              hubName - (string) the name of the IoT hub, which is the first segment of hostname
 *                                  sharedAccessSignature - (string) a shared access signature generated from the
 *                                  credentials of a policy with the appropriate registry persmissions (read and/or write).
 */
/* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_001: [The JobClient constructor shall accept a config object] */
function JobClient(config, transport) {
    this._config = config;
    this._httpTransport = transport;
}

/**
 * @method          module:azure-iothub.JobClient.fromConnectionString
 * @description     Constructs a JobClient object from the given connection
 *                  string
 *                  ({@link module:azure-iothub.Http|Http}).
 * @param {String}  value       A connection string which encapsulates the
 *                              appropriate (read and/or write) JobClient
 *                              permissions.
 * @returns {module:azure-iothub.JobClient}
 */
JobClient.fromConnectionString = function fromConnectionString(value, Transport) {
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_002: [The fromConnectionString method shall throw ReferenceError if the value argument is falsy.] */
    if (!value) throw new ReferenceError('value is \'' + value + '\'');

    var UseTransport = Transport || DefaultTransport;

    var cn = ConnectionString.parse(value);
    var sas = SharedAccessSignature.create(cn.HostName, cn.SharedAccessKeyName, cn.SharedAccessKey, anHourFromNow());
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_003: [Otherwise, it shall derive and transform the needed parts from the connection string in order to create a valid Config object to pass to the constructor.]*/
    var config = {
        host: cn.HostName,
        hubName: cn.HostName.split('.', 1)[0],
        sharedAccessSignature: sas.toString()
    };
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_004: [The fromConnectionString method shall return a new instance of the Registry object, as by a call to new Registry(transport).]*/
    return new JobClient(config, new UseTransport());
};

/**
 * @method            module:azure-iothub.JobClient.fromSharedAccessSignature
 * @description       Constructs a JobClient object from the given shared access
 *                    signature
 *                    ({@link module:azure-iothub.Http|Http}).
 * @param {String}    value     A shared access signature which encapsulates
 *                              the appropriate (read and/or write) Registry
 *                              permissions.
 * @returns {module:azure-iothub.JobClient}
 */
JobClient.fromSharedAccessSignature = function fromSharedAccessSignature(value, Transport) {
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_005: [The fromSharedAccessSignature method shall throw ReferenceError if the value argument is falsy.] */
    if (!value) throw new ReferenceError('value is \'' + value + '\'');

    var UseTransport = Transport || DefaultTransport;

    var sas = SharedAccessSignature.parse(value);
    var config = {
        host: sas.sr,
        hubName: sas.sr.split('.', 1)[0],
        sharedAccessSignature: sas.toString()
    };
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_007: [The fromSharedAccessSignature method shall return a new instance of the Registry object, as by a call to new Registry(transport).]*/
    return new JobClient(config, new UseTransport());
};

/*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_003: [If an error is encountered while sending the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (`err.message`).]*/  
/*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_004: [When the request completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and an object representing the new job created on the IoT Hub.]*/

JobClient.prototype._scheduleJob = function (jobDesc, done) {
    var config = this._config;
    var httpHeaders = {
        'Authorization': config.sharedAccessSignature,
        'UserAgent': packageJson.name + '/' + packageJson.version,
        'Accept': 'application/json',
        'Content-Type': 'application/json; charset=utf-8',
    };

    var path = '/jobs/v2/' + jobDesc.jobId + endpoint.versionQueryString();
    this._httpTransport.sendHttpRequest('PUT', path, httpHeaders, config.host, JSON.stringify(jobDesc), function (err, body, response) {
        if (!err) {
            var jobInfo = body ? JSON.parse(body) : null;
            done(null, jobInfo, response);
        }
        else {
            err.response = response;
            err.responseBody = body;
            done(err);
        }
    });
};

/**
 * @method            module:azure-iothub.JobClient#scheduleFirmwareUpdate`
 * @description       .
 * @param {Object}    jobId         An object which must include a `deviceId`
 *                                  property whose value is a valid device identifier.
 *                    deviceId      A string that indicates the device that is being Updated 
 *                    packageUri    Uri of the package that will be downloaded
 *                    timeout       Number of seconds before the operation times out
 * @param {Function}  done          function to call when the operation is
 *                                  complete. `done` will be called with three
 *                                  arguments: an Error object (can be null), an
 *                                  object representing the scheduled job, and a transport-specific response
 *                                  object useful for logging or debugging.
 */
JobClient.prototype.scheduleFirmwareUpdate = function (jobId, deviceIds, packageUri, timeout, done) {
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_008: [scheduleFirmwareUpdate method shall throw ReferenceError if any argument contains a falsy value.] */
    if (!jobId) throw new ReferenceError('The object \'jobId\' is not valid');
    if (!deviceIds) throw new ReferenceError('The object \'deviceIds\' is not valid');
    if (!packageUri) throw new ReferenceError('The object \'packageUri\' is not valid');
    if (!timeout) throw new ReferenceError('The object \'timeout\' is not valid');

    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_014: [ If the deviceIds argument is not an array, then getJob shall convert it to an array with one element before using it.] */
    if (!Array.isArray(deviceIds)) deviceIds = [deviceIds];

    /*Codes_Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_009: [`scheduleFirmwareUpdate` shall construct an HTTP request using information supplied by the caller, as follows:
    ```
    PUT <config.host>/jobs/v2/<jobId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Content-Type: application/json; charset=utf-8
    Accept: application/json,
    {
    "jobId":<jobId>,
    "jobParameters":{
        "DeviceIds":<deviceId>,
        "PackageUri":<packageUri>,
        "Timeout":<timeout>,
        "jobType":"firmwareUpdate"
    }
    }
    ```]*/
    var firmwareUpdateJob = {
        "jobId": jobId,
        "jobParameters": {
            "DeviceIds": deviceIds,
            "PackageUri": packageUri,
            "Timeout": '00:' + timeout + ':00', // Will be replace with the Timespan object after some research
            "jobType": "firmwareUpdate"
        }
    };

    this._scheduleJob(firmwareUpdateJob, done);
};

JobClient.prototype.scheduleDeviceConfigurationUpdate = function (jobId, deviceIds, value, done) {
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_001: [ `scheduleDeviceConfigurationUpdate` shall throw `ReferenceError` if either of the `jobId` or `deviceIds` arguments are falsy. ]*/
    if (!jobId) throw new ReferenceError('Argument \'jobId\' is ' + jobId);
    if (!deviceIds) throw new ReferenceError('Argument \'deviceId\' is ' + deviceIds);

    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_005: [ If the `deviceIds` argument is not an array, then `scheduleDeviceConfigurationUpdate` shall convert it to an array with one element before using it. ]*/
    if (!Array.isArray(deviceIds)) deviceIds = [deviceIds];

    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_002: [`scheduleDeviceConfigurationUpdate` shall construct an HTTPS request using information supplied by the caller, as follows:  
    ```
    PUT <config.host>/jobs/v2/<jobId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    UserAgent: <user-agent-string>
    Accept: application/json
    Content-Type: application/json; charset=utf-8
    Host: <config.host>

    {
    "jobId":"<jobId>",
    "jobParameters":{
        "Value":"<value>",
        "DeviceIds":<deviceIds>,
        "jobType":"updateDeviceConfiguration"
    }
    }
    ```]*/
    var deviceConfigurationUpdateJob = {
        jobId: jobId,
        jobParameters: {
            Value: value,
            DeviceIds: deviceIds,
            jobType: 'updateDeviceConfiguration'
        }
    };

    this._scheduleJob(deviceConfigurationUpdateJob, done);
};

/**
 * @method            module:azure-iothub.JobClient#scheduleDevicePropertyRead`
 * @description       Schedules a Job to update the System defined Device Property value.
 * @param {Object}    jobId         An object which must include a `deviceId`
 *                                  property whose value is a valid device identifier.
 *                    deviceId      A string that indicates the device that is being Updated 
 *                    propertyName  The Property to Read
 * @param {Function}  done          function to call when the operation is
 *                                  complete. `done` will be called with three
 *                                  arguments: an Error object (can be null), an
 *                                  object representing the scheduled job, and a transport-specific response
 *                                  object useful for logging or debugging.
 */
JobClient.prototype.scheduleDevicePropertyRead = function (jobId, deviceIds, propertyName, done) {
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_015: [ scheduleDevicePropertyRead method shall throw ArgumentError if any argument contains a falsy value.] */
    if (!jobId) throw new ReferenceError('The object \'jobId\' is not valid');
    if (!deviceIds) throw new ReferenceError('The object \'deviceIds\' is not valid');
    if (!propertyName) throw new ReferenceError('The object \'propertyName\' is not valid');

    if (!Array.isArray(deviceIds)) deviceIds = [deviceIds];
    if (!Array.isArray(propertyName)) propertyName = [propertyName];

    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_016: [`scheduleDevicePropertyRead` shall construct an HTTP request using information supplied by the caller, as follows:
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
    var devPropUpdate = {
        "jobId": jobId,
        "jobParameters": {
            "DevicePropertyNames": propertyName,
            "DeviceIds": deviceIds,
            "jobType": "readDeviceProperties"
        }
    };

    this._scheduleJob(devPropUpdate, done);
};

/**
 * @method            module:azure-iothub.JobClient#scheduleDevicePropertyWrite`
 * @description       Schedules a Job to update the System defined Device Property value.
 * @param {Object}    jobId         An object which must include a `deviceId`
 *                                  property whose value is a valid device identifier.
 *                    deviceId      A string that indicates the device that is being Updated 
 *                    propertyName  The Property to write
 * @param {Function}  done          function to call when the operation is
 *                                  complete. `done` will be called with three
 *                                  arguments: an Error object (can be null), an
 *                                  object representing the scheduled job, and a transport-specific response
 *                                  object useful for logging or debugging.
 */
JobClient.prototype.scheduleDevicePropertyWrite = function (jobId, deviceIds, propertyName, done) {
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_005: [** `scheduleDevicePropertyWrite` method shall throw a `ReferenceError` if any argument except 'done' contains a falsy value.]*/
    if (!jobId) throw new ReferenceError('The object \'jobId\' is not valid');
    if (!deviceIds) throw new ReferenceError('The object \'deviceIds\' is not valid');
    if (!propertyName) throw new ReferenceError('The object \'propertyName\' is not valid');

    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_007: [If the `deviceIds` argument is not an array, then `scheduleDevicePropertyWrite` shall convert it to an array with one element before using it. ]*/
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_008: [If the `propertyNames` argument is not an array, then `scheduleDevicePropertyWrite` shall convert it to an array with one element before using it. ]*/
    if (!Array.isArray(deviceIds)) deviceIds = [deviceIds];
    if (!Array.isArray(propertyName)) propertyName = [propertyName];

    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_006: [`scheduleDevicePropertyWrite` shall construct an HTTP request using information supplied by the caller, as follows:
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
    var writeDeviceProp = {
        "jobId": jobId,
        "jobParameters": {
            "DevicePropertyNames": propertyName,
            "DeviceIds": deviceIds,
            "jobType": "writeDeviceProperties"
        }
    };

    this._scheduleJob(writeDeviceProp, done);
};

/**
 * @method            module:azure-iothub.JobClient#scheduleReboot`
 * @description       Schedules a Job to reboot one or more device(s).
 * @param {Object}    jobId         An object which must include a `deviceId`
 *                                  property whose value is a valid device identifier.
 *                    deviceId      A string that indicates the device(s) that is/are being rebooted 
 * @param {Function}  done          function to call when the operation is
 *                                  complete. `done` will be called with three
 *                                  arguments: an Error object (can be null), an
 *                                  object representing the scheduled job, and a transport-specific response
 *                                  object useful for logging or debugging.
 */
JobClient.prototype.scheduleReboot = function (jobId, deviceIds, done) {
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_009: [`scheduleReboot` method shall throw a `ReferenceError` if any argument except 'done' contains a falsy value.]*/
    if (!jobId) throw new ReferenceError('The object \'jobId\' is not valid');
    if (!deviceIds) throw new ReferenceError('The object \'deviceIds\' is not valid');

    /*Codes_Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_011: [If the `deviceIds` argument is not an array, then `scheduleReboot` shall convert it to an array with one element before using it.]*/
    if (!Array.isArray(deviceIds)) deviceIds = [deviceIds];

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
    var rebootDevice = {
        "jobId": jobId,
        "jobParameters": {
            "DeviceIds": deviceIds,
            "jobType": "rebootDevice"
        }
    };

    this._scheduleJob(rebootDevice, done);
};


/**
 * @method            module:azure-iothub.JobClient#scheduleFactoryReset`
 * @description       Schedules a Job to factory-reset one or more device(s).
 * @param {Object}    jobId         An object which must include a `deviceId`
 *                                  property whose value is a valid device identifier.
 *                    deviceId      A string that indicates the device(s) that is/are being rebooted 
 * @param {Function}  done          function to call when the operation is
 *                                  complete. `done` will be called with three
 *                                  arguments: an Error object (can be null), an
 *                                  object representing the scheduled job, and a transport-specific response
 *                                  object useful for logging or debugging.
 */
JobClient.prototype.scheduleFactoryReset = function (jobId, deviceIds, done) {
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_012: [`scheduleFactoryReset` method shall throw a `ReferenceError` if any argument except 'done' contains a falsy value.]*/
    if (!jobId) throw new ReferenceError('The object \'jobId\' is not valid');
    if (!deviceIds) throw new ReferenceError('The object \'deviceIds\' is not valid');

    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_014: [If the `deviceIds` argument is not an array, then `scheduleFactoryReset` shall convert it to an array with one element before using it.]*/
    if (!Array.isArray(deviceIds)) deviceIds = [deviceIds];

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
    var factoryResetDevice = {
        "jobId": jobId,
        "jobParameters": {
            "DeviceIds": deviceIds,
            "jobType": "factoryResetDevice"
        }
    };

    this._scheduleJob(factoryResetDevice, done);
};

/**
 * @method            module:azure-iothub.JobClient#getJob`
 * @description       Gets the Job specified by an existing job identity on an IoT hub with
 *                    the given JobId information.
 * @param {Object}    jobId  An object which must include a `jobId`
 *                           property whose value is a valid job
 *                           identifier.
 * @param {Function}  done   The function to call when the operation is
 *                           complete. `done` will be called with three
 *                           arguments: an Error object (can be null), an
 *                           object representing the scheduled job, and a transport-specific response
 *                           object useful for logging or debugging.
 */
JobClient.prototype.getJob = function (jobId, done) {
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_012: [getJob shall throw an 'ReferenceError' if jobId is falsy.] */
    if (!jobId) {
        throw new ReferenceError('The object \'jobId\' is not valid');
    }

    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_013: [getJob shall construct an HTTPS request using information supplied by the caller, as follows:

    GET <config.host>/jobs/v2/<jobId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    UserAgent: <user-agent-string>
    Accept: application/json*/
    var config = this._config;
    var httpHeaders = {
        'Authorization': config.sharedAccessSignature,
        'Accept': 'application/json',
        'UserAgent': packageJson.name + '/' + packageJson.version
    };

    var path = '/jobs/v2/' + jobId + endpoint.versionQueryString();
    this._httpTransport.sendHttpRequest('GET', path, httpHeaders, config.host, null, function (err, body, response) {
        if (!done) return;

        if (!err) {
            var jobInfo = body ? JSON.parse(body) : null;
            done(null, jobInfo, response);
        }
        else {
            err.response = response;
            err.responseBody = body;
            done(err);
        }
    });
};

/**
 * @method            module:azure-iothub.JobClient#getJobs`
 * @description       Gets information for all jobs.
 * @param {Function}  done   The function to call when the operation is
 *                           complete. `done` will be called with three
 *                           arguments: an Error object (can be null), an
 *                           array of objects representing the job
 *                           identities, and a transport-specific response
 *                           object useful for logging or debugging.
 */
JobClient.prototype.getJobs = function getJobs (done) {
    var config = this._config;

    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_015: [`getJobs` shall construct an HTTPS request using information supplied by the caller, as follows:
    ```
    GET <config.host>/jobs/v2?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    UserAgent: <user-agent-string>
    Accept: application/json
    ```]*/

    var httpHeaders = {
        'Authorization': config.sharedAccessSignature,
        'Accept': 'application/json',
        'UserAgent': packageJson.name + '/' + packageJson.version
    };

    var path = '/jobs/v2' + endpoint.versionQueryString();
    this._httpTransport.sendHttpRequest('GET', path, httpHeaders, config.host, null, function (err, body, response) {
        if (!done) return;

        if (!err) {
            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_017: [`getJobs` shall call the `done` callback with a `null` error object and an array of jobs if the request succeeds.]*/
            var results = body ? JSON.parse(body) : null;
            done(null, results, response);
        }
        else {
            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_016: [`getJobs` shall call the `done` callback with an `Error` object if the request fails.]*/
            err.response = response;
            err.responseBody = body;
            done(err);
        }
    });
};

/**
 * @method            module:azure-iothub.JobClient#queryJobHistory
 * @description       Gets the jobs matching the specified query from the history.
 * 
 * @param {Object}    jobQuery  An object containing the query parameters.
 * @param {Function}  done   The function to call when the operation is
 *                           complete. `done` will be called with two
 *                           arguments: an Error object (can be null) and a
 *                           result object containing either an array of jobs
 *                           matching the query or an associative array containing
 *                           the aggregation results of the query.
 */
JobClient.prototype.queryJobHistory = function (jobQuery, done) {
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_001: [queryJobHistory shall throw a ReferenceError if the query parameter is falsy]*/
    if (!jobQuery) throw new ReferenceError('jobQuery cannot be \'' + jobQuery + '\'');

    var config = this._config;

    var httpHeaders = {
        'Authorization': config.sharedAccessSignature,
        'UserAgent': packageJson.name + '/' + packageJson.version,
        'Accept': 'application/json',
        'Content-Type': 'application/json; charset=utf-8',
    };

    var path = '/jobs/v2/query' + endpoint.versionQueryString();
    var queryContent = JSON.stringify(jobQuery);
    
    this._httpTransport.sendHttpRequest('POST', path, httpHeaders, config.host, queryContent, function (err, body, response) {
        if (!done) return;
        if (!err) {
            var bodyObject = JSON.parse(body);
            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_003: [queryJobHistory shall call the done callback with a null error object and an array of matching jobs if the query is a projection query.]*/
            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_004: [queryJobHistory shall call the done callback with a null error object and an associative array containing the results if the query is an aggregation query. ]*/
            var result = bodyObject.AggregateResult ? bodyObject.AggregateResult : bodyObject.Result;
            done(null, result, response);
        } else {
            /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_16_002: [queryJobHistory shall call the done callback with an `Error` object if the request fails.]*/
            err.response = response;
            err.responseBody = body;
            done(err);
        }
    });
};

module.exports = JobClient;