// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Message = require('azure-iot-common').Message;
var SharedAccessSignature = require('./shared_access_signature.js');

function makeError(errorMessage) {
    var err = new Error(errorMessage);
    return err;
}

function SimulatedMqtt(config) {
    this.handleRequest = function(done) {
        var sig = SharedAccessSignature.parse(config.sharedAccessSignature);

        if (config.host === 'bad') {                      // bad host
            done(new Error('Invalid host address'));
        }
        else if (config.deviceId === 'bad') {             // bad policy
            done(makeError('Connection Refused'));
        }
        else if (config.gatewayHostName === 'bad') {      // bad gateway url
            done(makeError('Invalid gateway address'));
        }
        else {
            var cmpSig = (SharedAccessSignature.create(config.host, config.deviceId, 'bad', sig.se)).toString();
            if (config.sharedAccessSignature === cmpSig) {  // bad key
                done(makeError('Connection Refused'));
            }
            else {
                done(null, 'OK');
            }
        }
    };
}

SimulatedMqtt.prototype.connect = function (done) {
    this.handleRequest(function (err, response) {
        done(err, response);
    });
};

SimulatedMqtt.prototype.sendEvent = function (message, done) {
    this.handleRequest(function (err, response) {
        done(err, response);
    });
};

SimulatedMqtt.prototype.receive = function (done) {
    this.handleRequest(function (err, response) {
        done(err, err ? null : new Message(''), response);
    });
};

module.exports = SimulatedMqtt;