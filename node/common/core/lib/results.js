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
function MessageEnqueued (transportObj) {
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

module.exports = {
    MessageEnqueued: MessageEnqueued,  
    MessageCompleted: MessageCompleted,  
    MessageRejected: MessageRejected,  
    MessageAbandoned: MessageAbandoned,  
};