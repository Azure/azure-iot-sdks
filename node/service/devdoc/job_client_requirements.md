#Azure-iothub.JobClient Requirements

##Overview
The JobClient object allows you to initiate device management jobs on an IoT Hub and query their status.

##Example Usage
```js
'use strict';
var JobClient = require('azure-iothub').JobClient;
var Device = require('azure-iothub').Device;

var connectionString = '[Connection string goes here]';
var jobClient = JobClient.fromConnectionString(connectionString);

var timeout = 25;
jobClient.scheduleFirmwareUpdate(jobId, 'nodeDevice1', 'http://www.bing.com/', timeout, function (err, jobResp, response) {
   if (err) {
      console.log('Schedule Firmware Update Failure: ' + err);
   }
   else {
       console.log(jobResp);
       queryJobInfo(jobResp.jobId)
   }
});
```
###JobClient constructor
**SRS_NODE_IOTHUB_JOBCLIENT_07_001: [**The JobClient constructor shall accept a config object
host – (string) the fully-qualified DNS hostname of an IoT hub
hubName – (string) the name of the IoT hub, which is the first segment of hostname
sharedAccessSignature – (string) a shared access signature generated from the credentials of a policy with the appropriate registry permissions (read and/or write).
**]**  

###fromConnectionString(value) [static]
The `fromConnectionString` static method returns a new instance of the JobClient object.  
**SRS_NODE_IOTHUB_JOBCLIENT_07_002: [**The fromConnectionString method shall throw ReferenceError if the value argument is falsy.**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_003: [**Otherwise, it shall derive and transform the needed parts from the connection string in order to create a valid Config object to pass to the constructor.**]**     
**SRS_NODE_IOTHUB_JOBCLIENT_07_004: [**The fromConnectionString method shall return a new instance of the JobClient object, as by a call to new JobClient(config).**]**     

###fromSharedAccessSignature(value) [static]
The `fromSharedAccessSignature` static method returns a new instance of the JobClient object.  
**SRS_NODE_IOTHUB_JOBCLIENT_07_005: [**The fromSharedAccessSignature method shall throw ReferenceError if the value argument is falsy.**]**   
**SRS_NODE_IOTHUB_JOBCLIENT_07_006: [**Otherwise, it shall derive and transform the needed parts from the shared access signature in order to create a valid Config object to pass to the constructor.**]**    
**SRS_NODE_IOTHUB_JOBCLIENT_07_007: [**The fromSharedAccessSignature method shall return a new instance of the JobClient object, as by a call to new JobClient(config).**]**  

###scheduleFirmwareUpdate(jobId, deviceId, packageUri, timeout, done)
The `scheduleFirmwareUpdate` method initiates a Firmware Update returning a jobResponse Object.  
**SRS_NODE_IOTHUB_JOBCLIENT_07_008: [**`scheduleFirmwareUpdate` method shall throw ArgumentError if any argument contains a falsy value excluding the done parameter.**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_009: [**`scheduleFirmwareUpdate` shall construct an HTTP request using information supplied by the caller, as follows:
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
```
**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_010: [**If any `scheduleFirmwareUpdate` operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).**]**    
**SRS_NODE_IOTHUB_JOBCLIENT_07_011: [**When the `scheduleFirmwareUpdate` method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a JobResponse object representing the new job identity returned from the IoT hub.**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_014: [** If the `deviceIds` argument is not an array, then `scheduleFirmwareUpdate` shall convert it to an array with one element before using it.**]**  

###scheduleDeviceConfigurationUpdate(jobId, deviceIds, value, done)
The `scheduleDeviceConfigurationUpdate` method initiates an update of the 'Value' resource on the 'Config' object for all devices named by the `deviceIds` array.  
**SRS_NODE_IOTHUB_JOBCLIENT_05_001: [** `scheduleDeviceConfigurationUpdate` shall throw `ReferenceError` if either of the `jobId` or `deviceIds` arguments are falsy. **]**
**SRS_NODE_IOTHUB_JOBCLIENT_05_005: [** If the `deviceIds` argument is not an array, then `scheduleDeviceConfigurationUpdate` shall convert it to an array with one element before using it. **]**  
**SRS_NODE_IOTHUB_JOBCLIENT_05_002: [** `scheduleDeviceConfigurationUpdate` shall construct an HTTPS request using information supplied by the caller, as follows:  
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
```
**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_05_003: [** If `scheduleDeviceConfigurationUpdate` encounters an error before it can send the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (`err.message`). **]**  
**SRS_NODE_IOTHUB_JOBCLIENT_05_004: [** When `scheduleDeviceConfigurationUpdate` completes, the callback function (indicated by the `done` argument) shall be invoked with an `Error` object (may be `null`), and a `JobResponse` object representing the new job created on the IoT Hub. **]**  

###ScheduleSystemPropertyRead(jobId, deviceIds, propertyNames, done)
**SRS_NODE_IOTHUB_JOBCLIENT_07_015: [** ScheduleSystemPropertyRead method shall throw ArgumentError if any argument except 'done' contains a falsy value.**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_016: [** ScheduleSystemPropertyRead shall construct an HTTP request using information supplied by the caller, as follows:
```
    PUT <path>?api-version=<version> HTTP/1.1
    Authorization: <config.sharedAccessSignature>
    UserAgent: packageJson.name + '/' + packageJson.version
    Content-Type: application/json; charset=utf-8
    'Accept': 'application/json',
    {
        "jobId":"<jobId>",
        "jobParameters":{
            "SystemPropertyNames":"<propertyNames>",
            "DeviceIds":<deviceIds>,
            "jobType":"readDeviceProperties"
        }
    }
```
**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_017: [** If the deviceIds argument is not an array, then getJob shall convert it to an array with one element before using it.**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_018: [** If the propertyNames argument is not an array, then getJob shall convert it to an array with one element before using it.**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_019: [** When the ScheduleSystemPropertyRead method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a JobResponse object representing the new job identity returned from the IoT hub.**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_020: [** If any ScheduleSystemPropertyRead operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).**]**  

###getJob(jobId, done)
The `getJob` method retrieves the job specified in jobId
**SRS_NODE_IOTHUB_JOBCLIENT_07_012: [**`getJob` shall throw an `ReferenceError` if jobId is falsy.**]**  
**SRS_NODE_IOTHUB_JOBCLIENT_07_013: [**`getJob` shall construct an HTTPS request using information supplied by the caller, as follows:    
```
GET <config.host>/jobs/v2/<jobId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
UserAgent: <user-agent-string>
Accept: application/json
```
**]**  

###queryJobHistory(query, done)
the `queryJobHistory` method retrieves an array of jobs or the result of an aggregation operation on jobs from the history that match the specified filter.

**SRS_NODE_IOTHUB_JOBCLIENT_16_001: [** queryJobHistory shall throw a ReferenceError if the query parameter is falsy **]**
**SRS_NODE_IOTHUB_JOBCLIENT_16_002: [** queryJobHistory shall call the done callback with an `Error` object if the request fails. **]**
**SRS_NODE_IOTHUB_JOBCLIENT_16_003: [** queryJobHistory shall call the done callback with a null error object and an array of matching jobs if the query is a projection query. **]**
**SRS_NODE_IOTHUB_JOBCLIENT_16_004: [** queryJobHistory shall call the done callback with a null error object and an associative array containing the results if the query is an aggregation query.  **]**
