# DeviceTwin requirements

## Overview
The `DeviceTwin` class provides methods to interact with a device's twin object stored in Azure IoTHub.

## Example usage: Get and update a device twin
```javascript
'use strict';

var Registry = require('azure-iothub').Registry;

var connectionString = '[Connection string goes here]';
var registry = Registry.fromConnectionString(connectionString);

registry.getDeviceTwin('deviceId', function(err, twin){
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

// Not setting the boolean here would result in an Error, which protects the basic user. Advanced-scenarios users won't mind having to set that boolean.
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
populate it with the latest state from the Device Registry, including version information that is used to protect a twin against multiple concurrent conflicting updates of the twin.

In advanced cases where benefitting from Optimistic Concurrency is not desired or desirable, The `DeviceTwin` constructors allow the users to create an empty `DeviceTwin` object initialized with only the arguments passed to it. It will also set the etag/version properties to `*`
so that if a user calls `update` or `replace` on `tags`, `properties.desired` or the `DeviceTwin` object itself, the call succeeds. This allows to update/replace parts of the twin without
having to get it first which can be useful in some high performance scenarios.

### DeviceTwin(deviceId, registryClient)
The `DeviceTwin(deviceId, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the `deviceId` base property to the `deviceId` argument.

The `DeviceTwin(deviceId, registryClient)` constructor shall throw a `ReferenceError` if `deviceId` is falsy.

The `DeviceTwin(deviceId, registryClient)` constructor shall throw a `ReferenceError` if `registryClient` is falsy.

The `DeviceTwin(deviceId, registryClient)` constructor shall throw an `ArgumentError` if `registryClient` is not of type `Registry`.

The `DeviceTwin(deviceId, registryClient)` constructor shall set the `DeviceTwin.etag`, `DeviceTwin.tags.$version` and `DeviceTwin.properties.desired.$version` to `*`.

### DeviceTwin(deviceObj, registryClient)
The `DeviceTwin(deviceObj, registryClient)` constructor shall initialize an empty instance of a `DeviceTwin` object and set the properties of the created object to the properties described in the `deviceObj` argument.

The `DeviceTwin(deviceObj, registryClient)` constructor shall throw a `ReferenceError` if `deviceObj` is falsy.

The `DeviceTwin(deviceObj, registryClient)` constructor shall throw a `ReferenceError` if `registryClient` is falsy.

The `DeviceTwin(deviceObj, registryClient)` constructor shall throw an `ArgumentError` if `registryClient` is not of type `Registry`.

The `DeviceTwin(deviceId, registryClient)` constructor shall set the `DeviceTwin.etag`, `DeviceTwin.tags.$version` and `DeviceTwin.properties.desired.$version` to `*`.

## DeviceTwin.tags
### replace(newTags, force, done)
The `replace` method is used to replace the existing tag dictionnary with the one passed as a parameter. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.tags.$version` field or `*` if the former is not defined.

The `replace` method shall throw a `ReferenceError` if `newTags` is falsy.

The `replace` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT /twins/<DeviceTwin.deviceId>/tags?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.tags.$version> | *

<tags object>
```
The `replace` method shall set the `If-Match` header to `*` if the `force` argument is `true`

The `replace` method shall throw an `InvalidOperationError` if the `force` argument is set to false and `DeviceTwin.tags.$version` is undefined or set to `*`.

The `replace` method shall throw a ReferenceError if the `force` argument does not meet one of the following conditions:
- `force` is boolean and `true`,
- `force` is boolean and `false`.
- `force` is a function and `done` is `undefined`.

The `replace` method shall use the `force` argument as the callback if `done` is `undefined` and `force` is a function.

## DeviceTwin.properties.desired
### replace(newProps, force, done)
The `replace` method is used to replace the existing desired properties dictionnary with the one passed as a parameter. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.properties.desired.$version` field or `*` if the former is `undefined`.

The `replace` method shall throw a `ReferenceError` if `newProps` is falsy.

The `replace` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT /twins/<DeviceTwin.deviceId>/properties/desired?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.properties.desired.$version> | *

<desired properties object>
```
The `replace` method shall set the `If-Match` header to `*` if the `force` argument is `true`

The `replace` method shall throw an `InvalidOperationError` if the `force` argument is set to false and `DeviceTwin.properties.desired.$version` is `undefined` or set to `*`.

The `replace` method shall throw a ReferenceError if the `force` argument does not meet one of the following conditions:
- `force` is boolean and `true`,
- `force` is boolean and `false`.
- `force` is a function and `done` is `undefined`.

The `replace` method shall use the `force` argument as the callback if `done` is `undefined` and `force` is a function.

### update(patch, force, done)
The `update` method is used to update the existing desired properties dictionnary with the values passed as a patch argument. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.properties.desired.$version` field or `*` if the former is `undefined`.

The `update` method shall throw a `ReferenceError` if `patch` is falsy.

The `update` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PATCH /twins/<DeviceTwin.deviceId>/properties/desired?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.properties.desired.$version> | *

<desired properties object>
```
The `update` method shall set the `If-Match` header to `*` if the `force` argument is `true`

The `update` method shall throw an `InvalidOperationError` if the `force` argument is set to false and `DeviceTwin.properties.desired.$version` is `undefined` or set to `*`.

The `update` method shall throw a ReferenceError if the `force` argument does not meet one of the following conditions:
- `force` is boolean and `true`,
- `force` is boolean and `false`.
- `force` is a function and `done` is `undefined`.

The `update` method shall use the `force` argument as the callback if `done` is `undefined` and `force` is a function.

## Methods
### get(done)
The `get` method is used to refresh the `DeviceTwin` instance with the latest values from the IoT Hub Registry.

The `update` method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET /twins/<DeviceTwin.deviceId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Request-Id: <guid>
```

### update(patch, force, done)
The `update` method is used to update any part of the device twin stored in the IoT Hub registry with the values passed as a patch argument. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `DeviceTwin.eTag` field or `*` if the former is `undefined`.

The `update` method shall throw a `ReferenceError` if `patch` is falsy.

The `update` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PATCH /twins/<DeviceTwin.deviceId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <DeviceTwin.eTag> | *

<deviceInfo>
```
The `update` method shall set the `If-Match` header to `*` if the `force` argument is `true`

The `update` method shall throw an `InvalidOperationError` if the `force` argument is set to false and `DeviceTwin.eTag` is `undefined` or set to `*`.

The `update` method shall throw a ReferenceError if the `force` argument does not meet one of the following conditions:
- `force` is boolean and `true`,
- `force` is boolean and `false`.
- `force` is a function and `done` is `undefined`.

The `update` method shall use the `force` argument as the callback if `done` is `undefined` and `force` is a function.
