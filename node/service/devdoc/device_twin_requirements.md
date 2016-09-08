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

// When the twin is manually created and not retrieved from the IoT Hub, the second parameter (force) must be set to true because we have no valid twin version information. 
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
populate it with the latest state from the Device Registry, including version information that is used to protect a twin against multiple concurrent updates.

In advanced cases where benefitting from optimistic concurrency is not desired or desirable, the `DeviceTwin` constructors allow the users to create an empty `DeviceTwin` object initialized with only the arguments passed to it. It will also set the etag/version properties to `*`
so that if a user calls `update` or `replace` on `tags`, `properties.desired` or the `DeviceTwin` object itself, the call succeeds. This allows updating/replacing parts of the twin without
having to get it first which can be useful in some high performance scenarios.

### DeviceTwin(device, registryClient)
**SRS_NODE_IOTHUB_DEVICETWIN_16_001: [** The `DeviceTwin(device, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the `deviceId` base property to the `device` argument if it is a `string`. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_006: [** The `DeviceTwin(device, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the properties of the created object to the properties described in the `device` argument if it's an `object`. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_002: [** The `DeviceTwin(device, registryClient)` constructor shall throw a `ReferenceError` if `device` is undefined, null or an empty string. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_003: [** The `DeviceTwin(device, registryClient)` constructor shall throw a `ReferenceError` if `registryClient` is falsy. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_007: [** The `DeviceTwin(device, registryClient)` constructor shall throw an `ArgumentError` if `device` is an object and does not have a `deviceId` property. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_005: [** The `DeviceTwin(device, registryClient)` constructor shall set the `DeviceTwin.etag`, `DeviceTwin.tags.$version` and `DeviceTwin.properties.desired.$version` to `*`. **]**

## DeviceTwin.tags
### replace(newTags, force, done)
The `replace` method is used to replace the existing tag dictionary with the one passed as a parameter. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.tags.$version` field if defined. If not defined, `*` will be used.

**SRS_NODE_IOTHUB_DEVICETWIN_16_009: [** The `replace` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT /twins/<DeviceTwin.deviceId>/tags?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.tags.$version> | *

<tags object>
```
**]**

## DeviceTwin.properties.desired
### replace(newProps, force, done)
The `replace` method is used to replace the existing desired properties dictionary with the one passed as a parameter. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.properties.desired.$version` field if defined. If not defined, `*` will be used.

The `replace` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT /twins/<DeviceTwin.deviceId>/properties/desired?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.properties.desired.$version> | *

<desired properties object>
```

### update(patch, force, done)
The `update` method is used to update the existing desired properties dictionary with the values passed as a patch argument. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.properties.desired.$version` field if defined. If not defined, `*` will be used.

The `update` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PATCH /twins/<DeviceTwin.deviceId>/properties/desired?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.properties.desired.$version> | *

<desired properties object>
```

## Methods
### get(done)
The `get` method is used to refresh the `DeviceTwin` instance with the latest values from the IoT Hub Registry.

**SRS_NODE_IOTHUB_DEVICETWIN_16_014: [** The `get` method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET /twins/<DeviceTwin.deviceId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Request-Id: <guid>
``` **]**

### update(patch, force, done)
The `update` method is used to update any part of the device twin stored in the IoT Hub registry with the values passed as a patch argument. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.eTag` field if defined. If not defined, `*` will be used.

The `update` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PATCH /twins/<DeviceTwin.deviceId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.eTag> | *

<deviceInfo>
```

### toJSON()
The `toJSON` method is called when calling `JSON.stringify()` on a `DeviceTwin` object.

**SRS_NODE_IOTHUB_DEVICETWIN_16_015: [** The `toJSON` method shall return a copy of the `DeviceTwin` object that doesn't contain the `_registry` private property. **]**

# All Device Twin update/replace methods
On top of the [common registry requirements](registry_requirements.md) the device twin methods must implement the following requirements:

**SRS_NODE_IOTHUB_DEVICETWIN_16_008: [** The method shall throw a `ReferenceError` if the patch object is falsy. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_010: [** The method shall set the `If-Match` header to `*` if the `force` argument is `true` **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_011: [** The method shall throw an `InvalidEtagError` if the `force` argument is set to false and the corresponding `$version` property is undefined or set to `*`. **]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_012: [** The method shall throw a `ReferenceError` if the `force` argument does not meet one of the following conditions:
- `force` is boolean and `true`,
- `force` is boolean and `false`.
- `force` is a function and `done` is `undefined`.
- `force` is `undefined` and `done` is `undefined`
**]**

**SRS_NODE_IOTHUB_DEVICETWIN_16_013: [** The `force` argument shall be optional, in other words, the method shall use the `force` argument as the callback if `done` is `undefined` and `force` is a function. **]**
