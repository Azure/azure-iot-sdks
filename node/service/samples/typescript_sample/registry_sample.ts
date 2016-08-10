// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Registry, Device } from 'azure-iothub';

const connectionString = '[IoT Connection String]';

let registry = Registry.fromConnectionString(connectionString);


// List devices
console.log('**listing devices..');
registry.list((err, deviceList) => {
    deviceList.forEach((device) => {
        let key = device.authentication ? device.authentication.SymmetricKey.primaryKey : '<no primary key>';
        console.log(device.deviceId + ': ' + key);
    });
});

// Create a new device
let device = new Device();
device.deviceId = 'sample-device-' + Date.now();
console.log('\n**creating device \'' + device.deviceId + '\'');
registry.create(device, printAndContinue('create', () => {

    // Get the newly-create device
    console.log('\n**getting device \'' + device.deviceId + '\'');
    registry.get(device.deviceId, printAndContinue('get', () => {

        // Delete the new device
        console.log('\n**deleting device \'' + device.deviceId + '\'');
        registry.delete(device.deviceId, printAndContinue('delete'));
    }));
}));

function printAndContinue(op: string, next?: () => void): Registry.ResponseCallback {
    return (err, deviceInfo, res) => {
        if (err) console.log(op + ' error: ' + err.toString());
        if (res) console.log(op + ' status: ' + res.statusCode + ' ' + res.statusMessage);
        if (deviceInfo) console.log(op + ' device info: ' + JSON.stringify(deviceInfo));
        if (next) next();
    };
}
