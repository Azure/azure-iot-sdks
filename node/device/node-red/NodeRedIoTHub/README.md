# node-red-contrib-azure-iot-hub

node-red-contrib-azure-iot-hub is a <a href="http://nodered.org" target="_new">Node-RED</a> node that allows you to send messages and register devices with Azure IoT Hub. This is a fork from the original Node-RED example by the Azure IoT team found [here](https://github.com/Azure/azure-iot-sdks/tree/master/node/device/node-red).

It contains two new Node-RED cloud nodes: **Azure IoT Hub** and **Azure IoT Registry**

![](images/flow-nodes.png)

#### Azure IoT Hub

This node allows you to send messages to your Azure IoT Hub.  It has the following payload format:
```
{
  "deviceId": "testenode1",
  "key": "cw3nniq77BbjpOCDLqb7xEFTU509HeR6Ki1NwR20jj0=",
  "protocol": "amqp",
  "data": "{tem: 25, wind: 20}"
}
```
- deviceId is your device's unique id.
- key is your device's primary or secondary key.
- protocol options are: amqp, amqpws, mqtt or http.
- data can either be a plain string or string wrapped JSON.

#### Azure IoT Registry

This node allows you to registers devices with your Azure IoT Hub. It has the following payload format:

```
{
  "deviceId": "testenode1"
}
```

## Installation

```
npm install -g node-red-contrib-azure-iot-hub
```



## How to Use


### Node-RED Setup
Follow the instructions [here](http://nodered.org/docs/getting-started/installation) to get Node-RED setup locally.

### Register Azure IoT Hub Device

1. Open Node-RED, usually: <http://127.0.0.1:1880>

2. Go to Hamburger Menu -> Import -> Clipboard

    ![](images/import-clip.png)

3. Paste the following code into the "Import nodes" dialog

    ![](images/import-nodes.png)

    ```
    [{"id":"e092747f.d2fb08","type":"azureiothubregistry","z":"a625ca19.b34ce8","name":"Azure IoT Hub Registry","x":370,"y":120,"wires":[["4c1b62c2.1681bc"]]},{"id":"8877c5f0.e15828","type":"inject","z":"a625ca19.b34ce8","name":"Register Payload","topic":"","payload":"{\"deviceId\": \"device146\"}","payloadType":"json","repeat":"","crontab":"","once":false,"x":140,"y":120,"wires":[["e092747f.d2fb08"]]},{"id":"4c1b62c2.1681bc","type":"debug","z":"a625ca19.b34ce8","name":"Log","active":true,"console":"false","complete":"true","x":650,"y":120,"wires":[]},{"id":"a3b3f737.a7a428","type":"debug","z":"a625ca19.b34ce8","name":"Log","active":true,"console":"false","complete":"true","x":650,"y":60,"wires":[]},{"id":"2be1c658.c9bbea","type":"azureiothub","z":"a625ca19.b34ce8","name":"Azure IoT Hub","protocol":"amqp","x":340,"y":60,"wires":[["a3b3f737.a7a428"]]},{"id":"600a3eb0.2b238","type":"inject","z":"a625ca19.b34ce8","name":"Send Payload","topic":"","payload":"{ \"deviceId\": \"device145\", \"key\": \"Qmq2SSe4CzuB5N0v2FvfV3LAE8OKf2rWj6IQPx+AU3w=\", \"protocol\": \"amqp\", \"data\": \"{tem: 25, wind: 20}\" }","payloadType":"json","repeat":"","crontab":"","once":false,"x":130,"y":60,"wires":[["2be1c658.c9bbea"]]}]
    ```
4. Double-click the Register Payload node

    ![](images/register-payload-node.png)

5. Enter your desired deviceId into the Payload field and click Done. Check "Inject once at start?" to register this device when you click Deploy.

    ![](images/register-payload-input.png)

6. Double-click the Azure IoT Hub Registry node, enter your IoT Hub connection string and click Done.

    ![](images/azureiot-registry-node.png) 
    
    ![](images/azureiot-registry-input.png)

7. Click Deploy

    ![](images/deploy.png)

8. Click the square button on the left side of the Register Payload node.
    
    ![](images/register-payload-node.png)

9. Click on the debug tab to your right and you'll see the device's primary and secondary keys.

    ![](images/device-register-output.png)


### Send Data to Azure IoT Hub

1. Double-click on the Send Payload node and modify the JSON in the Payload field to contain your deviceId, key, protocol and data.

    ![](images/send-payload-node.png)

    ![](images/send-payload-input.png)

2. Double-click the Azure IoT Hub node and enter your hostname and click Done.

    ![](images/azureiot-hub-node.png)

    ![](images/azureiot-hub-input.png)

3. Click Deploy

    ![](images/deploy.png)

4. Click the square button on the left side of the Send Payload node.
    
    ![](images/send-payload-node.png)

5. You should then see a Message sent debug message in the debug pane.

    ![](images/message-sent.png)

### References
You can read more about Azure IoT Hub [here](https://azure.microsoft.com/en-us/documentation/services/iot-hub/).



