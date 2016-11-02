# MqttReceiver Requirements

## Overview
Object used to subscribe to the Cloud-to-Device messages endpoint and receive messages for this device

## Example
```javascript
var receiver = new MqttReceiver(
        client,
        'message_topic/to/subscribe/to'
    );
receiver.on('message', function(msg) {
    console.log('Message received: ' + msg.data);
});
```

## Public API

### Constructor

**SRS_NODE_DEVICE_MQTT_RECEIVER_16_001: [** If the `topicMessage` parameter is falsy, a `ReferenceError` shall be thrown. **]**

**SRS_NODE_DEVICE_MQTT_RECEIVER_16_002: [** If the `mqttClient` parameter is falsy, a `ReferenceError` shall be thrown. **]**

### Events

**SRS_NODE_DEVICE_MQTT_RECEIVER_16_003: [** When a listener is added for the `message` event, the topic should be subscribed to. **]**

**SRS_NODE_DEVICE_MQTT_RECEIVER_13_002: [** When a listener is added for the `method` event, the topic should be subscribed to. **]**

**SRS_NODE_DEVICE_MQTT_RECEIVER_16_004: [** If there is a listener for the `message` event, a `message` event shall be emitted for each message received. **]**

**SRS_NODE_DEVICE_MQTT_RECEIVER_13_003: [** If there is a listener for the `method` event, a `method_<METHOD NAME>` event shall be emitted for each message received. **]**

**SRS_NODE_DEVICE_MQTT_RECEIVER_13_005: [** When a `method_<METHOD NAME>` event is emitted the parameter shall conform to the shape as defined by the interface specified below:

```
interface StringMap {
    [key: string]: string;
}

interface MethodMessage {
    methods: { methodName: string; };
    requestId: string;
    properties: StringMap;
    body: any;
    verb: string;
}
```
**]**

**SRS_NODE_DEVICE_MQTT_RECEIVER_16_005: [** When a `message` event is emitted, the parameter shall be of type `Message`. **]**

**SRS_NODE_DEVICE_MQTT_RECEIVER_16_006: [** When there are no more listeners for the `message` event, the topic should be unsubscribed. **]**

**SRS_NODE_DEVICE_MQTT_RECEIVER_13_004: [** When there are no more listeners for the `method` event, the topic should be unsubscribed. **]**
