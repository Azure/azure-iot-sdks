// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var Mqtt = require('./mqtt.js');

describe('Mqtt', function () {
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
          return new Mqtt(config);
        }, ReferenceError, 'Invalid transport configuration');
      });
    });
    it('throws if host is falsy', function () {
      [null, undefined].forEach(function (hostname) {
        var config = {
          host: hostname,
          deviceId: "deviceId",
          sharedAccessSignature: "sasToken",
          gatewayHostName: "gatewayHostName"
        };
        assert.throws(function () {
          return new Mqtt(config);
        }, ReferenceError, 'Invalid transport configuration');
      });
    });
    it('throws if deviceId is falsy', function () {
      [null, undefined].forEach(function (deviceId) {
        var config = {
          host: "host.name",
          deviceId: deviceId,
          sharedAccessSignature: "sasToken",
          gatewayHostName: "gatewayHostName"
        };
        assert.throws(function () {
          return new Mqtt(config);
        }, ReferenceError, 'Invalid transport configuration');
      });
    });
    it('throws if sasToken is falsy', function () {
      [null, undefined].forEach(function (sasToken) {
        var config = {
          host: "host.name",
          deviceId: "deviceId",
          sharedAccessSignature: sasToken,
          gatewayHostName: "gatewayHostName"
        };
        assert.throws(function () {
          return new Mqtt(config);
        }, ReferenceError, 'Invalid transport configuration');
      });
    });
    it('throws if gatewayHostName is falsy', function () {
      [null, undefined].forEach(function (gatewayHostName) {
        var config = {
          host: 'host.name',
          deviceId: "deviceId",
          sharedAccessSignature: "sasToken",
          gatewayHostName: gatewayHostName
        };
        assert.throws(function () {
          return new Mqtt(config);
        }, ReferenceError, 'Invalid transport configuration');
      });
    });
    /* Tests_SRS_NODE_HTTP_12_003: [Mqtt shall create a configuration structure for underlying MQTT.JS library and store it to a member variable */
    /* Tests_SRS_NODE_HTTP_12_004: [Mqtt shall return an instance itself */
    it('create options structure with config content and return itself', function () {
      var config = {
        host: "host.name",
        deviceId: "deviceId",
        sharedAccessSignature: "sasToken",
        gatewayHostName: "gatewayHostName"
      };
      var transport = new Mqtt(config);
      assert.notEqual(transport, null);
      assert.notEqual(transport, 'undefined');
      assert.equal(transport._options.clientId, config.deviceId);
      assert.equal(transport._options.username, config.host + '/' + config.deviceId);
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
          host: "host.name",
          deviceId: "deviceId",
          sharedAccessSignature: "sasToken",
          gatewayHostName: "gatewayHostName"
        };
        var transport = new Mqtt(config);
        assert.throws(function () {
          transport.publish(message);
        }, ReferenceError, 'Invalid message');
      });
    });
  });
});
