// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
'use strict';

var Registry = require('azure-iothub').Registry;
var Client = require('azure-iothub').Client;
var async = require('async');

// receive the IoT Hub connection string as a command line parameter
if(process.argv.length < 4) {
  console.error('Usage: node dmpatterns_reboot_service.js <<IoT Hub Connection String>> <<targetDeviceId>>');
  process.exit(1);
}

var connectionString = process.argv[2];
var registry = Registry.fromConnectionString(connectionString);
var client = Client.fromConnectionString(connectionString);
var deviceToReboot = process.argv[3];

// Initiate the reboot process on the device using a device method
async.waterfall([
  invokeReboot,
  displayRebootStatus 
],
function(err) {
  if (err){
    console.error(err);
  } else {
    console.log('Reboot complete');
  } 
});

// Initiate the reboot through a method
function invokeReboot(callback) {
  client.invokeDeviceMethod(deviceToReboot, 
    {
      methodName: "reboot",
      payload: null,
      timeoutInSeconds: 30
    }, function (err, result) {
      console.log(JSON.stringify(result, null, 2));
      callback(err);
    }
  );
}
 
// Get the twin and output the reboot status from reported properties
function displayRebootStatus(callback) {
  registry.getTwin(deviceToReboot, function(err, twin){
    if (err) { 
      callback(err);
    }
    else {
      // Output the value of twin reported properties, which includes the reboot details
      console.log(twin.properties.reported);
      callback(null);
    }
  });
}

