# Twin requirements

## Overview
The `Twin` class provides methods to interact with a device's twin object stored in Azure IoTHub.

## Example usage: Get and update a device twin
```javascript
'use strict';

var Registry = require('azure-iothub').Registry;

var connectionString = '[Connection string goes here]';
var registry = Registry.fromConnectionString(connectionString);

registry.getTwin('deviceId', function(err, twin) {
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
    
    // This will succeed even if the twin was updated by another service between the time of the `get` and the `update` operation because we are setting the etag to '*'
    twin.etag = '*';
    twin.update(patch, function(err) {
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

var twin = new Twin('deviceId', registry);
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
In the majority of cases, users are better of creating `Twin` objects using the `Registry.getTwin()` API. This will not only return a new Twin instance but will 
populate it with the latest state from the Device Registry, including the etag that is used to protect a twin against multiple concurrent updates.

In advanced cases where benefitting from optimistic concurrency is not desired or desirable, the `Twin` constructors allow the users to create an empty `Twin` object initialized with only the arguments passed to it. It will also set the etag to `*`
so that if a user calls `update` or `replace` on `tags`, `properties.desired` or the `Twin` object itself, the call succeeds. This allows updating/replacing parts of the twin without
having to get it first which can be useful in some high performance scenarios.

### Twin(device, registryClient)
**SRS_NODE_IOTHUB_TWIN_16_001: [** The `Twin(device, registryClient)` constructor shall initialize an empty instance of a `Twin` object and set the `deviceId` base property to the `device` argument if it is a `string`. **]**

**SRS_NODE_IOTHUB_TWIN_16_006: [** The `Twin(device, registryClient)` constructor shall initialize an empty instance of a `Twin` object and set the properties of the created object to the properties described in the `device` argument if it's an `object`. **]**

**SRS_NODE_IOTHUB_TWIN_16_002: [** The `Twin(device, registryClient)` constructor shall throw a `ReferenceError` if `device` is undefined, null or an empty string. **]**

**SRS_NODE_IOTHUB_TWIN_16_003: [** The `Twin(device, registryClient)` constructor shall throw a `ReferenceError` if `registryClient` is falsy. **]**

**SRS_NODE_IOTHUB_TWIN_16_007: [** The `Twin(device, registryClient)` constructor shall throw an `ArgumentError` if `device` is an object and does not have a `deviceId` property. **]**

**SRS_NODE_IOTHUB_TWIN_16_005: [** The `Twin(device, registryClient)` constructor shall set the `Twin.etag` to `*`. **]**

## Methods
### get(done)
The `get` method is used to refresh the `Twin` instance with the latest values from the IoT Hub Registry.

**SRS_NODE_IOTHUB_TWIN_16_020: [** The `get` method shall call the `getTwin` method of the `Registry` instance stored in `_registry` property with the following parameters:
- `this.deviceId`
- `done`
**]**

### update(patch, done)
The `update` method is used to update any part of the device twin stored in the IoT Hub registry with the values passed as a patch argument. 
To satisfy optimistic concurrency requirements, the HTTP request `If-Match` header will be populated with the `Twin.eTag` field if defined. If not defined, `*` will be used.

**SRS_NODE_IOTHUB_TWIN_16_019: [** The `update` method shall call the `updateTwin` method of the `Registry` instance stored in `_registry` property with the following parameters:
- `this.deviceId`
- `patch`
- `this.etag`
- `done`
**]**

### All twin methods calling to a Registry API
**SRS_NODE_IOTHUB_TWIN_16_021: [** The method shall copy properties, tags, and etag in the twin returned in the callback of the `Registry` method call into its parent object. **]**  
**SRS_NODE_IOTHUB_TWIN_16_022: [** The method shall call the `done` callback with an `Error` object if the request failed **]**  
**SRS_NODE_IOTHUB_TWIN_16_023: [** The method shall call the `done` callback with a `null` error object, its parent instance as a second argument and the transport `response` object as a third argument if the request succeeded. **]**  

### toJSON()
The `toJSON` method is called when calling `JSON.stringify()` on a `Twin` object.

**SRS_NODE_IOTHUB_TWIN_16_015: [** The `toJSON` method shall return a copy of the `Twin` object that doesn't contain the `_registry` private property. **]**