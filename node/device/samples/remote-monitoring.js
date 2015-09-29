// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');

// Settings specific to an instance of IoT Hub
var deviceID = "[DeviceID]";
var deviceKey = "[Device Key]";
var hubName = "[IoT Hub name]";
var hubSuffix = "[IoT Hub suffix i.e azure-devices.net]";

// Sensors data
var temperature = 50;
var humidity = 50;
var externalTemperature = 55;

// Create IoT Hub client
var connectionString = "HostName="+hubName+"."+hubSuffix+";CredentialScope=Device;DeviceId="+deviceID+";SharedAccessKey="+deviceKey;
var client = new device.Client(connectionString, new device.Https());

// Helper function to print results for an operation
function printResultFor(op) {
  return function printResult(err, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res) console.log(op + ' status: ' + res.statusCode + ' ' + res.statusMessage);
  };
}

// Helper function to generate random number between min and max
function generateRandomIncrement()
{
  return ((Math.random() * 2)-1);
}

// function to send telemetry data
function sendTelemetryData(){
  temperature += generateRandomIncrement();
  externalTemperature += generateRandomIncrement();
  humidity += generateRandomIncrement();
  var data = JSON.stringify({ "DeviceID":deviceID, "Temperature":temperature, "Humidity":humidity, "ExternalTemperature":externalTemperature });
  console.log("Sending device telemetry data:\n"+data);
  client.sendEvent(new device.Message(data), printResultFor('send'));
}

// function to wait on notifications
var isWaiting = false;
function waitForNotifications(){
  isWaiting = true;
  client.receive(function (err, res, msg) {
    printResultFor('receive')(err, res);
    if (!err && res.statusCode !== 204) {
      console.log('receive data: ' + msg.getData());
      var command = JSON.parse(msg.getData());
      if (command.Name === "SetTemperature")
      {
        temperature = command.Parameters.Temperature;
        console.log("New temperature set to :" + temperature + "F");
      }

      client.complete(msg, printResultFor('complete'));
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
    "DeviceID":deviceID,
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
  "Commands": [{"Name":"SetTemperature","Parameters":[{"Name":"Temperature","Type":"double"}]},
               {"Name":"SetHumidity","Parameters":[{"Name":"Humidity","Type":"double"}]}]
};
console.log("Sending device metadata:\n"+JSON.stringify(deviceMetaData));
client.sendEvent(new device.Message(JSON.stringify(deviceMetaData)), printResultFor('send'));

// start telemetry data send routing
setInterval(sendTelemetryData, 1000);

// Start notifications listener
setInterval(function(){ if (!isWaiting) waitForNotifications();}, 200);
