# azure-iot-device.DeviceTwin Requirements

## Overview
azure-iot-device.DeviceTwin provides access to the Device Twin functionaliy of IoTHub.

## Example usage
```js
var Protocol = require('azure-iot-device-mqtt').Mqtt;
var Client = require('azure-iot-device').Client;
var DeviceTwin = require('azure-iot-device').DeviceTwin;

var thermostatModel = {
  reported:{ 
    firmwareVersion:’1.2.1’,
    weather:{
      temperature:0,
      humidity:0
    }
  },
  desired:{
    climateControl:{
      minTemperature:60,
      maxTemperature:70
    }
  }
}

// Create IoT Hub client
var client = Client.fromConnectionString('[IoT Hub device connection string]', Protocol);
// Create device Twin
var thermostat = new DeviceTwin(client, thermostatModel);

thermostat.on('connectionError', function(detail) {
  console.log('connection error');
  console.log(detail);
};

thermostat.on('ready', function() {
  console.log('connected to hub');

  // Set Device Twin properties
  thermostat.reported.firmwareVersion = deviceFirmwareVersion;

  // Report Device Twin state
  thermostat.reportTwinState(thermostat.reported.firmwareVersion, function(err) {
    if (err) {
      console.log('error reporting state');
    } else {
      console.log('twin state updated successfully');
    }
  });

  // Event handler for climage control update  
  thermostat.on('climateControlUpdated', function() {
    // Setup stuff according to desired property change
    console.log('Received new heating control settings :'
      + thermostat.desired.climateControl.minTemperature
      + thermostat.desired.climateControl.maxTemperature);
  });
});
 

```

## Implementation notes

M13 requirements are marked with M13.

TEMPORARY_M13 requirements can be removed after the full API is implemented.

All service-to-device version identifiers are ignored.  No device-to-service version identifiers shall be sent.  

TODO: All PATCHes arriving before the GET response comes back shall be ignored.

TODO: handle re-connection.  Need to unsub, resub, and get.  We can't currently do this because the transport doesn't have an "onConnectionDropped" event.

## Public Interface



### DeviceTwin constructor
The `DeviceTwin` constructor creates a new instance of the `DeviceTwin` object.

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_001: [** The `DeviceTwin` constructor shall accept 2 parameters: an azure-iot-device client and a device model. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_002: [** The `DeviceTwin` constructor shall throw `ReferenceError` if the client object is falsy **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_003: [** The `DeviceTwin` constructor shall throw `ReferenceError` if the model object is falsy **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_005: [** The `DeviceTwin` constructor shall asynchronously call _connectAndConfigure **]**



### _connectAndConfigure method (private)
`_connectAndConfigure` is a helper method which connects to the IoTHub and configures the connection.  After `_connectAndConfigure` is done, it 
will either fire the `ready` event, which indicates that the connection is ready, or it will fire the `connectionError` event which indicates 
that the connection is not ready. 

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_036: [** `_connectAndConfigure` shall  call `_populateDeviceModel` **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_009: [** If `_popualteDeviceModel` fails, `_connectAndConfigure` shall fire the `connectionError` event  **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_004: [** If `_populateDeviceModel` succeeds, `_connectAndConfigure` shall  call `_getTwinReceiver` **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_010: [** If `_getTwinReceiver` fails, `_connectAndConfigure` shall fire the `connectionError` event **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_008: [** If `_getTwinReceiver` succeeds, `_connectAndConfigure` shall call `_registerForServiceMessages` **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_037: [** If `_registerForServiceMessages` fails, `_connectAndConfigure` shall fire the `connectionError` event **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_076: [** TEMPORARY_M13 If `_registerForServiceMessages` succeeds, `_connectAndConfigure` shall fire the `ready` event **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_038: [** If `_registerForServiceMessages` succeeds, `_connectAndConfigure` shall call `_GetDesiredState` **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_007: [** If `_getDesiredState` fails, `_connectAndConfigure` shall fire the `connectionError` event **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_006: [** If `_getDesiredState` succeeds, `_connectAndConfigure` shall fire the `ready` event. **]**



### _populateDeviceModel method (private)
`_populateDeviceModel` is a helper method which will copy properties of the model into the `DeviceTwin` object so that app developer can access them as properties of the twin.

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_015: [** `_populateDeviceModel` shall accept 2 parameters: an object representing the device model and a done callback. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_016: [** `_populateDeviceModel` shall throw a `ReferenceError` if the `done` callback is falsy. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_017: [** `_populateDeviceModel` shall throw a `ReferenceError` if the `model` is falsy. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_018: [** `_populateDeviceModel` shall populate `Desired` and `Reported` properties into the `DeviceTwin` object based on the same-named top-level properties in the `model`. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_019: [** If the `model` has any top-level properties besides `Desired` and `Reported`, `_populateDeviceModel` shall ignore them **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_020: [** If the `model` does not contain either `Desired` or `Reported` as top-level properties, `_populateDeviceModel` shall return an `ArgumentError` **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_021: [** If the `model` contains any properties or nested properties that are functions, `_populateDeviceModel` return an `ArgumentError` **]** 



### _getTwinReceiver method (private)
`_getTwinReceiver` is a helper method which acquires an object that can be used to listen to messages from the hub.

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_011: [** The `_getTwinReceiver` method calls `getTwinReceiver` on the protocol object to get a twin receiver. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_012: [** If the protocol does not contain a `getTwinReceiver` method, `_getTwinReceiver` shall return a `NotImplementedError` in the `done` callback. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_013: [** The `_getTwinReceiver` method stores the twinReceiver obejct as a property **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_014: [** If `_getTwinReceiver` succeeds, it shall call the `done` callback with no parameters **]**



### _registerForServiceMethods method (private)
`_registerForServiceMessages` is a helper method which does the handshaking required to establish hub-to-clint message subscriptions.

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_039: [** `_registerForServiceMessages` shall accept a `done` calback. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_040: [** `_registerForServiceMessages` shall add `_onServiceResponse` as a handler for the `response` event on the twinReceiver **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_041: [** `_registerForServiceMessages` shall add `_onPostResponse` as a handler for the `post` event on the twinReceiver **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_042: [** `_registerForServiceMessages` shall add handlers for the both the `subscribed` and `error` events on the twinReceiver **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_043: [** `_registerForServiceMessages` shall return a `ServiceUnavailableError` it has not received `subscribed` events for both topics within `DeviceTwin.timeout` milliseconds. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_044: [** `_registerForServiceMessages` shall return the first error that is returned from `error` event on the twinReceiver. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_045: [** `_registerForServiceMessages` shall call `done` with `err`=`null` if it receives `success` events for both the `response` and `post` topics. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_046: [** `_registerForServiceMessages` shall remove the handlers for both the `subscribed` and `error` events before calling the `done` callback. **]** 



### _getDesiredState method  (private)
`_getDesiredState` is a helper method which queries the hub for the current desired state and waits for the response.

**SRS_NODE_DEVICE_DEVICETWIN_18_022: [** `_getDesiredState` shall accept a `done` parameter **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_028: [** `_getDesiredState` shall fire a `ReferenceError` if `done` is falsy **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_023: [** `_getDesiredState` shall use `_sendTwinRequest` with method='GET' and resource='/properties/desired/' to get the desired properties from the service **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_024: [** If `_sendTwinRequest` fails, `_getDesiredState` shall pass the error to the `done` callback. **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_070: [** If `_sendTwinRequest` succeeds, `_getDesiredState` shall set `this.desired` to {} before ingesting the new state. **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_025: [** If `_sendTwinRequest` succeeds, `_getDesiredState` shall call `_ingestDesiredState` to ingest the resulting message. **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_026: [** If `_ingestDesiredState` fails, `_getDesiredState` shallwill pass the error to the `done` callback **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_027: [** If `_ingestDesiredState` succeeds, `_getDesiredState` shall call the `done` callback with err=null **]**  



### _ingestDesiredState method (private)
`_ingestDesiredState` is a method which accepts a desired state, ingests it into the `DeviceTwin.desired` object and fires onChanged events for the appropraite top-level properties.

**SRS_NODE_DEVICE_DEVICETWIN_18_029: [** `_ingestDesiredState` accepts an object with desired properties (`props`) and a `done` callback. **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_030: [** `_ingestDesiredState` shall ingest `props.desired` and ignore all other top-level properties. **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_058: [** if the `props` parameter is a string, `_ingestDesiredState` shall convert it into an object using `JSON.parse` **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_059: [** If `JSON.parse` throws a `SyntaxError`, `_ingestDesiredState` shall pass that error into the `done` callback. **]**  

**SRS_NODE_DEVICE_DEVICETWIN_18_031: [** `_ingestDesiredState` shall recursively remove any properties inside the `props` object that are set to null from the `this.desired` object **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_032: [** `_ingestDesiredState` shall recursively add any new properties inside the `props` object to the `this.desired` object **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_033: [** `_ingestDesiredState` shall recursively update any scalar properties inside the `props` object to the `this.desired` object **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_034: [** `_ingestDesiredState` shall not touch any properties that are present in `this.desired` but not present in `props` **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_035: [** For each top-level object that had a property added, updated, or removed, `_ingestDesiredState` will fire an event called 'topLevelPropNameUpdated' passing the contents of the top-level property as a parameter **]** 



### _sendTwinRequest method (private)
`_sendTwinRequest` is a helper method which sends an arbitrary message to the hub and waits for a response (or timeout or error)

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_047: [** `_sendTwinRequest` shall accept a `method`, `resource`, `properties`, `body`, and `done` callback **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_048: [** `_sendTwinRequest` shall use an arbitrary starting `rid` and incriment by it by one for every call to `_sendTwinRequest` **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_049: [** `_sendTwinRequest` shall add a handler for the `response` event on the twinReceiver object. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_050: [** `_sendTwinRequest` shall use the `sendTwinReqest` method on the transport to send the request **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_051: [** `_sendTwinRequest` shall put the `rid` value into the `properties` object that gets passed to `sendTwinRequest` on the transport **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_052: [** The response handler shall ignore any messages that don't have an `rid` that mattches the `rid` of the request **]** 

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_053: [** `_sendTwinRequest` shall call `done` with `err`=`null` if the response event returns `status`===200 **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_054: [** `_sendTwinRequest` shall call `done` with an `err` value generated using http_errors.js **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_055: [** Before calling `done`, `_sendTwinRequest` shall remove the handler for the `response` event **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_056: [** If the response doesn't come within `DeviceTwin.timeout` milliseconds, `_sendTwinRequest` shall call `done` with `err`=`ServiceUnavailableError` **]** 



### _onServiceResponse (private)
`_onServiceResponse` is a handler for response events from the hub.  Since each individual request also has it's own response handler, this method is a no-op.  It is used to manage the lifetime of the response subscription.

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_057: [** `_onServiceResponse` shall ignore all parameters and immediately return. **]**



### _onServicePost (private)
`_onServicePost` is a helper method which handles unsolicited posts from the service.

**SRS_NODE_DEVICE_DEVICETWIN_18_060: [** `_onServicePost` shall accept a single parameter which represents the payload of the posted message. **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_061: [** `_onServicePost` shall pass the payload of the message into `_ingestDesiredState` **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_062: [** `_onServicePost` shall ignore any errors returned by `_ingestDesiredState` **]**

**SRS_NODE_DEVICE_DEVICETWIN_18_063: [** `_onServicePost` shall not generate a response from the client back to the service. **]** 



### reportTwinState method
`reportTwinState` is a method which application developers use to send reported state to the service.

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_065: [** `reportTwinState` shall accept an object which represents the `reported` state and a `done` callback **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_066: [** `reportTwinState` shall use _createPatchObject to create a patch object which gets sent to the service. **]**

M13 When calling `_createPatchObject`, `reportTwinState` shall pass `base`=`this.reported` and `delta`=`reported`

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_067: [** `reportTwinState` shall use _sendTwinRequest to send the patch object to the service. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_069: [** When calling `_sendTwinRequest`, `reportTwinState` shall pass `method`='PATCH', `resource`='/properties/reported/', `properties`={}, and `body`=the result of `_createPatchObject` as a `String`. **]**  

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_068: [** `reportTwinState` shall call `done` with the results from `_sendTwinRequest` **]**



### _createPatchObject (private)
_createPatchObject is a helper method which takes a base object and a delta (which could be a sub-object) and creates a patch object.

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_071: [** `_createPatchObject` shall accept 3 parameters: a `base` object, a `delta` object, and a `done` callback **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_072: [** If the `delta` object is not a property or sub-property of the `base` object, `_createPatchObject` will call `done` with `err`=`ArgumentError` **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_073: [** `_createPatchObject` will search the `base` object to find out where in the object heirarchy the `delta` object sits. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_074: [** `_createPatchObject` will construct a new patch object which contains the contents of `delta` object placed in the heirarchy where it belongs. **]**

M13 **SRS_NODE_DEVICE_DEVICETWIN_18_075: [** `_createPatchObject` will return the patch object via the `done` callback. **]**  

