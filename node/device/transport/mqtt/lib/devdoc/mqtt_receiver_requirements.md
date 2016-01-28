# MqttReceiver Requirements

## Overview
Object used to subscribe to the Cloud-to-Device messages endpoint and receive messages for this device

## Example
```javascript
var receiver = new MqttReceiver(client, 'topic/to/subscribe/to');
receiver.on('message', function(msg) {
    console.log('Message received: ' + msg.data);
});
```

## Public API

### Constructor
**SRS_NODE_DEVICE_MQTT_RECEIVER_16_001: [**If the topic_subscribe parameter is falsy, a ReferenceError shall be thrown.**]**
**SRS_NODE_DEVICE_MQTT_RECEIVER_16_002: [**If the mqttClient parameter is falsy, a ReferenceError shall be thrown.**]**

### Events
**SRS_NODE_DEVICE_MQTT_RECEIVER_16_003: [**When a listener is added for the message event, the topic should be subscribed to.**]**
**SRS_NODE_DEVICE_MQTT_RECEIVER_16_004: [**If there is a listener for the message event, a message event shall be emitted for each message received.**]**
**SRS_NODE_DEVICE_MQTT_RECEIVER_16_005: [**When a message event is emitted, the parameter shall be of type Message**]**
**SRS_NODE_DEVICE_MQTT_RECEIVER_16_006: [**When there are no more listeners for the message event, the topic should be unsubscribed**]**