// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var clientFromConnectionString = require('azure-iot-device-mqtt').clientFromConnectionString;
var Message = require('azure-iot-device').Message;

// String containing hostname, device ID, device key in the following format:
var connectionString = '[IoT Hub device connection string]';

var client = clientFromConnectionString(connectionString);

client.open(function () {
  // Create a message and send it to the IoT Hub every second
  setInterval(function () {
    var windSpeed = 10 + (Math.random() * 4); // range: [10, 14]
    var data = JSON.stringify({
      deviceId: 'myFirstDevice',
      windSpeed: windSpeed
    });
    var message = new Message(data);
    message.properties.add('myproperty', 'myvalue');
    console.log('Sending message: ' + message.getData());
    client.sendEvent(message, function (err, result){
        console.log('Message sent: ' + result.constructor.name + '(' + JSON.stringify(result.transportObj) + ')');
    });
  }, 2000);
  
  // Monitor messages from IoT Hub and print them in the console.
  client.getReceiver(function(err, receiver){
      receiver.on('message', function(msg){
          console.log('Message received: ' + msg.data);
      });
  });
});

