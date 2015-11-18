// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');

var connectionString = '[IoT Connection String]';

var registry = iothub.Registry.fromConnectionString(connectionString);

// List devices
console.log('**listing devices...');
registry.list(function (err, deviceList) {
  deviceList.forEach(function (device) {
    var key = device.authentication ? device.authentication.SymmetricKey.primaryKey : '<no primary key>';
    console.log(device.deviceId + ': ' + key);
  });

  // Create a new device
  var device = new iothub.Device(null);
  device.deviceId = 'sample-device-' + Date.now();
  console.log('\n**creating device \'' + device.deviceId + '\'');
  registry.create(device, printAndContinue('create', function next() {

    // Get the newly-created device
    console.log('\n**getting device \'' + device.deviceId + '\'');
    registry.get(device.deviceId, printAndContinue('get', function next() {

      // Delete the new device
      console.log('\n**deleting device \'' + device.deviceId + '\'');
      registry.delete(device.deviceId, printAndContinue('delete'));
    }));
  }));
});

function printAndContinue(op, next) {
  return function printResult(err, deviceInfo, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res) console.log(op + ' status: ' + res.statusCode + ' ' + res.statusMessage);
    if (deviceInfo) console.log(op + ' device info: ' + JSON.stringify(deviceInfo));
    if (next) next();
  };
}