# DeviceTwin requirements

## Overview
The `DeviceTwin` class provides methods to interact with a device's twin object stored in Azure IoTHub.

## Example usage: Get and update a device twin
```javascript
'use strict';

var Registry = require('azure-iothub').Registry;

var connectionString = '[Connection string goes here]';
var registry = Registry.fromConnectionString(connectionString);

registry.getDeviceTwin('deviceId', function(err, twin) {
  if (err) {
    console.error(err.constructor.name + ': ' + err.message);
  } else {
    var patch = {
      tags: {
        building: 43
      },
      properties: {
        desired: {
          climateControl: {
            minTemperature: 68;
            maxTemperature: 76;
          }
        }
      }
    };

    // This will fail if the twin was updated by another service between the time of the `get` and the `update` operation
    twin.update(patch, function(err) {
      if (err) {
        console.error('Could not update twin: ' + err.constructor.name + ': ' + err.message);
      } else {
        console.log(twin.deviceId + ' twin updated successfully');
      }
    });
    
    // This will succeed even if the twin was updated by another service between the time of the `get` and the `update` operation
    twin.update(patch, true, function(err) {
      if (err) {
        console.error('Could not update twin: ' + err.constructor.name + ': ' + err.message);
      } else {
        console.log(twin.deviceId + ' twin updated successfully');
      }
    });
  }
});
```

## Example usage: Create a patch for a specific device and force-push an update to the twin without having to get it first (advanced case)
```javascript
'use strict';

var Registry = require('azure-iothub').Registry;
var connectionString = '[Connection string goes here]';
var registry = Registry.fromConnectionString(connectionString);

var twin = new DeviceTwin('deviceId', registry);
var propPatch = {
    climateControl: {
      minTemperature: 68;
      maxTemperature: 76;
    }
};

// When the twin is manually created and not retrieved from the IoT Hub, the second parameter (force) must be set to true because we have no valid twin etag. 
twin.properties.desired.update(propPatch, true, function (err) {
  if (err) {
    console.error(err.constructor.name + ': ' + err.message);
  } else {
    console.log('Twin desired properties forcefully updated');
  }
});
```

## Constructors
In the majority of cases, users are better of creating `DeviceTwin` objects using the `Registry.getDeviceTwin()` API. This will not only return a new DeviceTwin instance but will 
populate it with the latest state from the Device Registry, including the etag that is used to protect a twin against multiple concurrent updates.

In advanced cases where benefitting from optimistic concurrency is not desired or desirable, the `DeviceTwin` constructors allow the users to create an empty `DeviceTwin` object initialized with only the arguments passed to it. It will also set the etag to `*`
so that if a user calls `update` or `replace` on `tags`, `properties.desired` or the `DeviceTwin` object itself, the call succeeds. This allows updating/replacing parts of the twin without
having to get it first which can be useful in some high performance scenarios.

### DeviceTwin(device, registryClient)
**SRS_NODE_IOTHUB_DEVICETWIN_16_001: [** The `DeviceTwin(device, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the `deviceId` base property to the `device` argument if it is a `string`. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_006: [** The `DeviceTwin(device, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the properties of the created object to the properties described in the `device` argument if it's an `object`. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_002: [** The `DeviceTwin(device, registryClient)` constructor shall throw a `ReferenceError` if `device` is undefined, null or an empty string. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_003: [** The `DeviceTwin(device, registryClient)` constructor shall throw a `ReferenceError` if `registryClient` is falsy. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_007: [** The `DeviceTwin(device, registryClient)` constructor shall throw an `ArgumentError` if `device` is an object and does not have a `deviceId` property. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_005: [** The `DeviceTwin(device, registryClient)` constructor shall set the `DeviceTwin.etag` to `*`. **]**

## DeviceTwin.tags
### replace(newTags, force, done)
The `replace` method is used to replace the existing tag dictionary with the one passed as a parameter. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.etag` field if defined. If not defined, `*` will be used.

**SRS_NODE_IOTHUB_DEVICETWIN_16_009: [** The `replace` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT /twins/<DeviceTwin.deviceId>/tags?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.etags> | *

<tags object>
```
**]**

## DeviceTwin.properties.desired
### replace(newProps, force, done)
The `replace` method is used to replace the existing desired properties dictionary with the one passed as a parameter. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.etag` field if defined. If not defined, `*` will be used.

**SRS_NODE_IOTHUB_DEVICETWIN_16_016: [** The `replace` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT /twins/<DeviceTwin.deviceId>/properties/desired?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.etag> | *

<desired properties object>
```
**]**

### update(patch, force, done)
The `update` method is used to update the existing desired properties dictionary with the values passed as a patch argument. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.etag` field if defined. If not defined, `*` will be used.

**SRS_NODE_IOTHUB_DEVICETWIN_16_018: [** The `update` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PATCH /twins/<DeviceTwin.deviceId>/properties/desired?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.etag> | *

<desired properties object>
```
**]**

## Methods
### get(done)
The `get` method is used to refresh the `DeviceTwin` instance with the latest values from the IoT Hub Registry.

**SRS_NODE_IOTHUB_DEVICETWIN_16_020: [** The `get` method shall call the `getDeviceTwin` method of the `Registry` instance stored in `_registry` property with the following parameters:
- `this.deviceId`
- `done`
**]**

### update(patch, done)
The `update` method is used to update any part of the device twin stored in the IoT Hub registry with the values passed as a patch argument. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.eTag` field if defined. If not defined, `*` will be used.

**SRS_NODE_IOTHUB_DEVICETWIN_16_019: [** The `update` method shall call the `updateDeviceTwin` method of the `Registry` instance stored in `_registry` property with the following parameters:
- `this.deviceId`
- `patch`
- `this.etag`
- `done`
**]**

### All twin methods calling to a Registry API
**SRS_NODE_IOTHUB_DEVICETWIN_16_021: [** The method shall merge the twin returned in the callback of the `Registry` method call with its parent object. **]**  
**SRS_NODE_IOTHUB_DEVICETWIN_16_022: [** The method shall call the `done` callback with an `Error` object if the request failed **]**  
**SRS_NODE_IOTHUB_DEVICETWIN_16_023: [** The method shall call the `done` callback with a `null` error object, its parent instance as a second argument and the transport `response` object as a third argument if the request succeeded. **]**  

### toJSON()
The `toJSON` method is called when calling `JSON.stringify()` on a `DeviceTwin` object.

**SRS_NODE_IOTHUB_DEVICETWIN_16_015: [** The `toJSON` method shall return a copy of the `DeviceTwin` object that doesn't contain the `_registry` private property. **]**