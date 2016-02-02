// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/**
 * The `azure-iot-mqtt-base` module contains MQTT support code common to the Azure IoT Hub Device and Service SDKs.
 * 
 * @module azure-iot-mqtt-base
 */

module.exports = {
  Mqtt: require('./lib/mqtt.js'),
  MqttReceiver: require('./lib/mqtt_receiver.js')
};