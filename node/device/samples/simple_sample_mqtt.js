// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var mqtt = require('azure-iot-device').Mqtt;
var device = require('azure-iot-device');

// String containing Hostname, Device Id & Device Key in the following formats:
//  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
var connectionString = '[IoT Hub device connection string]';

var client = device.Client.fromConnectionString(connectionString, mqtt);

client.open(function (){
  // Create a message and send it to the IoT Hub every second
  setInterval(function(){
    var windSpeed = 10 + (Math.random() * 4); // range: [10, 14]
    var data = JSON.stringify({
      deviceId: 'myFirstDevice',
      windSpeed: windSpeed
    });
    var message = new device.Message(data);
    message.properties.add('myproperty', 'myvalue');
    console.log("Sending message: " + message.getData());
    client.sendEvent(message);
  }, 1000);
  
  // Monitor messages from IoT Hub and print them in the console.
  client.receive(function (err, msg) {
    if (err) {
      console.error(err);
    } else {
      console.log('Received data: ' + msg.toString());
    }
  });
});

