// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var Mqtt = require('../lib/mqtt.js');
var errors = require('azure-iot-common').errors;

describe('Mqtt', function () {
  var fakeConfig = {
    host: 'host.name',
    deviceId: 'deviceId'
  };

  describe('#constructor', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_12_001: [The `Mqtt` constructor shall accept the transport configuration structure */
    /* Tests_SRS_NODE_DEVICE_MQTT_12_002: [The `Mqtt` constructor shall store the configuration structure in a member variable */
    it('stores config and created transport in member', function () {
      var mqtt = new Mqtt(fakeConfig);
      assert.notEqual(mqtt, null);
      assert.notEqual(mqtt, undefined);
      assert.equal(mqtt._config, fakeConfig);
    });
  });

  describe('#setOptions', function() {
    var fakeX509Options = { cert: 'cert', key: 'key'};

    /*Tests_SRS_NODE_DEVICE_MQTT_16_011: [The `setOptions` method shall throw a `ReferenceError` if the `options` argument is falsy]*/
    [null, undefined].forEach(function(badOptions) {
      it('throws a ReferenceError if the `options` argument is \'' + badOptions + '\'', function() {
        var mqtt = new Mqtt(fakeConfig);
        assert.throws(function() {
          mqtt.setOptions(badOptions);
        }, ReferenceError);
      });
    });

    /*Tests_SRS_NODE_DEVICE_MQTT_16_015: [The `setOptions` method shall throw an `ArgumentError` if the `cert` property is populated but the device uses symmetric key authentication.]*/
    it('throws an ArgumentError if the options.cert property is set but the device is using symmetric key authentication', function() {
      var mqtt = new Mqtt({
        host: 'host.name',
        deviceId: 'deviceId',
        sharedAccessSignature: 'sharedAccessSignature'
      });
      assert.throws(function() {
        mqtt.setOptions(fakeX509Options);
      }, errors.ArgumentError);
    });

    /*Tests_SRS_NODE_DEVICE_MQTT_16_012: [The `setOptions` method shall update the existing configuration of the MQTT transport with the content of the `options` object.]*/
    it('updates the existing configuration with new options', function() {
      var mqtt = new Mqtt(fakeConfig);
      mqtt.setOptions(fakeX509Options);
      assert.equal(mqtt._config.x509.cert, fakeX509Options.cert);
      assert.equal(mqtt._config.x509.key, fakeX509Options.key);
    });

    /*Tests_SRS_NODE_DEVICE_MQTT_16_013: [If a `done` callback function is passed as a argument, the `setOptions` method shall call it when finished with no arguments.]*/
    it('calls the `done` callback with no arguments when finished', function(done){
      var mqtt = new Mqtt(fakeConfig);
      mqtt.setOptions(fakeX509Options, done);
    });

    /*Tests_SRS_NODE_DEVICE_MQTT_16_014: [The `setOptions` method shall not throw if the `done` argument is not passed.]*/
    it('doesn\'t throw if `done` is not passed in the arguments', function() {
      var mqtt = new Mqtt(fakeConfig);
      assert.doesNotThrow(function() {
        mqtt.setOptions(fakeX509Options);
      });
    });
  });
});
