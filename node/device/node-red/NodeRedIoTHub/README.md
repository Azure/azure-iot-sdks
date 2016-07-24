node-red-contrib-azure-iot-hub
==============================

<a href="http://nodered.org" target="_new">Node-RED</a> nodes to talk to Azure IoT Hub.

This is a fork from the original node by Microsoft (https://github.com/Azure/azure-iot-sdks/tree/master/node/device/node-red)

Some code of Azure are under MIT License.

Install
-------

Run the following command in your Node-RED user directory - typically `~/.node-red`

        npm install node-red-contrib-azure-iot-hub

Usage
-----

Provides two nodes, one of them,  ork from original Microsoft's node, that you can send data to Azure IoT Hub, but you pass deviceID and SharedAccessKey thru paylod to connect more than one device into the same node. And one to create a new device into IoT Hub.

### Input

Azure  node. Can be used to send a message to Azure IoT Hub:

 - Supports AMQP, MQTT and HTTP protocols
 - Supports RegistryManager

Use `msg.payload` to send a string with all data what you want to send to Azure, plus deviceID, protocol and SharedAccessKey.

Ex: 'msg.payload' -> {"deviceID": "testenode1", "SAK": "cw3nniq77BbjpOCDLqb7xEFTU509HeR6Ki1NwR20jj0=", "Protocol": "amqp", "Data": "25"};
 
Ex: 'msg.payload' using JSON in Data -> {"deviceID": "testenode1", "SAK": "cw3nniq77BbjpOCDLqb7xEFTU509HeR6Ki1NwR20jj0=", "Protocol": "amqp", "Data": "{tem: 25, wind: 20}"}


### Registry Node

Azure node to registry previously you device.

Use `msg.payload` to send a deviceID, registry and receive a Shared Access Key to connecto to Azure IoT Hub.

Ex: 'msg.payload' -> {"deviceID": "testenode1"};


Read more about Azure IoT Hub on <a href="https://azure.microsoft.com/en-us/documentation/services/iot-hub/">Azure IoT Hub</a>.



