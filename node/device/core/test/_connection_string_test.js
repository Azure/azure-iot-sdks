// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var ConnectionString = require('../lib/connection_string.js');

var incompleteConnectionStrings = {
  HostName: 'DeviceId=id;SharedAccessKey=key;GatewayHostName=gateway',
  DeviceId: 'HostName=name;SharedAccessKey=key;GatewayHostName=gateway',
  SharedAccessKey: 'HostName=name;DeviceId=id;GatewayHostName=gateway'
};

describe('ConnectionString', function () {
  describe('#parse', function () {
    /*Tests_SRS_NODE_DEVICE_CONNSTR_05_001: [The parse method shall return the result of calling azure-iot-common.ConnectionString.parse.]*/
    /*Tests_SRS_NODE_DEVICE_CONNSTR_05_002: [It shall throw ArgumentError if any of 'HostName', 'DeviceId', or 'SharedAccessKey' fields are not found in the source argument.]*/
    ['HostName', 'DeviceId', 'SharedAccessKey'].forEach(function (key) {
      it('throws if connection string is missing ' + key, function () {
        assert.throws(function () {
          ConnectionString.parse(incompleteConnectionStrings[key]);
        }, ArgumentError);
      });
    });

    it('does not throw if shared access signature is missing GatewayHostName', function () {
      assert.doesNotThrow(function () {
        ConnectionString.parse('HostName=name;DeviceId=id;SharedAccessKey=key');
      }, ArgumentError);
    });
  });
});
