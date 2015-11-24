// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var SharedAccessSignature = require('azure-iot-common').SharedAccessSignature;

function Response(statusCode) {
  this.statusCode = statusCode;
}

function createError() {
  var err = new Error();
  err.response = new Response(401);
  return err;
}

function SimulatedHttp(config) {
  this.handleRequest = function (done, body) {
    var sig = SharedAccessSignature.parse(config.sharedAccessSignature);

    if (config.host === 'bad') {                      // bad host
      done(new Error('getaddrinfo ENOTFOUND bad'));
    }
    else if (sig.skn === 'bad') {                     // bad policy
      done(createError());
    }
    else {
      var cmpSig = SharedAccessSignature.create(sig.sr, sig.skn, 'bad', sig.se).toString();
      if (config.sharedAccessSignature === cmpSig) {  // bad key
        done(createError());
      }
      else {                                          // ok
        done(null, body, new Response(204));
      }
    }
  };
}

SimulatedHttp.prototype.createDevice = function (path, deviceInfo, done) {
  var device = { deviceId: deviceInfo.deviceId };
  this.handleRequest(done, JSON.stringify(device));
};

SimulatedHttp.prototype.updateDevice = function (path, deviceInfo, done) {
  var device = { deviceId: deviceInfo.deviceId };
  this.handleRequest(done, JSON.stringify(device));
};

SimulatedHttp.prototype.getDevice = function (path, done) {
  var device = { deviceId: 'testDevice' };
  this.handleRequest(done, JSON.stringify(device));
};

SimulatedHttp.prototype.listDevices = function (path, done) {
  var devices = [
    { deviceId: 'testDevice1' },
    { deviceId: 'testDevice2' }
  ];
  this.handleRequest(done, JSON.stringify(devices));
};

SimulatedHttp.prototype.deleteDevice = function (path, done) {
  var device = { deviceId: 'testDevice' };
  this.handleRequest(done, JSON.stringify(device));
};

module.exports = SimulatedHttp;