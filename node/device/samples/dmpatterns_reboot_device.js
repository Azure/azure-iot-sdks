// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
'use strict';

var Client = require('azure-iot-device').Client;
var Protocol = require('azure-iot-device-mqtt').Mqtt;

// receive the IoT Hub device connection string as a command line parameter
if(process.argv.length < 3) {
  console.error('Usage: node dmpatterns_getstarted_device.js <<IoT Hub Device Connection String>>');
  process.exit(1);
}

var connectionString = process.argv[2];
var client = Client.fromConnectionString(connectionString, Protocol);

client.open(function(err) {
  if (!err) {
    client.onDeviceMethod('reboot', function onReboot(request, response) {
      response.send(200, 'Reboot started', function(err) {
        if (err) {
          console.error('An error occured when sending a method response:\n' + err.toString());
        } else {
          console.log('Response to method \'' + request.methodName + '\' sent successfully.');
        }
      });
      
      // Get device Twin
      client.getTwin(function(err, twin) {
        if (err) {
          console.error('could not get twin');
        } else {
          console.log('twin acquired');
          
          // Update the reported properties for this device through the 
          // twin.  This enables the back end app to query for all device that
          // have completed a reboot based on the lastReboot property.
          twin.properties.reported.update({
            iothubDM : {
              reboot : {
                startedRebootTime : new Date().toISOString(),
              }
            }
          }, function(err) {
            if (err) console.error('Error updating twin');
            else console.log('Device reboot twin state reported')
          });  
        }
      });
      
      // Add your device's reboot API for physical restart.
      console.log('Rebooting!');      
    });
    console.log('Client connected to IoT Hub.  Waiting for reboot device method.');
  } else {
    console.error(err);
  }
});
