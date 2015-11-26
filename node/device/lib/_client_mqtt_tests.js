// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var Client = require('./client.js');
var Message = require('azure-iot-common').Message;
var SimulatedMqtt = require('./mqtt_simulated.js');
var Mqtt_common = require('azure-iot-common').Mqtt;
var Mqtt = require('./mqtt.js');

describe('Mqtt-Common', function () {
    describe('#constructor', function () {
        /* Tests_SRS_NODE_HTTP_12_001: Mqtt shall accept the following argument:
            config [
                host: host address
                deviceID: device name
                sharedAccessSignature: SAS token  created for IoTHub
                gatewayHostName: gateway host name]
            Tests_SRS_NODE_HTTP_12_002: [Mqtt shall throw ReferenceError “Invalid transport configuration” error if either of the configuration field is falsy
        */
        it('throws if config structure is falsy', function () {
            [null, undefined, '', 0].forEach(function (config) {
                assert.throws(function () {
                    return new Mqtt_common(config);
                }, ReferenceError, 'Invalid transport configuration');
            });
        });
        it('throws if deviceId is falsy', function () {
            [null, undefined].forEach(function (deviceId) {
                var config = {
                    deviceId: deviceId,
                    sharedAccessSignature: "sasToken",
                    gatewayHostName: "gatewayHostName"
                };
                assert.throws(function () {
                    return new Mqtt_common(config);
                }, ReferenceError, 'Invalid transport configuration');
            });
        });
        it('throws if sasToken is falsy', function () {
            [null, undefined].forEach(function (sasToken) {
                var config = {
                    deviceId: "deviceId",
                    sharedAccessSignature: sasToken,
                    gatewayHostName: "gatewayHostName"
                };
                assert.throws(function () {
                    return new Mqtt_common(config);
                }, ReferenceError, 'Invalid transport configuration');
            });
        });
        it('throws if gatewayHostName is falsy', function () {
            [null, undefined].forEach(function (gatewayHostName) {
                var config = {
                    deviceId: "deviceId",
                    sharedAccessSignature: "sasToken",
                    gatewayHostName: gatewayHostName
                };
                assert.throws(function () {
                    return new Mqtt_common(config);
                }, ReferenceError, 'Invalid transport configuration');
            });
        });
        /* Tests_SRS_NODE_HTTP_12_003: [Mqtt shall create a configuration structure for underlying MQTT.JS library and store it to a member variable */
        /* Tests_SRS_NODE_HTTP_12_004: [Mqtt shall return an instance itself */
        it('create options structure with config content and return itself', function () {
            var config = {
                deviceId: "deviceId",
                sharedAccessSignature: "sasToken",
                gatewayHostName: "gatewayHostName"
            };
            var transport = new Mqtt_common(config);
            assert.notEqual(transport, null);
            assert.notEqual(transport, 'undefined');
            assert.equal(transport._options.clientId, config.deviceId);
            assert.equal(transport._options.username, config.deviceId);
            assert.equal(transport._options.password, config.sharedAccessSignature);

            assert.equal(transport._options.cmd, 'connect');
            assert.equal(transport._options.protocolId, 'MQTT');
            assert.equal(transport._options.protocolVersion, '4');
            assert.equal(transport._options.clean, false);
            assert.equal(transport._options.rejectUnauthorized, false);
        });
    });
    describe('#publish', function () {
        /* Tests_SRS_NODE_HTTP_12_006: The PUBLISH method shall throw ReferenceError “Invalid message” if the message is falsy */
        it('throws if message is falsy', function () {
            [null, undefined].forEach(function (message) {
                var config = {
                    deviceId: "deviceId",
                    sharedAccessSignature: "sasToken",
                    gatewayHostName: "gatewayHostName"
                };
                var transport = new Mqtt_common(config);
                assert.throws(function () {
                    transport.publish(message);
                }, ReferenceError, 'Invalid message');
            });
        });
    });
});

describe('Mqtt', function () {
    describe('#constructor', function () {
        /* Tests_SRS_NODE_DEVICE_HTTP_12_001: [The Mqtt shall accept the transport configuration structure */
        /* Tests_SRS_NODE_DEVICE_HTTP_12_002: [The Mqtt shall store the configuration structure in a member variable */
        /* Tests_SRS_NODE_DEVICE_HTTP_12_003: [The Mqtt shall create an Mqtt object and store it in a member variable */
        it('stores config and created transport in member', function () {
            var config = {
                deviceId: "deviceId",
                sharedAccessSignature: "sasToken",
                gatewayHostName: "gatewayHostName"
            };
            var mqtt_obj = new Mqtt(config);
            assert.notEqual(mqtt_obj, null);
            assert.notEqual(mqtt_obj, 'undefined');
            assert.equal(mqtt_obj._config, config);
            assert.notEqual(mqtt_obj._mqtt, null);
            assert.notEqual(mqtt_obj._mqtt, 'undefined');
        });
    });
});

function badConfigTests(opName, badConnStrings, Transport, requestFn) {

    function makeRequestWith(connectionString, test, done) {
        var client = Client.fromConnectionString(connectionString, Transport);
        requestFn(client, function (err, res) {
            test(err, res);
            done();
        });
    }

    function expectHostNotFoundError(err) {
        assert.equal(err.message, 'Invalid host address');
    }

    function expectGatewayNotFoundError(err) {
        assert.equal(err.message, 'Invalid gateway address');
    }

    function expectRefused(err) {
        assert.isNotNull(err);
        assert.equal(err.message, 'Connection Refused');
    }

    var tests = [
        { name: 'hostname is malformed', expect: expectHostNotFoundError },
        { name: 'device is not registered', expect: expectRefused },
        { name: 'password is wrong', expect: expectRefused },
        { name: 'gatewayurl is malformed', expect: expectGatewayNotFoundError },
    ];

    /*Tests_SRS_NODE_DEVICE_CLIENT_05_016: [When a Client method encounters an error in the transport, the callback function (indicated by the done argument) shall be invoked with the following arguments:
     err - the standard JavaScript Error object, with a response property that points to a transport-specific response object, and a responseBody property that contains the body of the transport response.]*/
    badConnStrings.forEach(function (test, index) {
        it('fails to ' + opName + ' when the ' + tests[index].name, function (done) {
            makeRequestWith(test, tests[index].expect, done);
        });
    });
}

function runTests(Transport, goodConnectionString, badConnectionStrings) {

    describe('Client', function () {
        describe('#sendEvent', function () {
            /*Tests_SRS_NODE_DEVICE_CLIENT_05_007: [The sendEvent method shall send the event indicated by the message argument via the transport associated with the Client instance.]*/
            /*Tests_SRS_NODE_DEVICE_CLIENT_05_017: [With the exception of receive, when a Client method completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
             err - null
             response - a transport-specific response object]*/
            it('sends the event', function (done) {
                var client = Client.fromConnectionString(goodConnectionString, Transport);
                var message = new Message('hello');
                client.sendEvent(message, function (err, res) {
                    assert.isNull(err);
                    assert.equal(res, 'OK');
                    done();
                });
            });

            badConfigTests('send an event', badConnectionStrings, Transport, function (client, done) {
                client.sendEvent(new Message(''), done);
            });

        });

        describe('#receive', function () {
            /*Tests_SRS_NODE_DEVICE_CLIENT_05_009: [The receive method shall query the IoT Hub for the next message via the transport associated with the Client instance.]*/
            /*Tests_SRS_NODE_DEVICE_CLIENT_05_018: [When receive completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
             err - null
             message - the received Message (for receive), otherwise null
             response - a transport-specific response object]*/
            it('queries the service for a message', function (done) {
                var client = Client.fromConnectionString(goodConnectionString, Transport);
                client.receive(function (err, msg, res) {
                    assert.isNull(err);
                    assert.equal(res, 'OK');
                    assert.instanceOf(msg, Message);
                    done();
                });
            });

            badConfigTests('receive messages', badConnectionStrings, Transport, function (client, done) {
                client.receive(function (err, message, response) {
                    done(err, response);
                });
            });
        });
    });
}

function makeConnectionString(host, device, key, gateway) {
    return 'HostName='+host+';DeviceId='+device+';SharedAccessKey='+key+';GatewayHostName='+gateway;
}

var connectionString = makeConnectionString('host', 'device', 'key', 'gateway');
var badConnStrings = [
    makeConnectionString('bad', 'device', 'key', 'gateway'),
    makeConnectionString('host', 'bad', 'key', 'gateway'),
    makeConnectionString('host', 'device', 'bad', 'gateway'),
    makeConnectionString('host', 'device', 'key', 'bad'),
];

describe('Over simulated MQTT', function () {
    runTests(SimulatedMqtt, connectionString, badConnStrings);
});

module.exports = runTests;
