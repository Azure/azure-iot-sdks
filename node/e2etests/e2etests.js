// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var serviceSdk = require('azure-iothub');
var deviceSdk = require('azure-iot-device');
var deviceAmqp = require('azure-iot-device-amqp');
var deviceHttp = require('azure-iot-device-http');

var ConnectionString = require('azure-iot-common').ConnectionString;

var assert = require('chai').assert;
var debug = require('debug')('e2etests');
var uuid = require('uuid');

var runTests = function (DeviceTransport, hubConnStr, deviceConStr, deviceName) {
    describe('Device connected over ' + DeviceTransport.name + ':', function () {
        
        var serviceClient, deviceClient;
        
        beforeEach(function(){
            serviceClient =  serviceSdk.Client.fromConnectionString(hubConnStr);
            deviceClient = deviceSdk.Client.fromConnectionString(deviceConStr, DeviceTransport);  
        });
        
        afterEach(function(){
            serviceClient = null;
            deviceClient = null;
        });
        
        it('Service sends 1 C2D message and it is re-sent until the device completes it', function(done)
        {
            this.timeout(15000);
            var guid = uuid.v4();
            var closeConnectionsAndTerminateTest = function () {
               serviceClient.close(function () {
                  debug('Connection to the service closed.');
               });
               deviceClient.close(function() {
                   debug('Connection as the device closed.');
               });
               
               done(); 
            };
            
            var abandonnedOnce = false;
            deviceClient.open(function (openErr, openRes) {
                deviceClient.getReceiver(function (err, receiver) {
                    assert.isNull(err);
                    receiver.on('errorReceived', function (err) {
                       assert.Fail('Device received an error: ' + err.message); 
                    });
                    receiver.on('message', function (msg) {
                        debug('Received a message with guid: ' + msg.data);
                        if (msg.data === guid) {
                            if(!abandonnedOnce) {
                                debug('Abandon the message with guid ' + msg.data);
                                abandonnedOnce = true;
                                receiver.abandon(msg);
                            } else {
                                debug('Complete the message with guid ' + msg.data);                            
                                receiver.complete(msg, function (err, res){
                                    assert.isNull(err);
                                    closeConnectionsAndTerminateTest();
                                });
                            }
                        }
                    });
                });
            });
            
            serviceClient.open(function () {
                serviceClient.send(deviceName, guid, function (sendErr, sendRes) {
                    assert.isNull(sendErr);
                    debug('Sent one message with guid: ' + guid);
                });
            });
        });
        
        it.skip('Service sends 1 C2D message and it is re-sent until the device rejects it', function(done)
        {
            this.timeout(15000);
            var guid = uuid.v4();
            var closeConnectionsAndTerminateTest = function () {
               serviceClient.close(function () {
                  debug('Connection to the service closed.');
               });
               deviceClient.close(function() {
                   debug('Connection as the device closed.');
               });
               
               done(); 
            };
            
            var abandonnedOnce = false;
            deviceClient.open(function (openErr, openRes) {
                deviceClient.getReceiver(function (err, receiver) {
                    assert.isNull(err);
                    receiver.on('errorReceived', function (err) {
                       assert.Fail('Device received an error: ' + err.message); 
                    });
                    receiver.on('message', function (msg) {
                        debug('Received a message with guid: ' + msg.data);
                        if (msg.data === guid) {
                            if(!abandonnedOnce) {
                                debug('Abandon the message with guid ' + msg.data);
                                abandonnedOnce = true;
                                receiver.abandon(msg);
                            } else {
                                debug('Rejects the message with guid ' + msg.data);                            
                                receiver.reject(msg, function (err, res) {
                                    assert.isNull(err);
                                    closeConnectionsAndTerminateTest();
                                });
                            }
                        } else {
                            debug('not the message I\'m looking for, abandoning it (' + msg.data + ')');
                            receiver.abandon(msg);
                        }
                    });
                });
            });
            
            serviceClient.open(function () {
                serviceClient.send(deviceName, guid, function (sendErr, sendRes) {
                    assert.isNull(sendErr);
                    debug('Sent one message with guid: ' + guid);
                });
            });
        });
        
        it('Service sends 5 C2D messages and they are received by the device', function (done) {
            this.timeout(15000);
            var deviceMessageCounter = 0;
            
            var closeConnectionsAndTerminateTest = function () {
               serviceClient.close(function () {
                  debug('Connection to the service closed.');
               });
               deviceClient.close(function() {
                   debug('Connection as the device closed.');
               });
               
               done(); 
            };
            
            deviceClient.open(function (openErr, openRes) {
                deviceClient.getReceiver(function (err, receiver) {
                    assert.isNull(err);
                    receiver.on('errorReceived', function (err) {
                       assert.Fail('Device received an error: ' + err.message); 
                    });
                    receiver.on('message', function (msg) {
                        deviceMessageCounter++;
                        debug('Received ' + deviceMessageCounter + ' message(s)');
                        receiver.complete(msg);
                        
                        if(deviceMessageCounter === 5) {
                            closeConnectionsAndTerminateTest();
                        }
                    });
                });
            });
            
            serviceClient.open(function () {
                var msgSentCounter = 0;
                for (var i = 0; i < 5; i++) {
                    debug('Sending message #' + i);
                    serviceClient.send(deviceName, { 'counter' : i }, function (sendErr, sendRes) {
                        assert.isNull(sendErr);
                        msgSentCounter++;
                        debug('Sent ' + msgSentCounter + ' message(s)');
                    });
                }
            });
        });
    });
};

module.exports = runTests;

var hubConnectionString = process.env.IOTHUB_CONNECTION_STRING;
var host = ConnectionString.parse(process.env.IOTHUB_CONNECTION_STRING).HostName;
var deviceConnectionString = 'HostName=' + host + ';DeviceId=' + process.env.IOTHUB_DEVICE_ID + ';SharedAccessKey=' + process.env.IOTHUB_DEVICE_KEY;
var deviceName = process.env.IOTHUB_DEVICE_ID;

runTests(deviceAmqp.Amqp, hubConnectionString, deviceConnectionString, deviceName);
runTests(deviceHttp.Http, hubConnectionString, deviceConnectionString, deviceName);
