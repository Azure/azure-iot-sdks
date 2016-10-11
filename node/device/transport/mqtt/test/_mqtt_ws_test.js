// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var MqttWs = require('../lib/mqtt_ws.js');

describe('MqttWs', function () {
  var fakeConfig = {
    host: 'host.name',
    deviceId: 'deviceId'
  };

  describe('#constructor', function () {
    /* Tests_SRS_NODE_DEVICE_MQTT_12_001: [The `Mqtt` constructor shall accept the transport configuration structure */
    /* Tests_SRS_NODE_DEVICE_MQTT_12_002: [The `Mqtt` constructor shall store the configuration structure in a member variable */
    it('stores config and created transport in member', function () {
      var mqttWs = new MqttWs(fakeConfig);
      assert.notEqual(mqttWs, null);
      assert.notEqual(mqttWs, undefined);
      assert.equal(mqttWs._config, fakeConfig);
    });

    /*Tests_SRS_NODE_DEVICE_MQTT_16_017: [The `MqttWs` constructor shall initialize the `uri` property of the `config` object to `wss://<host>:443/$iothub/websocket`.]*/
    it('sets the uri property to \'wss://<host>:443/$iothub/websocket\'', function () {
      var mqttWs = new MqttWs(fakeConfig);
      assert.equal(mqttWs._config.uri, 'wss://' + fakeConfig.host + ':443/$iothub/websocket');
    });
  });
});
