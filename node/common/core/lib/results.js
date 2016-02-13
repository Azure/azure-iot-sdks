// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/**
 * @class       module:azure-iot-common.MessageEnqueued
 * @classdesc   Result returned when a message was successfully enqueued.
 *
 * @property {Object} transportObj  The transport-specific object
 *
 * @augments {Object}
 */
function MessageEnqueued(transportObj) {
  this.transportObj = transportObj;
}

/**
 * @class       module:azure-iot-common.MessageCompleted
 * @classdesc   Result returned when a message was successfully rejected.
 *
 * @property {Object} transportObj  The transport-specific object
 *
 * @augments {Object}
 */
function MessageCompleted(transportObj) {
  this.transportObj = transportObj;
}

/**
 * @class       module:azure-iot-common.MessageRejected
 * @classdesc   Result returned when a message was successfully rejected.
 *
 * @property {Object} transportObj  The transport-specific object
 *
 * @augments {Object}
 */
function MessageRejected(transportObj) {
  this.transportObj = transportObj;
}

/**
 * @class       module:azure-iot-common.MessageAbandoned
 * @classdesc   Result returned when a message was successfully abandoned.
 *
 * @property {Object} transportObj  The transport-specific object
 *
 * @augments {Object}
 */
function MessageAbandoned(transportObj) {
  this.transportObj = transportObj;
}

/**
 * @class       module:azure-iot-common.Connected
 * @classdesc   Result returned when a transport is successfully connected.
 *
 * @property {Object} transportObj  The transport-specific object
 *
 * @augments {Object}
 */
function Connected(transportObj) {
  this.transportObj = transportObj;
}

/**
 * @class       module:azure-iot-common.Disconnected
 * @classdesc   Result returned when a transport is successfully disconnected.
 *
 * @property {Object} transportObj  The transport-specific object.
 * @property {String} reason        The reason why the disconnected event is emitted.
 *
 * @augments {Object}
 */
function Disconnected(transportObj, reason) {
  this.transportObj = transportObj;
  this.reason = reason;
}

/**
 * @class       module:azure-iot-common.TransportConfigured
 * @classdesc   Result returned when a transport is successfully configured.
 *
 * @property {Object} transportObj  The transport-specific object.
 *
 * @augments {Object}
 */
function TransportConfigured(transportObj) {
  this.transportObj = transportObj;
}

/**
 * @class       module:azure-iot-common.SharedAccessSignatureUpdated
 * @classdesc   Result returned when a SAS token has been successfully updated.
 *
 * @param {Boolean} needToReconnect  Value indicating whether the client needs to reconnect or not.
 *
 * @augments {Object}
 */
function SharedAccessSignatureUpdated(needToReconnect) {
  this.needToReconnect = needToReconnect;
}

module.exports = {
  MessageEnqueued: MessageEnqueued,
  MessageCompleted: MessageCompleted,
  MessageRejected: MessageRejected,
  MessageAbandoned: MessageAbandoned,
  Connected: Connected,
  Disconnected: Disconnected,
  TransportConfigured: TransportConfigured,
  SharedAccessSignatureUpdated: SharedAccessSignatureUpdated
};