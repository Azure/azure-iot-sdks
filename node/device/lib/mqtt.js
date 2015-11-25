// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Mqtt_common = require('azure-iot-common').Mqtt;

/*
 Codes_SRS_NODE_DEVICE_HTTP_12_001: [The Mqtt  shall accept the transport configuration structure
 Codes_SRS_NODE_DEVICE_HTTP_12_002: [The Mqtt  shall store the configuration structure in a member variable
 Codes_SRS_NODE_DEVICE_HTTP_12_003: [The Mqtt  shall create an MqttTransport object and store it in a member variable
*/
function Mqtt(config) {
    this._config = config;
    this._mqtt = new Mqtt_common(config);
}

/* Codes_SRS_NODE_DEVICE_HTTP_12_004: [The connect method shall call the connect method on MqttTransport */
Mqtt.prototype.connect = function (done) {
    this._mqtt.connect(function (){
        if (done) done();
    });
};

/* Codes_SRS_NODE_DEVICE_HTTP_12_005: [The sendEvent method shall call the publish method on MqttTransport */
Mqtt.prototype.sendEvent = function (message) {
    this._mqtt.publish(message);
};

/*
 Codes_SRS_NODE_DEVICE_HTTP_12_006: [The receive method shall subscribe on MqttTransport
 Codes_SRS_NODE_DEVICE_HTTP_12_007: [The receive method shall receive on MqttTransport
*/
Mqtt.prototype.receive = function (done) {
    this._mqtt.subscribe();
    this._mqtt.receive(done);
};

module.exports = Mqtt;