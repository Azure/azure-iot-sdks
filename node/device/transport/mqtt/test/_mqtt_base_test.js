// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var MqttBase = require('../lib/mqtt_base.js');
var PackageJson = require('../package.json');
var FakeMqtt = require('./_fake_mqtt.js');
var Message = require('azure-iot-common').Message;
var endpoint = require('azure-iot-common').endpoint;

describe('MqttBase', function () {
  describe('#connect', function() {
    /*Tests_SRS_NODE_COMMON_MQTT_BASE_16_006: [The `connect` method shall throw a ReferenceError if the config argument is falsy, or if one of the following properties of the config argument is falsy: deviceId, host, and one of sharedAccessSignature or x509.cert and x509.key.]*/
    it('throws if config structure is falsy', function () {
      [null, undefined, '', 0].forEach(function (config) {
        var mqtt = new MqttBase(new FakeMqtt());
        assert.throws(function () {
          mqtt.connect(config);
        }, ReferenceError, 'Invalid transport configuration');
      });
    });

    it('throws if host is falsy', function () {
      [null, undefined].forEach(function (hostname) {
        var mqtt = new MqttBase(new FakeMqtt());
        var config = {
          host: hostname,
          deviceId: "deviceId",
          sharedAccessSignature: "sasToken"
        };
        assert.throws(function () {
          mqtt.connect(config);
        }, ReferenceError, 'Invalid transport configuration');
      });
    });

    it('throws if deviceId is falsy', function () {
      [null, undefined].forEach(function (deviceId) {
        var mqtt = new MqttBase(new FakeMqtt());
        var config = {
          host: "host.name",
          deviceId: deviceId,
          sharedAccessSignature: "sasToken"
        };
        assert.throws(function () {
          mqtt.connect(config);
        }, ReferenceError, 'Invalid transport configuration');
      });
    });

    [null, undefined].forEach(function (sas) {
      [
        null,
        undefined,
        { cert: null, key: 'key' },
        { cert: undefined, key: 'key' },
        { cert: '', key: 'key' },
        { cert: 'cert', key: null },
        { cert: 'cert', key: undefined },
        { cert: 'cert', key: '' }
      ].forEach(function (x509) {
        it('throws if sharedAccessSignature is \'' + sas + '\' and x509 options are \'' + JSON.stringify(x509) + '\'', function () {
          var mqtt = new MqttBase(new FakeMqtt());
          var config = {
            host: "host.name",
            deviceId: "deviceId",
            sharedAccessSignature: sas,
            x509: x509
          };
          assert.throws(function () {
            mqtt.connect(config);
          }, ReferenceError, 'Invalid transport configuration');
        });
      });
    });

    /*Tests_SRS_NODE_COMMON_MQTT_BASE_16_002: [The `connect` method shall use the authentication parameters contained in the `config` argument to connect to the server.]*/
    it('uses the authentication parameters contained in the config structure (SharedAccessSignature)', function () {
      var config = {
        host: "host.name",
        deviceId: "deviceId",
        sharedAccessSignature: "sasToken"
      };

      var fakemqtt = new FakeMqtt();
      var transport = new MqttBase(fakemqtt);

      fakemqtt.connect = function(host, options) {
        assert.equal(options.clientId, config.deviceId);
        assert.equal(options.username, config.host + '/' +
                                       config.deviceId +
                                       '/DeviceClientType=' +
                                       encodeURIComponent(
                                         'azure-iot-device/' + PackageJson.version
                                       ) +
                                       '&' + endpoint.versionQueryString().substr(1));
        assert.equal(options.password, config.sharedAccessSignature);

        assert.equal(options.cmd, 'connect');
        assert.equal(options.protocolId, 'MQTT');
        assert.equal(options.protocolVersion, '4');
        assert.equal(options.clean, false);
        assert.equal(options.rejectUnauthorized, true);
      };

      transport.connect(config);
    });

    it('uses the authentication parameters contained in the config structure (x509)', function () {
      var config = {
        host: "host.name",
        deviceId: "deviceId",
        x509: {
          cert: 'cert',
          key: 'key',
          passphrase: 'passphrase'
        }
      };

      var fakemqtt = new FakeMqtt();
      var transport = new MqttBase(fakemqtt);

      fakemqtt.connect = function(host, options) {
        assert.equal(options.clientId, config.deviceId);
        assert.equal(options.username, config.host + '/' +
                                       config.deviceId +
                                       '/DeviceClientType=' +
                                       encodeURIComponent(
                                         'azure-iot-device/' + PackageJson.version
                                       ) +
                                       '&' + endpoint.versionQueryString().substr(1));
        assert.equal(options.cert, config.x509.cert);
        assert.equal(options.key, config.x509.key);
        assert.equal(options.passphrase, config.x509.passphrase);

        assert.equal(options.cmd, 'connect');
        assert.equal(options.protocolId, 'MQTT');
        assert.equal(options.protocolVersion, '4');
        assert.equal(options.clean, false);
        assert.equal(options.rejectUnauthorized, true);
      };

      transport.connect(config);
    });

    it('uses mqtts as a protocol by default', function () {
      var config = {
        host: "host.name",
        deviceId: "deviceId",
        sharedAccessSignature: "sasToken"
      };

      var fakemqtt = new FakeMqtt();
      var transport = new MqttBase(fakemqtt);

      fakemqtt.connect = function(host) {
        assert.strictEqual(host, 'mqtts://' + config.host);
      };

      transport.connect(config);
    });

    it('uses the uri specified by the config object', function () {
      var config = {
        host: "host.name",
        deviceId: "deviceId",
        sharedAccessSignature: "sasToken",
        uri: 'wss://host.name:443/$iothub/websocket'
      };

      var fakemqtt = new FakeMqtt();
      var transport = new MqttBase(fakemqtt);

      fakemqtt.connect = function(host) {
        assert.strictEqual(host, config.uri);
      };

      transport.connect(config);
    });

    /*Tests_SRS_NODE_COMMON_MQTT_BASE_12_005: [The `connect` method shall call connect on MQTT.JS  library and call the `done` callback with a `null` error object and the result as a second argument.]*/
    it('calls the done callback once successfully connected to the server', function(done) {
      var config = {
        host: "host.name",
        deviceId: "deviceId",
        sharedAccessSignature: "sasToken"
      };

      var fakemqtt = new FakeMqtt();
      var transport = new MqttBase(fakemqtt);
      transport.connect(config, function(err) {
        if(err) {
          done(err);
        } else {
          done();
        }
      });

      fakemqtt.emit('connect', { connack: true });
    });

    /*Tests_SRS_NODE_COMMON_MQTT_BASE_16_003: [The `connect` method shall call the `done` callback with a standard javascript `Error` object if the connection failed.]*/
    ['close', 'offline', 'error', 'disconnect'].forEach(function(event) {
      it('calls the done callback with an error if we get the \'' + event + '\' event before connect', function(done) {
        var config = {
          host: "host.name",
          deviceId: "deviceId",
          sharedAccessSignature: "sasToken"
        };

        var fakemqtt = new FakeMqtt();
        var transport = new MqttBase(fakemqtt);
        transport.connect(config, function(err) {
          assert.isNotNull(err);
          done();
        });

        fakemqtt.emit(event, new Error('could not connect'));
      });
    });
    /*Tests_SRS_NODE_COMMON_MQTT_BASE_16_007: [The `connect` method shall not throw if the `done` argument has not been passed.]*/
    it('does not throw if the `done` argument is undefined', function() {
      var mqtt = new MqttBase(new FakeMqtt());
      assert.doesNotThrow(function() {
        mqtt.connect({
          host: "host.name",
          deviceId: "deviceId",
          sharedAccessSignature: "sasToken"
        });
      });
    });
  });

  describe('#publish', function () {
    /* Tests_SRS_NODE_HTTP_12_006: The PUBLISH method shall throw ReferenceError “Invalid message” if the message is falsy */
    it('throws if message is falsy', function () {
      [null, undefined].forEach(function (message) {
        var config = {
          host: "host.name",
          deviceId: "deviceId",
          sharedAccessSignature: "sasToken"
        };
        var transport = new MqttBase(config);
        assert.throws(function () {
          transport.publish(message);
        }, ReferenceError, 'Invalid message');
      });
    });

    it('calls publish on the MQTT library', function(done) {
      var config = {
        host: "host.name",
        deviceId: "deviceId",
        sharedAccessSignature: "sasToken"
      };

      var fakemqtt = new FakeMqtt();
      var transport = new MqttBase(fakemqtt);
      transport.connect(config, function () {
        transport.client.publishShouldSucceed(true);
        transport.publish(new Message('message'), function(err, result) {
          if(err) {
            done (err);
          } else {
            assert.equal(result.constructor.name, 'MessageEnqueued');
            done();
          }
        });
      });
      fakemqtt.emit('connect', { connack: true });
    });

    // Publish errors are handled with a callback, so 'error' should be subscribed only once when connecting, to get link errors.
    it('does not subscribe to the error event', function (done) {
      var config = {
        host: "host.name",
        deviceId: "deviceId",
        sharedAccessSignature: "sasToken"
      };

      var fakemqtt = new FakeMqtt();
      var transport = new MqttBase(fakemqtt);
      transport.connect(config, function () {
        assert.equal(transport.client.listeners('error').length, 1);
        transport.client.publishShouldSucceed(false);
        transport.publish(new Message('message'), function() {
          assert.equal(transport.client.listeners('error').length, 1);
          done();
        });
      });

      fakemqtt.emit('connect', { connack: true });
    });
  });
}); 