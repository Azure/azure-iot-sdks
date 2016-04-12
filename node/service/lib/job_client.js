// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var anHourFromNow = require('azure-iot-common').anHourFromNow;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var ConnectionString = require('./connection_string.js');
var JobResponse = require('./job_response.js');
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
    return new JobClient(config, new UseTransport() );
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
    return new JobClient(config, new UseTransport() );
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
 *                                  arguments: an Error object (can be null), a
 *                                  {@link module:azure-iothub.JobResponse|JobResponse}
 *                                  object representing the updated device
 *                                  identity, and a transport-specific response
 *                                  object useful for logging or debugging.
 */
JobClient.prototype.scheduleFirmwareUpdate = function (jobId, deviceIds, packageUri, timeout, done) {
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_008: [scheduleFirmwareUpdate method shall throw ArgumentError if any argument contains a falsy value.] */
    if (!jobId) throw new ArgumentError('The object \'jobId\' is not valid');
    if (!deviceIds) throw new ArgumentError('The object \'deviceIds\' is not valid');
    if (!packageUri) throw new ArgumentError('The object \'packageUri\' is not valid');
    if (!timeout) throw new ArgumentError('The object \'timeout\' is not valid');

    var config = this._config;
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_009: [scheduleFirmwareUpdate shall construct an HTTP request using information supplied by the caller, as follows:
    PUT <path>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    Content-Type: application/json; charset=utf-8
    'Accept': 'application/json',
    [JobInfo]
    ]*/
    var httpHeaders = {
        'Authorization': config.sharedAccessSignature,
        'UserAgent': packageJson.name + '/' + packageJson.version,
        'Accept': 'application/json',
        'Content-Type': 'application/json; charset=utf-8',
    };
    
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_014: [ If the deviceIds argument is not an array, then getJob shall convert it to an array with one element before using it.] */
    if (!Array.isArray(deviceIds)) deviceIds = [deviceIds];
        
    var firmwareUpdateJob = {
        "jobId": jobId,
        "jobParameters":{
            "DeviceIds": deviceIds,
            "PackageUri": packageUri,
            "Timeout": '00:' + timeout + ':00', // Will be replace with the Timespan object after some research
            "jobType": "firmwareUpdate"
        } 
    };
        
    var path = '/jobs/v2/'+ jobId + endpoint.versionQueryString();
    this._httpTransport.sendHttpRequest('PUT', path, httpHeaders, config.host, JSON.stringify(firmwareUpdateJob), function (err, body, response) {
        if (!err) {
            /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_011: [When the scheduleFirmwareUpdate method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a JobResponse object representing the new job identity returned from the IoT hub.]*/
            var jobInfo;
            if (body) {
                jobInfo = new JobResponse(body);
            }
            done(null, jobInfo, response);
        }
        else {
            /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_010: [If any scheduleFirmwareUpdate operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
            err.response = response;
            err.responseBody = body;
            done(err);
        }
    });
};

JobClient.prototype.scheduleDeviceConfigurationUpdate = function(jobId, deviceIds, value, done) {
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_001: [ `scheduleDeviceConfigurationUpdate` shall throw `ReferenceError` if either of the `jobId` or `deviceIds` arguments are falsy. ]*/
    if (!jobId) throw new ReferenceError('Argument \'jobId\' is ' + jobId);
    if (!deviceIds) throw new ReferenceError('Argument \'deviceId\' is ' + deviceIds);

    var headers = {
        'Authorization': this._config.sharedAccessSignature,
        'UserAgent': packageJson.name + '/' + packageJson.version,
        'Accept': 'application/json',
        'Content-Type': 'application/json; charset=utf-8',
    };
    
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_005: [ If the `deviceIds` argument is not an array, then `scheduleDeviceConfigurationUpdate` shall convert it to an array with one element before using it. ]*/
    if (!Array.isArray(deviceIds)) deviceIds = [deviceIds];
    
    var deviceConfigurationUpdateJob = {
        jobId: jobId,
        jobParameters: {
            Value: value,
            DeviceIds: deviceIds,
            jobType: 'updateDeviceConfiguration'
        }
    };

    var path = '/jobs/v2/' + jobId + endpoint.versionQueryString();
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_002: [ `scheduleDeviceConfigurationUpdate` shall construct an HTTPS request using information supplied by the caller, as follows:  
    PUT <config.host>/jobs/v2/<jobId>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    UserAgent: <user-agent-string>
    Accept: application/json
    Content-Type: application/json; charset=utf-8
    Host: <config.host>

    {"jobId":"<jobId>","jobParameters":{"Value":"<value>","DeviceIds":<deviceIds>,"jobType":"updateDeviceConfiguration"}}]*/
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_003: [ If `scheduleDeviceConfigurationUpdate` encounters an error before it can send the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (`err.message`). ]*/
    /*Codes_SRS_NODE_IOTHUB_JOBCLIENT_05_004: [ When `scheduleDeviceConfigurationUpdate` completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and a `JobResponse` object representing the new job created on the IoT Hub. ]*/
    this._httpTransport.sendHttpRequest('PUT', path, headers, this._config.host, JSON.stringify(deviceConfigurationUpdateJob), function (err, body, response) {
        if (!done) return;
            
        if (err) {
            err.response = response;
            err.responseBody = body;
            done(err);
        } else {
            var jobInfo = body ? new JobResponse(body) : null;
            done(null, jobInfo, response);
        }
    });
};

/**
 * @method            module:azure-iothub.JobClient#ScheduleSystemPropertyRead`
 * @description       Schedules a Job to update the System defined Device Property value.
 * @param {Object}    jobId         An object which must include a `deviceId`
 *                                  property whose value is a valid device identifier.
 *                    deviceId      A string that indicates the device that is being Updated 
 *                    propertyName  The Property to Read
 * @param {Function}  done          function to call when the operation is
 *                                  complete. `done` will be called with three
 *                                  arguments: an Error object (can be null), a
 *                                  {@link module:azure-iothub.JobResponse|JobResponse}
 *                                  object representing the updated device
 *                                  identity, and a transport-specific response
 *                                  object useful for logging or debugging.
 */
JobClient.prototype.scheduleSystemPropertyRead = function (jobId, deviceIds, propertyName, done) {
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_015: [ ScheduleSystemPropertyRead method shall throw ArgumentError if any argument contains a falsy value.] */
    if (!jobId) throw new ReferenceError('The object \'jobId\' is not valid');
    if (!deviceIds) throw new ReferenceError('The object \'deviceIds\' is not valid');
    if (!propertyName) throw new ReferenceError('The object \'propertyName\' is not valid');

    var config = this._config;
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_016: [ ScheduleSystemPropertyRead shall construct an HTTP request using information supplied by the caller, as follows:
    PUT <path>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    UserAgent: packageJson.name + '/' + packageJson.version
    Content-Type: application/json; charset=utf-8
    'Accept': 'application/json',
    {
        "jobId":"<jobId>",
        "jobParameters":{
            "SystemPropertyNames":"<propertyName>",
            "DeviceIds":<deviceIds>,
            "jobType":"readDeviceProperties"
        }
    }
    ]*/
    var httpHeaders = {
        'Authorization': config.sharedAccessSignature,
        'UserAgent': packageJson.name + '/' + packageJson.version,
        'Accept': 'application/json',
        'Content-Type': 'application/json; charset=utf-8',
    };
    
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_017: [ If the deviceIds argument is not an array, then getJob shall convert it to an array with one element before using it.] */
    /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_018: [ If the sysPropNames argument is not an array, then getJob shall convert it to an array with one element before using it.] */
    if (!Array.isArray(deviceIds)) deviceIds = [deviceIds];
    if (!Array.isArray(propertyName)) propertyName = [propertyName];
        
    var sysPropUpdate = {
        "jobId": jobId,
        "jobParameters":{
            "SystemPropertyNames": propertyName,
            "DeviceIds": deviceIds,
            "jobType": "readDeviceProperties"
        } 
    };

    var path = '/jobs/v2/'+ jobId + endpoint.versionQueryString();
    this._httpTransport.sendHttpRequest('PUT', path, httpHeaders, config.host, JSON.stringify(sysPropUpdate), function (err, body, response) {
        if (!err) {
            /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_019: [ When the ScheduleSystemPropertyRead method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a JobResponse object representing the new job identity returned from the IoT hub.] */
            var jobInfo;
            if (body) {
                jobInfo = new JobResponse(body);
            }
            done(null, jobInfo, response);
        }
        else {
            /* Codes_SRS_NODE_IOTHUB_JOBCLIENT_07_020: [ If any ScheduleSystemPropertyRead operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).] */
            err.response = response;
            err.responseBody = body;
            done(err);
        }
    });
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
 *                           arguments: an Error object (can be null), a
 *                           {@link module:azure-iothub.JobResponse|JobResponse}
 *                           object representing the updated Job
 *                           identity, and a transport-specific response
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

    var path = '/jobs/v2/'+ jobId + endpoint.versionQueryString();
    this._httpTransport.sendHttpRequest('GET', path, httpHeaders, config.host, null, function (err, body, response) {
        if (!done) return;
        
        if (!err) {
            var jobInfo;
            if (body) {
            jobInfo = new JobResponse(body);
            }
            done(null, jobInfo, response);
        }
        else {
            err.response = response;
            err.responseBody = body;
            done(err);
        }
    });
};

module.exports = JobClient;