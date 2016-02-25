// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var ConnectionString = require('../lib/connection_string.js');

var incompleteConnectionStrings = {
  HostName: 'SharedAccessKeyName=keyname;SharedAccessKey=key',
  SharedAccessKeyName: 'HostName=hostname;SharedAccessKey=key',
  SharedAccessKey: 'HostName=name;SharedAccessKeyName=keyname'
};

describe('ConnectionString', function () {
  describe('#parse', function () {
    /*Tests_SRS_NODE_IOTHUB_CONNSTR_05_001: [The parse method shall return the result of calling azure-iot-common.ConnectionString.parse.]*/
    /*Tests_SRS_NODE_IOTHUB_CONNSTR_05_002: [It shall throw ArgumentError if any of 'HostName', 'SharedAccessKeyName', or 'SharedAccessKey' fields are not found in the source argument.]*/
    ['HostName', 'SharedAccessKeyName', 'SharedAccessKey'].forEach(function (key) {
      it('throws if connection string is missing ' + key, function () {
        assert.throws(function () {
          ConnectionString.parse(incompleteConnectionStrings[key]);
        }, ArgumentError);
      });
    });
  });
});
