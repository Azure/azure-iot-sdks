"use strict";

var Registry = require('azure-iothub').Registry;

var connectionString = "[IoT Hub Connection String]";
var deviceId = '[Device ID]';

var registry = Registry.fromConnectionString(connectionString);
registry.getDeviceTwin(deviceId, function(err, twin) {
  if (err) {
    console.error(err.message);
  } else {
    console.log(JSON.stringify(JSON.parse(twin), null, 2));
  }
});