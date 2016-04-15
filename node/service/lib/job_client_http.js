// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Base = require('azure-iot-http-base').Http;

function Http() {
    this._http = new Base();
}

Http.prototype.sendHttpRequest = function (verb, path, headers, host, writeData, done) {
    var request = this._http.buildRequest(verb, path, headers, host, function (err, body, response) {
        if (!err) {
             done(null, body, response);
        } else {
            err.response = response;
            err.responseBody = body;
            done(err);
        }
    });
    if (writeData) {
        request.write(writeData);
    }
    request.end();
};

module.exports = Http;