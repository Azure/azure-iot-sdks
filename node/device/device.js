// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var common = require('_common');

module.exports = {
    Client: require('./lib/client.js'),
    Https: common.Https,
    Message: common.Message
};