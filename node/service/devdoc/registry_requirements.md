#Azure-iothub.Registry Requirements

##Overview
Registry represents an IoT hub’s device identity service, known as the Registry.  A consumer can add, remove, update, or read device 
metadata from the Registry through the transport supplied by the caller (e.g., azure-iothub.Http—currently the Registry service only supports HTTP).

##Example Usage
```js
'use strict';
var Registry = require('azure-iothub').Registry;
var Device = require('azure-iothub').Device;

function print(err) {
  console.log(err.toString());
}

var connectionString = '[Connection string goes here]';
var registry = Registry.fromConnectionString(connectionString);

registry.create({deviceId: 'dev1'}, function (err, dev) {
  if (err) print(err);
  else {
    console.log(dev.deviceId);
    registry.get('dev1', function (err, dev) {
      if (err) print(err);
      else {
        console.log(dev.deviceId);
        registry.delete('dev1', function(err) {
          console.log('dev1 deleted');
        });
      }
    });
  }
});
```

## Public API
###Registry constructor
**SRS_NODE_IOTHUB_REGISTRY_05_001: [**The Registry constructor shall accept a transport object**]**, e.g. azure-iothub.Https;  

###fromConnectionString(value) [static]
The fromConnectionString static method returns a new instance of the Registry object using the default (HTTP) transport.  
**SRS_NODE_IOTHUB_REGISTRY_05_008: [**The fromConnectionString method shall throw ReferenceError if the value argument is falsy.**]**  
**SRS_NODE_IOTHUB_REGISTRY_05_009: [**Otherwise, it shall derive and transform the needed parts from the connection string in order to create a new instance of the default transport (azure-iothub.Http).**]**   
**SRS_NODE_IOTHUB_REGISTRY_05_010: [**The fromConnectionString method shall return a new instance of the Registry object, as by a call to new Registry(transport).**]**   

###fromSharedAccessSignature(value) [static]
**SRS_NODE_IOTHUB_REGISTRY_05_011: [**The fromSharedAccessSignature method shall throw ReferenceError if the value argument is falsy.**]**   
**SRS_NODE_IOTHUB_REGISTRY_05_012: [**Otherwise, it shall derive and transform the needed parts from the shared access signature in order to create a new instance of the default transport (azure-iothub.Http).**]**  
**SRS_NODE_IOTHUB_REGISTRY_05_013: [**The fromSharedAccessSignature method shall return a new instance of the Registry object, as by a call to new Registry(transport).**]** 

###create(deviceInfo, done)
The create method creates a device with the given device properties.  
**SRS_NODE_IOTHUB_REGISTRY_07_001: [**The create method shall throw ArgumentError if the first argument does not contain a deviceId property.**]**  
**SRS_NODE_IOTHUB_REGISTRY_07_002: [**When the create method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a Device object representing the new device identity returned from the IoT hub.**]**  
 
###update(deviceInfo, done)
The update method updates an existing device identity with the given device properties.  
**SRS_NODE_IOTHUB_REGISTRY_07_003: [**The update method shall throw ArgumentError if the first argument does not contain a deviceId property.**]**  
**SRS_NODE_IOTHUB_REGISTRY_07_004: [**When the update method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a Device object representing the new device identity returned from the IoT hub.**]** 

###get(deviceId, done)
The get method requests information about the device with the given ID.  
**SRS_NODE_IOTHUB_REGISTRY_05_006: [**The get method shall throw ReferenceError if the supplied deviceId is falsy.**]**  
**SRS_NODE_IOTHUB_REGISTRY_05_002: [**The get method shall request metadata for the device (indicated by the deviceId argument) from an IoT hub’s identity service via the transport associated with the Registry instance.**]**  
**SRS_NODE_IOTHUB_REGISTRY_05_003: [**When the get method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and a Device object representing the new device identity returned from the IoT hub.**]**

###list(done)
The list method requests information about the first 1000 devices registered in an IoT hub’s identity service.  
**SRS_NODE_IOTHUB_REGISTRY_05_004: [**The list method shall request information about devices from an IoT hub’s identity service via the transport associated with the Registry instance.**]**  
**SRS_NODE_IOTHUB_REGISTRY_07_006: [**The JSON array returned from the service shall be converted to a list of Device objects.**]**  
**SRS_NODE_IOTHUB_REGISTRY_07_005: [**When the list method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null), and an array of Device objects representing up to 1000 devices from the IoT hub.**]**  

###delete(deviceId, done)
The delete method removes a device with the given ID.  
**SRS_NODE_IOTHUB_REGISTRY_07_007: [**The delete method shall throw ReferenceError if the supplied deviceId is falsy.**]**  
**SRS_NODE_IOTHUB_REGISTRY_05_007: [**The delete method shall delete the given device from an IoT hub’s identity service via the transport associated with the Registry instance.**]**  
**SRS_NODE_IOTHUB_REGISTRY_05_005: [**When the delete method completes, the callback function (indicated by the done argument) shall be invoked with an Error object (may be null).**]**

###importDevicesFromBlob(inputBlobContainerUri, outputBlobContainerUri, done)
The importDevicesFromBlob imports a list of devices from a blob named devices.txt found at the input URI given as a parameter, and output logs from the import job in a blob at found at the output URI given as a parameter.  
**SRS_NODE_IOTHUB_REGISTRY_16_001: [**A ReferenceError shall be thrown if inputBlobContainerUri is falsy**]** 
**SRS_NODE_IOTHUB_REGISTRY_16_002: [**A ReferenceError shall be thrown if outputBlobContainerUri is falsy**]**  
**SRS_NODE_IOTHUB_REGISTRY_16_003: [**The ‘done’ callback shall be called with a null error parameter and the job status as a second parameter when the import job has been created**]** 
**SRS_NODE_IOTHUB_REGISTRY_16_008: [**The importDevicesFromBlob method should create a bulk import job using the transport associated with the Registry instance by giving it the correct URI path and an import request object such as:
{
	'type': 'import',
	'inputBlobContainerUri': <input container Uri given as parameter>,
	'outputBlobContainerUri': <output container Uri given as parameter>
}**]**  

###exportDevicesToBlob(outputBlobContainerUri, excludeKeys, done)
The exportDevicesToBlob exports a list of devices in a blob named devices.txt and logs from the export job at the output URI given as a parameter given as a parameter the export will contain security keys if the excludeKeys is false.  
**SRS_NODE_IOTHUB_REGISTRY_16_004: [**A ReferenceError shall be thrown if outputBlobContainerUri is falsy**]**  
**SRS_NODE_IOTHUB_REGISTRY_16_005: [**The ‘done’ callback shall be called with a null error parameter and the job status as a second parameter when the export job has been created**]** 
**SRS_NODE_IOTHUB_REGISTRY_16_009: [**The exportDevicesToBlob method should create a bulk export job using the transport associated with the Registry instance by giving it the correct path URI and export request object such as:
{
	'type': 'export',
	'outputBlobContainerUri': <output container Uri given as parameter>,
	'excludeKeysInExport': <excludeKeys Boolean given as parameter>
}**]**  

###listJobs(done)
The listJobs method will obtain a list of recent bulk import/export jobs (including the active one, if any).  
**SRS_NODE_IOTHUB_REGISTRY_16_016: [**The listJobs method should request a list of active and recent bulk import/export jobs using the transport associated with the Registry instance and give it the correct path URI.**]** 
**SRS_NODE_IOTHUB_REGISTRY_16_010: [**The ‘done’ callback shall be called with a null error parameter and list of recent jobs as a second parameter if the request is successful.**]** 
**SRS_NODE_IOTHUB_REGISTRY_16_011: [**The ‘done’ callback shall be called with only an error object if the request fails.**]** 

###getJob(jobId, done)
The getJob method will obtain status information of the bulk import/export job identified by the jobId parameter.  
**SRS_NODE_IOTHUB_REGISTRY_16_017: [**The getJob method should request status information of the bulk import/export job identified by the jobId parameter using the transport associated with the Registry instance and give it the correct path URI.**]**   
**SRS_NODE_IOTHUB_REGISTRY_16_006: [**A ReferenceError shall be thrown if jobId is falsy**]**  
**SRS_NODE_IOTHUB_REGISTRY_16_015: [**The ‘done’ callback shall be called with only an error object if the request fails.**]**  
**SRS_NODE_IOTHUB_REGISTRY_16_007: [**The ‘done’ callback shall be called with a null error parameter and the job status as second parameter if the request is successful.**]**  

###cancelJob(jobId, done)
The cancelJob method will cancel the bulk import/export job identified by the jobId parameter.  
**SRS_NODE_IOTHUB_REGISTRY_16_018: [**The cancelJob method should request cancel the job identified by the jobId parameter using the transport associated with the Registry instance by giving it the correct path URI.**]**  
**SRS_NODE_IOTHUB_REGISTRY_16_012: [**A ReferenceError shall be thrown if the jobId is falsy**]**  
**SRS_NODE_IOTHUB_REGISTRY_16_013: [**The ‘done’ callback shall be called with only an error object if the request fails.**]**  
**SRS_NODE_IOTHUB_REGISTRY_16_014: [**The ‘done’ callback shall be called with no parameters if the request is successful.**]**  

###queryDevicesByTag(tags, maxCount, done)
The `queryDevicesByTag` method will return a list of devices that match the tags that are specified in the `tags` array.  
**SRS_NODE_IOTHUB_REGISTRY_07_019: [**A `ReferenceError` shall be thrown if the tags array is empty.**]**  
**SRS_NODE_IOTHUB_REGISTRY_07_020: [**The `queryDevicesByTag` method shall call the server for the devices that contain the tags**]**  
**SRS_NODE_IOTHUB_REGISTRY_07_021: [**When the list method completes, the callback function (indicated by the done argument) shall be invoked with an `Error` object (may be null), and an array of `Device` objects representing up to `maxCount` devices from the IoT hub.**]**  
**SRS_NODE_IOTHUB_REGISTRY_07_022: [**The JSON array returned from the service shall be converted to a list of `Device` objects.**]**
**SRS_NODE_IOTHUB_REGISTRY_07_023: [**A `ReferenceError` shall be thrown if the `maxCount` parameter is less than or equal to zero.**]**  

###queryDevices(query, done)
The `queryDevices` method will return a list of devices that match the query parameters specified in the `query` object.
**SRS_NODE_IOTHUB_REGISTRY_16_019: [** A `ReferenceError` shall be thrown if the query object is falsy or empty. **]**
**SRS_NODE_IOTHUB_REGISTRY_16_020: [** The `done` callback shall be called with an `Error` object if the request fails. **]**
**SRS_NODE_IOTHUB_REGISTRY_16_021: [** The `done` callback shall be called with a null object for first parameter and the result object as a second parameter that is a simple array of `Device` objects corresponding to the devices matching the query if it uses projection. **]**
**SRS_NODE_IOTHUB_REGISTRY_16_022: [** The `done` callback shall be called with a null object for first parameter and the result object as a second parameter that is an associative array (dictionary) of results if the query was an aggregation query. **]**