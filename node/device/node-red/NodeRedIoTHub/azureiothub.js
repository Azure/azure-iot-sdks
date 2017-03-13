module.exports = function (RED) {

    var Client = require('azure-iot-device').Client;
    var Registry = require('azure-iothub').Registry;
    var Message = require('azure-iot-device').Message;

    var Protocols = {
        amqp: require('azure-iot-device-amqp').Amqp,
        mqtt: require('azure-iot-device-mqtt').Mqtt,
        http: require('azure-iot-device-http').Http,
        amqpWs: require('azure-iot-device-amqp-ws').AmqpWs
    };

    var client = null;
    var clientConnectionString = "";
    var newConnectionString = "";
    var newProtocol = "";
    var clientProtocol = "";

    var statusEnum = {
        disconnected: { color: "red", text: "Disconnected" },
        connected: { color: "green", text: "Connected" },
        sent: { color: "blue", text: "Sent message" },
        received: { color: "yellow", text: "Received" },
        error: { color: "grey", text: "Error" }
    };

    var setStatus = function (node, status) {
        node.status({ fill: status.color, shape: "dot", text: status.text });
    }

    var sendData = function (node, data) {
        node.log('Sending Message to Azure IoT Hub :\n   Payload: ' + JSON.stringify(data));
        // Create a message and send it to the IoT Hub every second
        var message = new Message(JSON.stringify(data));
        client.sendEvent(message, function (err, res) {
            if (err) {
                node.error('Error while trying to send message:' + err.toString());
                setStatus(node, statusEnum.error);
            } else {
                node.log('Message sent.');
                node.send("Message sent.");
                setStatus(node, statusEnum.sent);
            }
        });
    };

    var sendMessageToIoTHub = function (node, message, reconnect) {
        if (!client || reconnect) {
            node.log('Connection to IoT Hub not established or configuration changed. Reconnecting.');
            // Update the connection string
            clientConnectionString = newConnectionString;
            // update the protocol
            clientProtocol = newProtocol;

            // If client was previously connected, disconnect first
            if (client)
                disconnectFromIoTHub(node);

            // Connect the IoT Hub
            connectToIoTHub(node, message);
        } else {
            sendData(node, message);
        }
    };

    var connectToIoTHub = function (node, pendingMessage) {
        node.log('Connecting to Azure IoT Hub:\n   Protocol: ' + newProtocol + '\n   Connection string :' + newConnectionString);
        client = Client.fromConnectionString(newConnectionString, Protocols[newProtocol]);
        client.open(function (err) {
            if (err) {
                node.error('Could not connect: ' + err.message);
                setStatus(node, statusEnum.disconnected);
            } else {
                node.log('Connected to Azure IoT Hub.');
                setStatus(node, statusEnum.connected);

                // Check if a message is pending and send it 
                if (pendingMessage) {
                    node.log('Message is pending. Sending it to Azure IoT Hub.');
                    // Send the pending message
                    sendData(node, pendingMessage);
                }

                client.on('message', function (msg) {
                    // We received a message
                    node.log('Message received from Azure IoT Hub\n   Id: ' + msg.messageId + '\n   Payload: ' + msg.data);
                    var outpuMessage = new Message();
                    outpuMessage.payload = msg.data;
                    setStatus(node, statusEnum.received);
                    node.send(outpuMessage);
                    client.complete(msg, printResultFor('Completed'));
                });

                client.on('error', function (err) {
                    node.error(err.message);

                });

                client.on('disconnect', function () {
                    disconnectFromIoTHub(node);
                });
            }
        });
    };

    var disconnectFromIoTHub = function (node) {
        if (client) {
            node.log('Disconnecting from Azure IoT Hub');
            client.removeAllListeners();
            client.close(printResultFor('close'));
            client = null;
            setStatus(node, statusEnum.disconnected);
        }
    };

    function nodeConfigUpdated(cs, proto) {
        return ((clientConnectionString != cs) || (clientProtocol != proto));
    }

    // Main function called by Node-RED    
    function AzureIoTHubNode(config) {
        // Store node for further use
        var node = this;
        //nodeConfig = config;

        // Create the Node-RED node
        RED.nodes.createNode(this, config);
        node.on('input', function (msg) {

            var messageJSON = null;

            if (typeof (msg.payload) != "string") {
                node.log("JSON");
                messageJSON = msg.payload;
            } else {
                node.log("String");
                //Converting string to JSON Object
                //Sample string: {"deviceId": "name", "key": "jsadhjahdue7230-=13", "protocol": "amqp", "data": "25"}
                messageJSON = JSON.parse(msg.payload);
            }

            //Creating connectionString
            //Sample
            //HostName=sample.azure-devices.net;DeviceId=sampleDevice;SharedAccessKey=wddU//P8fdfbSBDbIdghZAoSSS5gPhIZREhy3Zcv0JU=
            newConnectionString = "HostName=" + node.credentials.hostname + ";DeviceId=" + messageJSON.deviceId + ";SharedAccessKey=" + messageJSON.key
            newProtocol = messageJSON.protocol;

            // Sending data to Azure IoT Hub Hub using specific connectionString
            sendMessageToIoTHub(node, messageJSON.data, nodeConfigUpdated(newConnectionString, newProtocol));
        });

        node.on('close', function () {
            disconnectFromIoTHub(node, this);
        });

    }

    function IoTHubRegistry(config) {
        RED.nodes.createNode(this, config);

        var node = this;

        node.on('input', function (msg) {
            if (typeof (msg.payload) == 'string') {
                msg.payload = JSON.parse(msg.payload);
            }

            var registry = Registry.fromConnectionString(node.credentials.connectionString);

            registry.create(msg.payload, function (err, device) {
                if (err) {
                    node.error('Error while trying to create a new device: ' + err.toString());
                    setStatus(node, statusEnum.error);
                } else {
                    node.log("Device created: " + JSON.stringify(device));
                    node.log("Device ID: " + device.deviceId + " - primaryKey: " + device.authentication.SymmetricKey.primaryKey + " - secondaryKey: " + device.authentication.SymmetricKey.secondaryKey);
                    node.send("Device ID: " + device.deviceId + " - primaryKey: " + device.authentication.SymmetricKey.primaryKey + " - secondaryKey: " + device.authentication.SymmetricKey.secondaryKey);
                }
            });
        });

        node.on('close', function () {
            disconnectFromIoTHub(node, this);
        });

    }

    // Registration of the node into Node-RED
    RED.nodes.registerType("azureiothub", AzureIoTHubNode, {
        credentials: {
            hostname: { type: "text" }
        },
        defaults: {
            name: { value: "Azure IoT Hub" },
            protocol: { value: "amqp" }
        }
    });

    // Registration of the node into Node-RED
    RED.nodes.registerType("azureiothubregistry", IoTHubRegistry, {
        credentials: {
            connectionString: { type: "text" }
        },
        defaults: {
            name: { value: "Azure IoT Hub Registry" },
        }
    });


    // Helper function to print results in the console
    function printResultFor(op) {
        return function printResult(err, res) {
            if (err) node.error(op + ' error: ' + err.toString());
            if (res) node.log(op + ' status: ' + res.constructor.name);
        };
    }
}