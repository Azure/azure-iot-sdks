# MqttTwinReceiver Requirements

## Overview
Object used to subscribe to the Cloud-to-Device messages for Twin

## Example
```javascript
var receiver = new MqttTwinReceiver({ 'client' : client });
receiver.on('response', function(status, requestId, body) {
    console.log('Response received for request ' + requestId);
    console.log('  status = " + status);
    console.log(body);
});
```

## Public API

### Constructor

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_001: [** The `MqttTwinReceiver` constructor shall accept a `client` object **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_002: [** The `MqttTwinReceiver` constructor shall throw `ReferenceError` if the `client` object is falsy **]**

### response event

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_003: [** When a listener is added for the `response` event, the appropriate topic shall be asynchronously subscribed to. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_009: [** The subscribed topic for `response` events shall be '$iothub/twin/res/#' **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_004: [** If there is a listener for the `response` event, a `response` event shall be emitted for each response received. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_005: [** When there are no more listeners for the `response` event, the topic should be unsubscribed **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_006: [** When a `response` event is emitted, the parameter shall be an object which contains `status`, `requestId` and `body` members **]** 

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_010: [** The topic which receives the response shall be formatted as '$iothub/twin/res/{status}/?$rid={request id}' **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_015: [** the {status} and {request id} fields in the topic name are required. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_016: [** The {status} and {request id} fields in the topic name shall be strings **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_017: [** The {status} and {request id} fields in the topic name cannot be zero length. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_014: [** Any messages received on topics which violate the topic name formatting shall be ignored. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_011: [** The `status` parameter of the `response` event shall be parsed out of the topic name from the {status} field **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_012: [** The `requestId` parameter of the `response` event shall be parsed out of the topic name from the {request id} field **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_013: [** The `body` parameter of the `response` event shall be the body of the received MQTT message **]** 

### post event

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_018: [** When a listener is added to the post event, the appropriate topic shall be asynchronously subscribed to. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_019: [** The subscribed topic for post events shall be $iothub/twin/PATCH/properties/desired/# **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_020: [** If there is a listener for the post event, a post event shal be emitteded for each post message received **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_021: [** When there are no more listeners for the post event, the topic should be unsubscribed. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_022: [** When a post event it emitted, the parameter shall be the body of the message **]**

### error event

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_023: [** If the `error` event is subscribed to, an `error` event shall be emitted if any asynchronous subscribing operations fails. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_024: [** When the `error` event is emitted, the first parameter shall be an error object obtained via the MQTT `translateErrror` module. **]**

### subscribed event

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_025: [** If the `subscribed` event is subscribed to, a `subscribed` event shall be emitted after an MQTT topic is subscribed to. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_028: [** When the `subscribed` event is emitted, the parameter shall be an object which contains an `eventName` field and an `transportObject` field. **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_026: [** When the `subscribed` event is emitted because the response MQTT topic was subscribed, the `eventName` field shall be the string 'response' **]** 

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_027: [** When the `subscribed` event is emitted because the post MQTT topic was subscribed, the `eventName` field shall be the string 'post' **]**

**SRS_NODE_DEVICE_MQTT_TWIN_RECEIVER_18_029: [** When the subscribed event is emitted, the `transportObject` field shall contain the object returned by the library in the subscription response. **]**