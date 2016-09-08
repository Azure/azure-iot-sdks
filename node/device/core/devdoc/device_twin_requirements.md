# azure-iot-device.DeviceTwin Requirements

## Overview
azure-iot-device.DeviceTwin provides access to the Device Twin functionaliy of IoTHub.

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

**SRS_NODE_DEVICE_TWIN_18_028: [** if `fromDeviceClient` has previously been called for this client, it shall return the same object **]**

**SRS_NODE_DEVICE_TWIN_18_029: [** if `fromDeviceClient` is called with 2 different `client`s, it shall return 2 unique `DeviceTwin` objects **]**

**SRS_NODE_DEVICE_TWIN_18_003: [** `fromDeviceClient` shall allocate a new `DeviceTwin` object **]** 

**SRS_NODE_DEVICE_TWIN_18_004: [** `fromDeviceClient` shall call `getTwinReceiver` on the protocol object to get a twin receiver. **]** 

**SRS_NODE_DEVICE_TWIN_18_005: [** If the protocol does not contain a `getTwinReceiver` method, `fromDeviceClient` shall call the `done` callback with a `NotImplementedError` object **]** 

**SRS_NODE_DEVICE_TWIN_18_007: [** `fromDeviceClient` shall add handlers for the both the `subscribed` and `error` events on the twinReceiver **]** 

**SRS_NODE_DEVICE_TWIN_18_009: [** `fromDeviceClient` shall call the `done` callback passing a `TimeoutError` if it has not received a `subscribed` event within `DeviceTwin.timeout` milliseconds. **]** 

**SRS_NODE_DEVICE_TWIN_18_010: [** `fromDeviceClient` shall call the `done` callback passing  the first error that is returned from `error` event on the twinReceiver. **]** 

**SRS_NODE_DEVICE_TWIN_18_011: [** `fromDeviceClient` shall call the `done` callback with `err`=`null` if it receives a `subscribed` event for the `response` topic. **]** 

**SRS_NODE_DEVICE_TWIN_18_012: [** `fromDeviceClient` shall remove the handlers for both the `subscribed` and `error` events before calling the `done` callback. **]**


### _sendTwinRequest method (private)
`_sendTwinRequest` is a helper method which sends an arbitrary message to the hub and waits for a response (or timeout or error)

**SRS_NODE_DEVICE_TWIN_18_014: [** `_sendTwinRequest` shall use an arbitrary starting `rid` and increment it by one for every call to `_sendTwinRequest` **]** 

**SRS_NODE_DEVICE_TWIN_18_015: [** `_sendTwinRequest` shall add a handler for the `response` event on the twinReceiver object.  **]**

**SRS_NODE_DEVICE_TWIN_18_016: [** `_sendTwinRequest` shall use the `sendTwinRequest` method on the transport to send the request **]** 

**SRS_NODE_DEVICE_TWIN_18_017: [** `_sendTwinRequest` shall put the `rid` value into the `properties` object that gets passed to `sendTwinRequest` on the transport **]** 

**SRS_NODE_DEVICE_TWIN_18_018: [** The response handler shall ignore any messages that don't have an `rid` that matches the `rid` of the request **]**  

**SRS_NODE_DEVICE_TWIN_18_019: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 or 204 **]**  

**SRS_NODE_DEVICE_TWIN_18_020: [** `_sendTwinRequest` shall call `done` with an `err` value translated using http_errors.js **]** 

**SRS_NODE_DEVICE_TWIN_18_021: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]** 

**SRS_NODE_DEVICE_TWIN_18_022: [** If the response doesn't come within `DeviceTwin.timeout` milliseconds, `_sendTwinRequest` shall call `done` with `err`=`TimeoutError` **]**  


### properties.reported.update method
`update` is a method which application developers use to send reported state to the service.

**SRS_NODE_DEVICE_TWIN_18_025: [** `properties.reported.update` shall use _sendTwinRequest to send the patch object to the service. **]** 

**SRS_NODE_DEVICE_TWIN_18_026: [** When calling `_sendTwinRequest`, `properties.reported.update` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the `body` parameter passed in to `reportState` as a string. **]**   

**SRS_NODE_DEVICE_TWIN_18_027: [** `properties.reported.update` shall call `done` with the results from `_sendTwinRequest` **]** 



