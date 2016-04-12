// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var iothub = require('azure-iothub');

var connectionString = '[IoT Connection String]';

var registry = iothub.Registry.fromConnectionString(connectionString);

console.log('**listing devices...');

var tags = ['tag1', 'tag2', 'tag3'];
registry.queryDevices(tags, 13, printResult);
function printResult(err, deviceList, res) {
    if (err) console.log('error ' + err.responseBody);
    if (res) console.log('status: ' + res.statusCode + ' ' + res.statusMessage);
    
    if (deviceList) {
        deviceList.forEach(function(device) {
            console.log('device info: ' + device.deviceId);
            device.serviceProperties.tags.forEach(function(tag) {
                console.log('    tag: '+tag);
            });
        });
    }
    else{
        console.log('No device info');
    }
}