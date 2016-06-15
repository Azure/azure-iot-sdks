// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Base = require('azure-iot-common').ConnectionString;
var errors = require('azure-iot-common').errors;

module.exports = {
  parse: function parse(source) {
    /*Codes_SRS_NODE_DEVICE_CONNSTR_05_001: [The parse method shall return the result of calling azure-iot-common.ConnectionString.parse.]*/
    /*Codes_SRS_NODE_DEVICE_CONNSTR_05_002: [It shall throw ArgumentError if any of 'HostName' or 'DeviceId' fields are not found in the source argument.]*/
    var connectionString = Base.parse(source, ['HostName', 'DeviceId']);
    /*Codes_SRS_NODE_DEVICE_CONNSTR_16_001: [It shall throw `ArgumentError` if `SharedAccessKey` and `x509` are present at the same time or if none of them are present.]*/
    if((connectionString.SharedAccessKey && connectionString.x509) || (!connectionString.SharedAccessKey && !connectionString.x509)) {
      throw new errors.ArgumentError('The connection string must contain either a SharedAccessKey or x509=true');
    }

    return connectionString;
  }
};
