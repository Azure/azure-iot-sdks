// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var serviceSdk = require('azure-iothub');
var deviceSdk = require('azure-iot-device');
var deviceAmqp = require('azure-iot-device-amqp');
var deviceHttp = require('azure-iot-device-http');
var Message = require('azure-iot-common').Message;

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
        
        var closeConnectionsAndTerminateTest = function (done) {
           serviceClient.close(function (err) {
             if(err) {
               done(err);
             } else {
               serviceClient = null;
               debug('Connection to the service closed.');
               deviceClient.close(function(err) {
                 if(err) {
                   done(err);
                 } else {
                   deviceClient = null;
                   debug('Connection as the device closed.');
                   done(); 
                 }
               });
             }
           });
        };
        
        it('Service sends 1 C2D message and it is re-sent until the device completes it', function(done)
        {
            this.timeout(60000);
            var guid = uuid.v4();
            
            var abandonnedOnce = false;
            deviceClient.open(function (openErr) {
                if(openErr) {
                    done(openErr);
                } else {
                    deviceClient.on('message', function (msg){
                        debug('Received a message with guid: ' + msg.data);
                        if (msg.data === guid) {
                            if(!abandonnedOnce) {
                                debug('Abandon the message with guid ' + msg.data);
                                abandonnedOnce = true;
                                deviceClient.abandon(msg, function (err, result){
                                    assert.isNull(err);
                                    assert.equal(result.constructor.name, 'MessageAbandoned');
                                });
                            } else {
                                debug('Complete the message with guid ' + msg.data);                            
                                deviceClient.complete(msg, function (err, res){
                                    assert.isNull(err);
                                    assert.equal(res.constructor.name, 'MessageCompleted');
                                    closeConnectionsAndTerminateTest(done);
                                });
                            }
                        } else {
                            debug('not the message I\'m looking for, completing it to clean the queue (' + msg.data + ')');
                            deviceClient.complete(msg, function (err, result){
                                assert.isNull(err);
                                assert.equal(result.constructor.name, 'MessageCompleted');
                            });
                        }
                    });
                }
            });
            
            serviceClient.open(function (serviceErr) {
                if(serviceErr) {
                    done(serviceErr);
                } else {
                    serviceClient.send(deviceName, guid, function (sendErr) {
                        assert.isNull(sendErr);
                        debug('Sent one message with guid: ' + guid);
                    });
                }
            });
        });
        
        it('Service sends 1 C2D message and it is re-sent until the device rejects it', function(done)
        {
            this.timeout(60000);
            var guid = uuid.v4();
            
            var abandonnedOnce = false;
            deviceClient.open(function (openErr) {
                if(openErr) {
                    done(openErr);
                } else {
                    deviceClient.on('message', function (msg) {
                        debug('Received a message with guid: ' + msg.data);
                        if (msg.data === guid) {
                            if(!abandonnedOnce) {
                                debug('Abandon the message with guid ' + msg.data);
                                abandonnedOnce = true;
                                deviceClient.abandon(msg, function (err, result){
                                    assert.isNull(err);
                                    assert.equal(result.constructor.name, 'MessageAbandoned');
                                });
                            } else {
                                debug('Rejects the message with guid ' + msg.data);                            
                                deviceClient.reject(msg, function (err, res) {
                                    assert.isNull(err);
                                    assert.equal(res.constructor.name, 'MessageRejected');
                                    closeConnectionsAndTerminateTest(done);
                                });
                            }
                        } else {
                            debug('not the message I\'m looking for, completing it to clean the queue (' + msg.data + ')');
                            deviceClient.complete(msg, function (err, result){
                                assert.isNull(err);
                                assert.equal(result.constructor.name, 'MessageCompleted');
                            });
                        }
                    });
                }
            });
            
            serviceClient.open(function (serviceErr) {
                if(serviceErr) {
                    done(serviceErr);
                } else {
                    serviceClient.send(deviceName, guid, function (sendErr) {
                        assert.isNull(sendErr);
                        debug('Sent one message with guid: ' + guid);
                    });
                }
            });
        });
        
        it('Service sends 5 C2D messages and they are all received by the device', function (done) {
            this.timeout(60000);
            var deviceMessageCounter = 0;
            
            deviceClient.open(function (openErr) {
                if(openErr) {
                    done(openErr);
                } else {
                    deviceClient.on('message', function (msg) {
                        deviceMessageCounter++;
                        debug('Received ' + deviceMessageCounter + ' message(s)');
                        deviceClient.complete(msg, function (err, result){
                            assert.isNull(err);
                            assert.equal(result.constructor.name, 'MessageCompleted');
                        });
                        
                        if(deviceMessageCounter === 5) {
                            closeConnectionsAndTerminateTest(done);
                        }
                    });
                }
            });
            
            serviceClient.open(function (serviceErr) {
                if(serviceErr) {
                    done(serviceErr);
                } else {
                    var msgSentCounter = 0;
                    var sendCallback = function (sendErr) {
                        assert.isNull(sendErr);
                        msgSentCounter++;
                        debug('Sent ' + msgSentCounter + ' message(s)');
                    };
                    
                    for (var i = 0; i < 5; i++) {
                        debug('Sending message #' + i);
                        var msg = new Message({ 'counter' : i });
                        msg.expiryTimeUtc = Date.now() + 10000; // Expire 10s from now, to reduce the chance of us hitting the 50-message limit on the IoT Hub
                        serviceClient.send(deviceName, msg, sendCallback);
                    }
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
