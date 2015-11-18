// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Base = require('azure-iot-common').SharedAccessSignature;

module.exports = {
  parse: function parse(source) {
    /*Codes_SRS_NODE_DEVICE_SAS_05_001: [The parse method shall return the result of calling azure-iot-common.SharedAccessSignature.parse.]*/
    /*Codes_SRS_NODE_DEVICE_SAS_05_002: [It shall throw ArgumentError if any of 'sr', 'sig', 'se' fields are not found in the source argument.]*/
    return Base.parse(source, ['sr', 'sig', 'se']);
  } 
};