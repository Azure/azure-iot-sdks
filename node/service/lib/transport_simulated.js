// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var errors = require('azure-iot-common').errors;

function SimulatedTransport() {}

SimulatedTransport.prototype.connect = function connect(done) {
  if (!!done) done();
};

SimulatedTransport.prototype.disconnect = function disconnect(done) {
  if (!!done) done();
};

SimulatedTransport.prototype.send = function send(deviceId, message, done) {
  if (done) {
    if (deviceId.search(/^no-device/) !== -1) {
      done(new errors.DeviceNotFoundError());
    }
    else {
      done(null, { descriptor: 0x24 });
    }
  }
};

module.exports = SimulatedTransport;
