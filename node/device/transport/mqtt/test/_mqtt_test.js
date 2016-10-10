// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

require('es5-shim');
var assert = require('chai').assert;
var sinon = require('sinon');
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

    /* Tests_SRS_NODE_DEVICE_MQTT_18_025: [** If the `Mqtt` constructor receives a second parameter, it shall be used as a provider in place of mqtt. **]**   */
    it ('accepts an mqttProvider for testing', function() {
      var provider = {};
      var mqtt = new Mqtt(fakeConfig, provider);
      assert.equal(mqtt._mqtt.mqttprovider, provider);
    });
  });

  describe('#sendMethodResponse', function() {
    var MockMqttBase = {
      client: {
        publish: function(){}
      }
    };

    // Tests_SRS_NODE_DEVICE_MQTT_13_001: [ sendMethodResponse shall throw an Error if response is falsy or does not conform to the shape defined by DeviceMethodResponse. ]
    [
      // response is falsy
      null,
      // response is falsy
      undefined,
      // response.requestId is falsy
      {},
      // response.requestId is not a string
      { requestId: 42 },
      // response.requestId is an empty string
      { requestId: '' },
      // response.properties is falsy
      { requestId: 'req1' },
      // response.properties is not an object
      {
        requestId: 'req1',
        properties: 42
      },
      // response.properties has empty string keys
      {
        requestId: 'req1',
        properties: {
          '': 'val1'
        }
      },
      // response.properties has non-string values
      {
        requestId: 'req1',
        properties: {
          'k1': 42
        }
      },
      // response.properties has empty string values
      {
        requestId: 'req1',
        properties: {
          'k1': ''
        }
      },
      // response.status is falsy
      {
        requestId: 'req1',
        properties: {
          'k1': 'v1'
        }
      },
      // response.status is not a number
      {
        requestId: 'req1',
        properties: {
          'k1': 'v1'
        },
        status: '200'
      },
      // response.bodyParts is falsy
      {
        requestId: 'req1',
        properties: {
          'k1': 'v1'
        },
        status: 200
      },
      // response.bodyParts.length is falsy
      {
        requestId: 'req1',
        properties: {
          'k1': 'v1'
        },
        status: 200,
        bodyParts: {}
      }
    ].forEach(function(response) {
      it('throws an Error if response is falsy or is improperly constructed', function()
      {
        var mqtt = new Mqtt(fakeConfig);
        assert.throws(function() {
          mqtt.sendMethodResponse(response, null);
        });
      });
    });

    // Tests_SRS_NODE_DEVICE_MQTT_13_002: [ sendMethodResponse shall build an MQTT topic name in the format: $iothub/methods/res/<STATUS>/?$rid=<REQUEST ID>&<PROPERTIES> where <STATUS> is response.status. ]
    // Tests_SRS_NODE_DEVICE_MQTT_13_003: [ sendMethodResponse shall build an MQTT topic name in the format: $iothub/methods/res/<STATUS>/?$rid=<REQUEST ID>&<PROPERTIES> where <REQUEST ID> is response.requestId. ]
    // Tests_SRS_NODE_DEVICE_MQTT_13_004: [ sendMethodResponse shall build an MQTT topic name in the format: $iothub/methods/res/<STATUS>/?$rid=<REQUEST ID>&<PROPERTIES> where <PROPERTIES> is URL encoded. ]
    it('formats MQTT topic with status code', function() {
      // setup
      var mqtt = new Mqtt(fakeConfig);
      var spy = sinon.spy(MockMqttBase.client, 'publish');
      mqtt._mqtt = MockMqttBase;

      // test
      mqtt.sendMethodResponse({
        requestId: 'req1',
        status: 200,
        payload: null
      });

      // assert
      assert.isTrue(spy.calledOnce);
      assert.strictEqual(spy.args[0][0], '$iothub/methods/res/200/?$rid=req1');

      // cleanup
      MockMqttBase.client.publish.restore();
    });

    // Tests_SRS_NODE_DEVICE_MQTT_13_006: [ If the MQTT publish fails then an error shall be returned via the done callback's first parameter. ]
    it('calls callback with error when mqtt publish fails', function() {
      // setup
      var mqtt = new Mqtt(fakeConfig);
      var stub = sinon.stub(MockMqttBase.client, 'publish')
                      .callsArgWith(3, new Error('No connection to broker'));
      var callback = sinon.spy();
      mqtt._mqtt = MockMqttBase;

      // test
      mqtt.sendMethodResponse({
        requestId: 'req1',
        status: 200,
        payload: null
      }, callback);

      // assert
      assert.isTrue(stub.calledOnce);
      assert.isTrue(callback.calledOnce);
      assert.isOk(callback.args[0][0]);

      // cleanup
      MockMqttBase.client.publish.restore();
    });

    // Tests_SRS_NODE_DEVICE_MQTT_13_007: [ If the MQTT publish is successful then the done callback shall be invoked passing null for the first parameter. ]
    it('calls callback with null when mqtt publish succeeds', function() {
      // setup
      var mqtt = new Mqtt(fakeConfig);
      var stub = sinon.stub(MockMqttBase.client, 'publish')
                      .callsArgWith(3, null);
      var callback = sinon.spy();
      mqtt._mqtt = MockMqttBase;

      // test
      mqtt.sendMethodResponse({
        requestId: 'req1',
        status: 200,
        payload: null
      }, callback);

      // assert
      assert.isTrue(stub.calledOnce);
      assert.isTrue(callback.calledOnce);
      assert.isNotOk(callback.args[0][0]);

      // cleanup
      MockMqttBase.client.publish.restore();
    });
    
    /*Tests_SRS_NODE_DEVICE_MQTT_16_016: [The `Mqtt` constructor shall initialize the `uri` property of the `config` object to `mqtts://<host>`.]*/
    it('sets the uri property to \'mqtts://<host>\'', function () {
      var mqtt = new Mqtt(fakeConfig);
      assert.strictEqual(mqtt._config.uri, 'mqtts://' + fakeConfig.host);
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
