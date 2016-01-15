// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var Client = require('azure-iot-device').Client;
var Message = require('azure-iot-device').Message;
var SimulatedMqtt = require('./mqtt_simulated.js');
var Mqtt = require('./mqtt.js');

describe('Mqtt', function () {
  describe('#constructor', function () {
    /* Tests_SRS_NODE_DEVICE_HTTP_12_001: [The Mqtt shall accept the transport configuration structure */
    /* Tests_SRS_NODE_DEVICE_HTTP_12_002: [The Mqtt shall store the configuration structure in a member variable */
    /* Tests_SRS_NODE_DEVICE_HTTP_12_003: [The Mqtt shall create an Mqtt object and store it in a member variable */
    it('stores config and created transport in member', function () {
      var config = {
        host: "host.name",
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
  return 'HostName=' + host + ';DeviceId=' + device + ';SharedAccessKey=' + key + ';GatewayHostName=' + gateway;
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
