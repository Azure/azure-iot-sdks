# azure-iot-device.DeviceTwin Requirements

## Overview
azure-iot-device.DeviceTwin provides access to the Device Twin functionaliy of IoTHub.

## Example usage
```js
var Protocol = require('azure-iot-device-mqtt').Mqtt;
var Client = require('azure-iot-device').Client;
var DeviceTwin = require('azure-iot-device').DeviceTwin; // OPEN ISSUE: what module is this in?

// Create IoT Hub client
var client = Client.fromConnectionString('[IoT Hub device connection string]', Protocol);

// Create device Twin
var thermostat = DeviceTwin.fromDeviceClient(client, function(err) {
  if (err) {
  console.log('could not establish connection to twin');
  console.log(detail);
} else {
  console.log('connected to twin');

  // Report firmare version and tempeature.
  thermostat.reportTwinState({
    firmwareVersion = deviceFirmwareVersion,
    climate = { humidity: 90, temperature=72 }
  }, function(err) {
    if (err) {
      console.log('error reporting state');
    } else {
      console.log('twin state updated successfully');
    }
  });

  // OPEN ISSUE: is there a disconnect method
});
 

```

## Implementation notes

All service-to-device version identifiers are ignored.  No device-to-service version identifiers shall be sent.  

TODO: All PATCHes arriving before the GET response comes back shall be ignored.

TODO: handle re-connection.  Need to unsub, resub, and get.  We can't currently do this because the transport doesn't have an "onConnectionDropped" event.

## Public Interface



### `fromDeviceClient` method (statc)
The `fromDeviceclient` method creates a deviceTwin conncetion to the given hub and calls the `done` method after the connection is complete.

**SRS_NODE_DEVICE_TWIN_18_001: [** `fromDeviceclient` shall accept 2 parameters: an azure-iot-device `client` and a `done` method. **]**

**SRS_NODE_DEVICE_TWIN_18_002: [** `fromDeviceclient` shall throw `ReferenceError` if the client object is falsy **]**

**SRS_NODE_DEVICE_TWIN_18_003: [** `fromDeviceClient` shall alloate a new DeviceClient object **]** 

**SRS_NODE_DEVICE_TWIN_18_004: [** `fromDeviceClient` shall call `getTwinReceiver` on the protocol object to get a twin receiver. **]** 

**SRS_NODE_DEVICE_TWIN_18_005: [** If the protocol does not contain a `getTwinReceiver` method, `fromDeviceClient` shall return a `NotImplementedError` in the `done` callback. **]** 

**SRS_NODE_DEVICE_TWIN_18_006: [** `fromDeviceClient` shall store the twinReceiver obejct as a property **]** 

**SRS_NODE_DEVICE_TWIN_18_007: [** `fromDeviceClient` shall add handlers for the both the `subscribed` and `error` events on the twinReceiver **]** 

**SRS_NODE_DEVICE_TWIN_18_008: [** `fromDeviceClient` shall add `_onServiceResponse` as a handler for the `response` event on the twinReceiver **]** 

**SRS_NODE_DEVICE_TWIN_18_009: [** `fromDeviceClient` shall return a `ServiceUnavailableError` it has not received a `subscribed` event within `DeviceTwin.timeout` milliseconds. **]** 

**SRS_NODE_DEVICE_TWIN_18_010: [** `fromDeviceClient` shall return the first error that is returned from `error` event on the twinReceiver. **]** 

**SRS_NODE_DEVICE_TWIN_18_011: [** `fromDeviceClient` shall call `done` with `err`=`null` if it receives a `subscribed` events for the `response` topic. **]** 

**SRS_NODE_DEVICE_TWIN_18_012: [** `fromDeviceClient` shall remove the handlers for both the `subscribed` and `error` events before calling the `done` callback. **]**  


### _sendTwinRequest method (private)
`_sendTwinRequest` is a helper method which sends an arbitrary message to the hub and waits for a response (or timeout or error)

**SRS_NODE_DEVICE_TWIN_18_013: [** `_sendTwinRequest` shall accept a `method`, `resource`, `properties`, `body`, and `done` callback **]** 

**SRS_NODE_DEVICE_TWIN_18_014: [** `_sendTwinRequest` shall use an arbitrary starting `rid` and incriment by it by one for every call to `_sendTwinRequest` **]** 

**SRS_NODE_DEVICE_TWIN_18_015: [** `_sendTwinRequest` shall add a handler for the `response` event on the twinReceiver object.  **]**

**SRS_NODE_DEVICE_TWIN_18_016: [** `_sendTwinRequest` shall use the `sendTwinReqest` method on the transport to send the request **]** 

**SRS_NODE_DEVICE_TWIN_18_017: [** `_sendTwinRequest` shall put the `rid` value into the `properties` object that gets passed to `sendTwinRequest` on the transport **]** 

**SRS_NODE_DEVICE_TWIN_18_018: [** The response handler shall ignore any messages that don't have an `rid` that mattches the `rid` of the request **]**  

**SRS_NODE_DEVICE_TWIN_18_019: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 **]** 

**SRS_NODE_DEVICE_TWIN_18_020: [** `_sendTwinRequest` shall call `done` with an `err` value generated using http_errors.js **]** 

**SRS_NODE_DEVICE_TWIN_18_021: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]** 

**SRS_NODE_DEVICE_TWIN_18_022: [** If the response doesn't come within `DeviceTwin.timeout` milliseconds, `_sendTwinRequest` shall call `done` with `err`=`ServiceUnavailableError` **]**  



### _onServiceResponse (private)
`_onServiceResponse` is a handler for response events from the hub.  Since each individual request also has it's own response handler, this method is a no-op.  It is used to manage the lifetime of the response subscription.

**SRS_NODE_DEVICE_TWIN_18_023: [** `_onServiceResponse` shall ignore all parameters and immediately return. **]** 



### reportTwinState method
`reportTwinState` is a method which application developers use to send reported state to the service.

**SRS_NODE_DEVICE_TWIN_18_024: [** `reportTwinState` shall accept an object which represents the `reported` state and a `done` callback **]** 

**SRS_NODE_DEVICE_TWIN_18_025: [** `reportTwinState` shall use _sendTwinRequest to send the patch object to the service. **]** 

**SRS_NODE_DEVICE_TWIN_18_026: [** When calling `_sendTwinRequest`, `reportTwinState` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the result of `_createPatchObject` as a `String`. **]**   

**SRS_NODE_DEVICE_TWIN_18_027: [** `reportTwinState` shall call `done` with the results from `_sendTwinRequest` **]** 



