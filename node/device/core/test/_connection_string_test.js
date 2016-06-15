// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var ConnectionString = require('../lib/connection_string.js');

var incompleteConnectionStrings = {
  HostName: 'DeviceId=id;SharedAccessKey=key;GatewayHostName=gateway',
  DeviceId: 'HostName=name;SharedAccessKey=key;GatewayHostName=gateway'
};

var invalidConnectionStrings = {
  BothSharedAccessKeyAndx509:'DeviceId=id;HostName=name;SharedAccessKey=key;x509=true',
  NeitherSharedAccessKeyNorx509: 'DeviceId=id;HostName=name'
};

describe('ConnectionString', function () {
  describe('#parse', function () {
    /*Tests_SRS_NODE_DEVICE_CONNSTR_05_001: [The parse method shall return the result of calling azure-iot-common.ConnectionString.parse.]*/
    /*Tests_SRS_NODE_DEVICE_CONNSTR_05_002: [It shall throw ArgumentError if any of 'HostName', 'DeviceId' fields are not found in the source argument.]*/
    ['HostName', 'DeviceId'].forEach(function (key) {
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

    /*Codes_SRS_NODE_DEVICE_CONNSTR_16_001: [It shall throw `ArgumentError` if `SharedAccessKey` and `x509` are present at the same time or if none of them are present.]*/
    ['BothSharedAccessKeyAndx509', 'NeitherSharedAccessKeyNorx509'].forEach(function(key) {
      it('throws if the connection string is invalid because ' + key, function() {
        assert.throws(function() {
          ConnectionString.parse(invalidConnectionStrings[key]);
        }, ArgumentError);
      });
    });

    it('does not throw if x509 is true and SharedAccessKey is not present', function() {
      assert.doesNotThrow(function() {
        ConnectionString.parse('HostName=name;DeviceId=id;x509=true');
      }, ArgumentError);
    });
  });
});
