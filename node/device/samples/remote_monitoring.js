// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');

// String containing Hostname, Device Id & Device Key in the following formats:
//  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
var connectionString = "[IoT Hub device connection string]";
var deviceId = device.ConnectionString.parse(connectionString).DeviceId;

// Sensors data
var temperature = 50;
var humidity = 50;
var externalTemperature = 55;

// Create IoT Hub client
var client = device.Client.fromConnectionString(connectionString);

// Helper function to print results for an operation
function printErrorFor(op) {
  return function printError(err) {
    if (err) console.log(op + ' error: ' + err.toString());
  };
}

// Helper function to generate random number between min and max
function generateRandomIncrement() {
  return ((Math.random() * 2)-1);
}

// function to send event data
function sendEventData() {
  temperature += generateRandomIncrement();
  externalTemperature += generateRandomIncrement();
  humidity += generateRandomIncrement();

  var data = JSON.stringify({
    "DeviceID":deviceId,
    "Temperature":temperature,
    "Humidity":humidity,
    "ExternalTemperature":externalTemperature
  });

  console.log("Sending device event data:\n"+data);
  client.sendEvent(new device.Message(data), printErrorFor('send event'));
}

// function to wait on messages
var isWaiting = false;
function waitForMessages() {
  isWaiting = true;
  client.receive(function (err, msg, res) {
    printErrorFor('receive')(err, res);
    if (!err && res.statusCode !== 204) {
      console.log('receive data: ' + msg.getData());
      try {
        var command = JSON.parse(msg.getData());
        if (command.Name === "SetTemperature") {
          temperature = command.Parameters.Temperature;
          console.log("New temperature set to :" + temperature + "F");
        }

        client.complete(msg, printErrorFor('complete'));
      }
      catch (err) {
        printErrorFor('parse received message')(err);
        client.reject(msg, printErrorFor('reject'));
      }
    }
    isWaiting = false;
  });
}

// Send device meta data
var deviceMetaData = {
  "ObjectType":"DeviceInfo",
  "IsSimulatedDevice":0,
  "Version":"1.0",
  "DeviceProperties": {
    "DeviceID":deviceId,
    "HubEnabledState":1,
    "CreatedTime":"2015-09-21T20:28:55.5448990Z",
    "DeviceState":"normal",
    "UpdatedTime":null,
    "Manufacturer":"Contoso Inc.",
    "ModelNumber":"MD-909",
    "SerialNumber":"SER9090",
    "FirmwareVersion":"1.10",
    "Platform":"node.js",
    "Processor":"ARM",
    "InstalledRAM":"64 MB",
    "Latitude":47.617025,
    "Longitude":-122.191285
  },
  "Commands": [{
    "Name": "SetTemperature",
    "Parameters": [{
      "Name": "Temperature",
      "Type": "double"
    }]
  },
  {
    "Name": "SetHumidity",
    "Parameters": [{
      "Name": "Humidity",
      "Type": "double"
    }]
  }]
};

console.log("Sending device metadata:\n" + JSON.stringify(deviceMetaData));
client.sendEvent(new device.Message(JSON.stringify(deviceMetaData)), printErrorFor('send metadata'));

// start event data send routing
setInterval(sendEventData, 1000);

// Start messages listener
setInterval(function () {
  if (!isWaiting) waitForMessages();
}, 200);
