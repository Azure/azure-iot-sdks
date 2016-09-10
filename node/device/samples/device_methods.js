// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

require('es5-shim');
var Protocol = require('azure-iot-device-mqtt').Mqtt;
var Client = require('azure-iot-device').Client;

var client = null;

function main() {
    // receive the IoT Hub connection string as a command line parameter
    if(process.argv.length < 3) {
        console.error('Usage: node device_methods.js <<IoT Hub Device Connection String>>');
        process.exit(1);
    }

    // open a connection to the device
    var deviceConnectionString = process.argv[2];
    client = Client.fromConnectionString(deviceConnectionString, Protocol);
    client.open(onConnect);
}

function onConnect(err) {
    if(!!err) {
        console.error('Could not connect: ' + err.message);
    } else {
        console.log('Connected to device. Registering handlers for methods.');

        // register handlers for all the method names we are interested in
        client.onDeviceMethod('reboot', onReboot);
        client.onDeviceMethod('updateFirmware', onUpdateFirmware);
    }
}

function onReboot(request, response) {
    printDeviceMethodRequest(request);

    // Implement actual device reboot logic here.

    // add some properties to the response
    response.properties = {
        'RebootTimeInMS': '8500',
        'DeviceStatus': 'Ready'
    };

    // add a payload to the response
    response.write('Boot log line 1');
    response.write('Boot log line 2');

    // complete the response
    response.end(200, function(err) {
        if(!!err) {
            console.error('An error ocurred when sending a method response:\n' +
                err.toString());
        } else {
            console.log('Response to method \'' + request.methodName +
                '\' sent successfully.' );
        }
    });
}

function onUpdateFirmware(request, response) {
    printDeviceMethodRequest(request);

    // Implement actual device firmware update logic here.

    // add some properties to the response
    response.properties = {
        'NewFirmwareVersion': '1.5.0'
    };

    // complete the response
    response.end(200, function(err) {
        if(!!err) {
            console.error('An error ocurred when sending a method response:\n' +
                err.toString());
        } else {
            console.log('Response to method \'' + request.methodName +
                '\' sent successfully.' );
        }
    });
}

function printDeviceMethodRequest(request) {
    // print method name
    console.log('Received method call for method \'' + request.methodName + '\'');

    // print method properties received
    console.log('Request properties:');
    for (var key in request.properties) {
        if (request.properties.hasOwnProperty(key)) {
            console.log('  ' + key + ' = ' + request.properties[key]);
        }
    }

    // if there's a payload just do a default console log on it
    if(!!(request.body)) {
        console.log('Body:\n' + request.body);
    }
}

// get the app rolling
main();