# Node-Red node for Azure IoT Hub

This Node-RED node adds Azure IoT connectivity to your Node-RED flow.

## Prerequisites
To use the Azure IoT Hub Node-RED node, you need to have the following installed on the target machine:

- Node.js v0.10 or above. (Check out [Nodejs.org](https://nodejs.org/) for more info)
- Node-RED. To install Node-RED, run the following command:
   ```
   npm install -g node-red
   ```
- Azure IoT Hub instance deployed in an Azure subscription with a device ID provisionned. You can learn how to deploy an Azure IoT Hub instance, create a Device ID and retrieve its connection string [here](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).

## Install the Node-RED node 
To install the node, run the following command from the folder containing the package.json file for the node.
   ```
   npm install -g
   ```

## First simple flow
To learn how to use the Azure IoT Hub node, here is a simple example:
1. run Node-RED typing the following command
   ```
   node-red
   ```
1. Open your Web browser and navigate to  [http://localhost:1880](http://localhost:1880)
1. Click on the top right menu and select **Import | Clipboard**
1. Paste the below in the popup window:
   ```
   [{"id":"7b108cb1.4807c8","type":"azureiothub","z":"8d630437.6181a8","name":"Azure IoT Hub","protocol":"amqp","x":379.5,"y":89.22000122070312,"wires":[["b79e7772.b39cc8"]]},{"id":"2269b09.2499b5","type":"inject","z":"8d630437.6181a8","name":"Inject data","topic":"","payload":"Hello from device","payloadType":"str","repeat":"","crontab":"","once":false,"x":209.5,"y":196.01998901367187,"wires":[["7b108cb1.4807c8"]]},{"id":"b79e7772.b39cc8","type":"function","z":"8d630437.6181a8","name":"Convert Bytes to String","func":"msg.payload = msg.payload.toString();\nreturn msg;","outputs":1,"noerr":0,"x":614.5,"y":201.57000732421875,"wires":[["a142535d.158f4"]]},{"id":"a142535d.158f4","type":"debug","z":"8d630437.6181a8","name":"","active":true,"console":"false","complete":"false","x":762.5,"y":320.1499938964844,"wires":[]}]
   ```
1. Double click on the **Azure IoT Hub** node.
1. Select the protocol of your choice.
1. Enter the connection string for the device (see prerequisites for details).
1. Click OK to validate
1. Click on the **Deploy** red button in the top left to deploy the flow. Note that if you change the settings (protocol or connection string) you will need to redeploy the flow for the settings to be taken into account.
1. You can now click on the **Inject Data** node to send messages to Azure IoT Hub
1. When receiving messages from Azure IoT Hub, they will be displayed in the debug tab.
