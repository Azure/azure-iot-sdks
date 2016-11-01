#JobClient requirements

## Overview
The `JobClient` class provides the ability to schedule long-running jobs on the IoT Hub instance.

## Usage
```js
'use strict';

var uuid = require('uuid');
var JobClient = require('azure-iothub').JobClient;

var jobClient = JobClient.fromConnectionString('<IoTHub Connection String>');

var jobId = uuid.v4();
var startTime = new Date(Date.now() + 3600000); // an hour from now
var maxExecutionTimeInSeconds =  3600;
var methodParams = {
  methodName: '<method name>',
  payload: null,
  timeoutInSeconds: 42
};
jobClient.scheduleDeviceMethod(jobId, // Job Unique Identifier
                               'SELECT * FROM devices WHERE tags.building == 43', // Query that will be run by the job to determine which devices to run on
                               methodParams, // method timeout (method execution will be considered a failure after that many seconds)
                               startTime, // The time at which the job should startTime
                               maxRunTime, // The maximum running time for the job.
                               function(err, result) {
  if (err) {
    console.error('Could not schedule job: ' + err.message);
  } else {
    var jobMonitorInterval = setInterval(function() {
      jobClient.getJob(jobId, function(err, result) {
        if (err) {
          console.error('Could not get job status: ' + err.message);
        } else {
          console.log('Job status: ' + result.status);
          if (result.status === 'completed' || result.status === 'failed' || result.status === 'cancelled') {
            clearInterval(jobMonitorInterval);
          }
        }
      })
    }, 5000);
  }
});
```

## Public interface

### JobClient(restApiClient)
**SRS_NODE_JOB_CLIENT_16_001: [** The `JobClient` constructor shall throw a `ReferenceError` if `restApiClient` is falsy. **]**

### fromConnectionString(connectionString) [static]
**SRS_NODE_JOB_CLIENT_16_002: [** The `fromConnectionString` method shall throw a `ReferenceError` if `connectionString` is falsy. **]**

**SRS_NODE_JOB_CLIENT_16_003: [** The `fromConnectionString` method shall return a new `JobClient` instance. **]**

### fromSharedAccessSignature(sharedAccessSignature) [static]
**SRS_NODE_JOB_CLIENT_16_004: [** The `fromSharedAccessSignature` method shall throw a `ReferenceError` if `sharedAccessSignature` is falsy. **]**

**SRS_NODE_JOB_CLIENT_16_005: [** The `fromSharedAccessSignature` method shall return a new `JobClient` instance. **]**


### getJob(jobId, done)
The `getJob` method calls the `done` callback with an object containing the status of the job identified by the `jobid` argument.

**SRS_NODE_JOB_CLIENT_16_006: [** The `getJob` method shall throw a `ReferenceError` if `jobId` is `null`, `undefined` or an empty string. **]**

**SRS_NODE_JOB_CLIENT_16_007: [** The `getJob` method shall construct the HTTP request as follows:
```
GET /jobs/v2/<jobId>?api-version=<version>
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
User-Agent: <sdk-name>/<sdk-version>
```
**]**

### cancelJob(jobId, done)
The `cancelJob` method cancels the job identified by the `jobId` argument.

**SRS_NODE_JOB_CLIENT_16_008: [** The `cancelJob` method shall throw a `ReferenceError` if `jobId` is `null`, `undefined` or an empty string. **]**

**SRS_NODE_JOB_CLIENT_16_009: [** The `cancelJob` method shall construct the HTTP request as follows:
```
POST /jobs/v2/<jobId>/cancel?api-version=<version>
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
User-Agent: <sdk-name>/<sdk-version>
```
**]**

### createQuery(jobType, jobStatus, pageSize)
The `createQuery` method creates a query that can be used to find all jobs matching `jobType` and `jobStatus` and get them in a pages of a specified size.

**SRS_NODE_JOB_CLIENT_16_032: [** The `createQuery` method shall throw a `TypeError` if the `jobType` and `jobStatus` arguments are not `null`, `undefined` or of type `string`. **]**

**SRS_NODE_JOB_CLIENT_16_033: [** The `createQuery` method shall throw a `TypeError` if the `pageSize` argument is not `null`, `undefined` or a number. **]**

**SRS_NODE_JOB_CLIENT_16_034: [** The `createQuery` method shall return a new `Query` instance. **]**

### _getJobsFunc(jobType, jobStatus, pageSize)
The `_getJobsFunc` method gets a new page of jobs matching `jobType` and `jobStatus` if specified and calls the `done` callback with that list.

**SRS_NODE_JOB_CLIENT_16_012: [** The `_getJobsFunc` method shall construct the HTTP request as follows:
```
GET /jobs/v2/query?api-version=<version>[&jobType=<jobType>][&jobStatus=<jobStatus>][&pageSize=<pageSize>][&continuationToken=<continuationToken>]
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
User-Agent: <sdk-name>/<sdk-version>
```
**]**

**SRS_NODE_JOB_CLIENT_16_035: [** The `_getJobsFunc` function shall return a function that can be used by the `Query` object to get a new page of results **]**

### scheduleDeviceMethod(jobId, queryCondition, methodParams, jobStartTime, maxExecutionTimeInSeconds, done)
The `scheduleDeviceMethod` method schedules a device method call on a list of devices at a specific time.

**SRS_NODE_JOB_CLIENT_16_013: [** The `scheduleDeviceMethod` method shall throw a `ReferenceError` if `jobId` is `null`, `undefined` or an empty string. **]**

**SRS_NODE_JOB_CLIENT_16_014: [** The `scheduleDeviceMethod` method shall throw a `ReferenceError` if `queryCondition` is falsy. **]**

**SRS_NODE_JOB_CLIENT_16_029: [** The `scheduleDeviceMethod` method shall throw a `ReferenceError` if `methodParams` is falsy. **]** 

**SRS_NODE_JOB_CLIENT_16_015: [** The `scheduleDeviceMethod` method shall throw a `ReferenceError` if `methodParams.methodName` is `null`, `undefined` or an empty string. **]**

**SRS_NODE_JOB_CLIENT_16_030: [** The `scheduleDeviceMethod` method shall use the `DeviceMethod.defaultPayload` value if `methodParams.payload` is `undefined`. **]**

**SRS_NODE_JOB_CLIENT_16_031: [** The `scheduleDeviceMethod` method shall use the `DeviceMethod.defaultTimeout` value if `methodParams.timeoutInSeconds` is falsy. **]**

**SRS_NODE_JOB_CLIENT_16_018: [** If `jobStartTime` is a function, `jobStartTime` shall be considered the callback and a `TypeError` shall be thrown if `maxExecutionTimeInSeconds` and/or `done` are not `undefined`. **]**

**SRS_NODE_JOB_CLIENT_16_019: [** If `maxExecutionTimeInSeconds` is a function, `maxExecutionTimeInSeconds` shall be considered the callback and a `TypeError` shall be thrown if `done` is not `undefined`. **]**

**SRS_NODE_JOB_CLIENT_16_020: [** The `scheduleDeviceMethod` method shall construct the HTTP request as follows:
```
PUT /jobs/v2/<jobId>?api-version=<version>
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
User-Agent: <sdk-name>/<sdk-version>

{
  jobId: '<jobId>',
  type: 'scheduleDirectRequest',
  cloudToDeviceMethod: <methodParams>,
  queryCondition: '<queryCondition>',   // if the queryCondition parameter is a string
  startTime: <jobStartTime>,            // as an ISO-8601 date string
  maxExecutionTimeInSeconds: <maxExecutionTimeInSeconds>  // Number of seconds
}
```
**]**

### scheduleTwinUpdate(jobId, queryCondition, patch, jobStartTime, maxExecutionTimeInSeconds, done)
The `scheduleTwinUpdate` method schedules an update of the device twin on a list of devices at a specific time.

**SRS_NODE_JOB_CLIENT_16_021: [** The `scheduleTwinUpdate` method shall throw a `ReferenceError` if `jobId` is `null`, `undefined` or an empty string. **]**

**SRS_NODE_JOB_CLIENT_16_022: [** The `scheduleTwinUpdate` method shall throw a `ReferenceError` if `queryCondition` is falsy. **]**

**SRS_NODE_JOB_CLIENT_16_023: [** The `scheduleTwinUpdate` method shall throw a `ReferenceError` if `patch` is falsy. **]**

**SRS_NODE_JOB_CLIENT_16_024: [** If `jobStartTime` is a function, `jobStartTime` shall be considered the callback and a `TypeError` shall be thrown if `maxExecutionTimeInSeconds` and/or `done` are not `undefined`. **]**

**SRS_NODE_JOB_CLIENT_16_025: [** If `maxExecutionTimeInSeconds` is a function, `maxExecutionTimeInSeconds` shall be considered the callback and a `TypeError` shall be thrown if `done` is not `undefined`. **]**

**SRS_NODE_JOB_CLIENT_16_026: [** The `scheduleTwinUpdate` method shall construct the HTTP request as follows:
```
PUT /jobs/v2/<jobId>?api-version=<version>
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
User-Agent: <sdk-name>/<sdk-version>

{
  jobId: '<jobId>',
  type: 'scheduleTwinUpdate',
  updateTwin: <patch>                   // Valid JSON object
  queryCondition: '<queryCondition>',   // if the queryCondition parameter is a string
  startTime: <jobStartTime>,            // as an ISO-8601 date string
  maxExecutionTimeInSeconds: <maxExecutionTimeInSeconds>  // Number of seconds
}
```
**]**

## All methods with callbacks

**SRS_NODE_JOB_CLIENT_16_027: [** The method shall call the `done` callback with a single argument that is a standard Javascript `Error` object if the request failed. **]**

**SRS_NODE_JOB_CLIENT_16_028: [** The method shall call the `done` callback with a `null` error argument, a result and a transport-specific response object if the request was successful. **]**
