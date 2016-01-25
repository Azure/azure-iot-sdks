// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

function MessageAccepted (transportObj) {
    this.transportObj = transportObj;
}

MessageAccepted.prototype = Object.create(Object);
MessageAccepted.prototype.constructor = MessageAccepted;
MessageAccepted.prototype.toString = function () {
    return this.constructor.name;
}

module.exports = {
    MessageAccepted: MessageAccepted  
};