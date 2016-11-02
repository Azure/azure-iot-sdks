# azure-iot-device.Twin Requirements

## Overview
azure-iot-device.Twin provides access to the Device Twin functionaliy of IoTHub.

## Example usage
```js
var Protocol = require('azure-iot-device-mqtt').Mqtt;
var Client = require('azure-iot-device').Client;

// Create IoT Hub client
var client = Client.fromConnectionString('[IoT Hub device connection string]', Protocol);

// Create device Twin
client.getTwin(client, function(err, thermostat) {
  if (err) {
  console.error('could not establish connection to twin');
} else {
  console.log('connected to twin');

  var patch = {
    firmwareVersion: deviceFirmwareVersion,
    climate : { 
      humidity: 90, 
      temperature: 72 
      }
  };

  // Report firmare version and tempeature.
  thermostat.properties.reported.update(patch, function(err) {
    if (err) {
      console.error('update failed');
    } else {
      console.log('twin state updated successfully');
    }
  });

  thermostat.on('properties.desired', function(patch) {
    console.log('patch received:');
    console.log(patch);
  });

});
 

```

## Implementation notes

All service-to-device version identifiers are ignored.  No device-to-service version identifiers shall be sent.  

TODO: All PATCHes arriving before the GET response comes back shall be ignored.

TODO: handle re-connection.  Need to unsub, resub, and get.  We can't currently do this because the transport doesn't have an "onConnectionDropped" event.

## Public Interface


### `fromDeviceClient` method (static)
The `fromDeviceclient` method creates a device twin connection to the given hub and calls the `done` method after the connection is complete.

**SRS_NODE_DEVICE_TWIN_18_002: [** `fromDeviceclient` shall throw `ReferenceError` if the `client` object is falsy **]**

**SRS_NODE_DEVICE_TWIN_18_030: [** `fromDeviceclient` shall throw `ReferenceError` if the `done` argument is falsy **]**

**SRS_NODE_DEVICE_TWIN_18_028: [** if `fromDeviceClient` has previously been called for this client, it shall perform a GET operation and return the same object **]**

**SRS_NODE_DEVICE_TWIN_18_029: [** if `fromDeviceClient` is called with 2 different `client`s, it shall return 2 unique `Twin` objects **]**

**SRS_NODE_DEVICE_TWIN_18_003: [** `fromDeviceClient` shall allocate a new `Twin` object **]** 

**SRS_NODE_DEVICE_TWIN_18_004: [** `fromDeviceClient` shall call `getTwinReceiver` on the protocol object to get a twin receiver. **]** 

**SRS_NODE_DEVICE_TWIN_18_005: [** If the protocol does not contain a `getTwinReceiver` method, `fromDeviceClient` shall call the `done` callback with a `NotImplementedError` object **]** 

**SRS_NODE_DEVICE_TWIN_18_007: [** `fromDeviceClient` shall add handlers for the both the `subscribed` and `error` events on the twinReceiver **]** 

**SRS_NODE_DEVICE_TWIN_18_031: [** `fromDeviceClient` shall subscribe to the response topic **]**

**SRS_NODE_DEVICE_TWIN_18_032: [** `fromDeviceClient` shall subscribe to the desired property patch topic **]**

**SRS_NODE_DEVICE_TWIN_18_009: [** `fromDeviceClient` shall call the `done` callback passing a `TimeoutError` if it has not received all necessary `subscribed` events within `Twin.timeout` milliseconds. **]** 

**SRS_NODE_DEVICE_TWIN_18_010: [** `fromDeviceClient` shall call the `done` callback passing  the first error that is returned from `error` event on the twinReceiver. **]** 

**SRS_NODE_DEVICE_TWIN_18_044: [** `fromDeviceClient` shall do a GET call to retrieve desired properties from the service. **]**

**SRS_NODE_DEVICE_TWIN_18_043: [** If the GET operation fails, `fromDeviceClient` shall call the done method with the error object in the first parameter **]**

**SRS_NODE_DEVICE_TWIN_18_034: [** `fromDeviceClient` shall ignore any PATCH operations that arrive before the GET returns **]**

**SRS_NODE_DEVICE_TWIN_18_012: [** `fromDeviceClient` shall remove the handlers for both the `subscribed` and `error` events before calling the `done` callback. **]**


### _sendTwinRequest method (private)
`_sendTwinRequest` is a helper method which sends an arbitrary message to the hub and waits for a response (or timeout or error)

**SRS_NODE_DEVICE_TWIN_18_014: [** `_sendTwinRequest` shall use an arbitrary starting `rid` and increment it by one for every call to `_sendTwinRequest` **]** 

**SRS_NODE_DEVICE_TWIN_18_015: [** `_sendTwinRequest` shall add a handler for the `response` event on the twinReceiver object.  **]**

**SRS_NODE_DEVICE_TWIN_18_016: [** `_sendTwinRequest` shall use the `sendTwinRequest` method on the transport to send the request **]** 

**SRS_NODE_DEVICE_TWIN_18_017: [** `_sendTwinRequest` shall put the `rid` value into the `properties` object that gets passed to `sendTwinRequest` on the transport **]** 

**SRS_NODE_DEVICE_TWIN_18_018: [** The response handler shall ignore any messages that don't have an `rid` that matches the `rid` of the request **]**  

**SRS_NODE_DEVICE_TWIN_18_019: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 or 204 **]**  

**SRS_NODE_DEVICE_TWIN_18_020: [** `_sendTwinRequest` shall call `done` with an `err` value translated using `translateError`  **]** 

**SRS_NODE_DEVICE_TWIN_18_021: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]** 

**SRS_NODE_DEVICE_TWIN_18_022: [** If the response doesn't come within `Twin.timeout` milliseconds, `_sendTwinRequest` shall call `done` with `err`=`TimeoutError` **]**  


### properties.reported.update method
`update` is a method which application developers use to send reported state to the service.

**SRS_NODE_DEVICE_TWIN_18_025: [** `properties.reported.update` shall use _sendTwinRequest to send the patch object to the service. **]** 

**SRS_NODE_DEVICE_TWIN_18_026: [** When calling `_sendTwinRequest`, `properties.reported.update` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the `body` parameter passed in to `reportState` as a string. **]**   

**SRS_NODE_DEVICE_TWIN_18_027: [** `properties.reported.update` shall call `done` with the results from `_sendTwinRequest` **]** 

**SRS_NODE_DEVICE_TWIN_18_031: [** `properties.reported.update` shall merge the contents of the patch object into `properties.reported` **]**

**SRS_NODE_DEVICE_TWIN_18_032: [** When merging the patch, if any properties are set to `null`, `properties.reported.update` shall delete that property from `properties.reported`. **]**

**SRS_NODE_DEVICE_TWIN_18_033: [** If `_sendTwinRequest` fails, `properties.reported.update` shall not merge the contents of the patch object into `properties.reported` **]**


### events for desired property changes

**SRS_NODE_DEVICE_TWIN_18_035: [** When a the results of a GET operation is received, the `Twin` object shall merge the properties into `this.properties.desired`. **]**

**SRS_NODE_DEVICE_TWIN_18_036: [** When a PATCH operation is received, the `Twin` object shall merge the properties into `this.properties.desired`. **]**

**SRS_NODE_DEVICE_TWIN_18_039: [** After merging a GET result, the `Twin` object shall recursively fire property changed events for every changed property. **]**

**SRS_NODE_DEVICE_TWIN_18_040: [** After merging a PATCH result, the `Twin` object shall recursively fire property changed events for every changed property. **]**

**SRS_NODE_DEVICE_TWIN_18_041: [** When firing a property changed event, the `Twin` object shall name the event from the property using dot notation starting with 'properties.desired.' **]**

**SRS_NODE_DEVICE_TWIN_18_042: [** When firing a property changed event, the `Twin` object shall pass the changed value of the property as the event parameter **]**

**SRS_NODE_DEVICE_TWIN_18_045: [** If a property is already set when a handler is added for that property, the `Twin` object shall fire a property changed event for the property. **]* 

