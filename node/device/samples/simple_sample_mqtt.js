// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var mqtt = require('azure-iot-device').Mqtt;
var device = require('azure-iot-device');

var connectionString = '[IoT Hub device connection string]';

var client = new device.Client.fromConnectionString(connectionString, mqtt);

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
  client.receive(function (topic, msg) {
      console.log('Received data: ' + msg.toString());
    }
  );
});

