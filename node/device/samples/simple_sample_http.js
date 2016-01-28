// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var clientFromConnectionString = require('azure-iot-device-http').clientFromConnectionString;
var Message = require('azure-iot-device').Message;

// String containing Hostname, Device Id & Device Key in the following formats:
//  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
var connectionString = '[IoT Hub device connection string]';

var client = clientFromConnectionString(connectionString);

// Create a message and send it to the IoT hub every second
setInterval(function () {
  var windSpeed = 10 + (Math.random() * 4); // range: [10, 14]
  var data = JSON.stringify({
    deviceId: 'myFirstDevice',
    windSpeed: windSpeed
  });
  var message = new Message(data);
  message.properties.add('myproperty', 'myvalue');
  console.log('Sending message: ' + message.getData());
  client.sendEvent(message, printResultFor('send'));
}, 1000);

// Monitor messages from IoT Hub and print them in the console.
//  Note: In this sample, the polling interval is set to 1 second
//  to enable you to see messages as they are sent. To enable an
//  IoT solution to scale, you should extend this interval. For
//  example, to scale to 1 million devices, set the polling
//  interval to 25 minutes. For further information, see
//  https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
client.getReceiver(function(err, receiver) {
  // Example of use of the 'interval' option:
  receiver.setOptions({ interval: 1, at: null, cron: null, drain: true });
  
  // Example of use of the 'at' option:
  // var date = new Date(new Date().getTime()+ 1000);
  // receiver.setOptions({ interval: null, at: date, cron: null, drain: true });
  
  // Example of use of the 'cron' option:
  //receiver.setOptions({ interval: null, at: null, cron: "* * * * *", drain: true });
  receiver.on('message', function(msg) { 
    console.log('Message: ' + msg.data); 
    receiver.complete(msg, printResultFor('complete'));
    // abandon and reject obey the same pattern.
  });
  
  receiver.on('errorReceived', printResultFor('error'));  
});

// Helper function to print results in the console
function printResultFor(op) {
  return function printResult(err, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res) console.log(op + ' status: ' + res.constructor.name + ' (' + res.transportObj.statusCode + ': ' + res.transportObj.statusMessage + ')');
  };
}
