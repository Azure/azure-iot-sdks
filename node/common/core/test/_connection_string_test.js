// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('../lib/errors.js').ArgumentError;
var ConnectionString = require('../lib/connection_string.js');

describe('ConnectionString', function () {
  describe('#parse', function () {
    /*Tests_SRS_NODE_CONNSTR_05_005: [The parse method shall throw ArgumentError if any of fields in the requiredFields argument are not found in the source argument.]*/
    it('throws if connection string is missing a required field', function () {
      assert.throws(function () {
        ConnectionString.parse('one=abc;two=123', ['one', 'two', 'three']);
      }, ArgumentError, 'The connection string is missing the property: three');
    });

    /*Tests_SRS_NODE_CONNSTR_05_002: [The parse method shall create a new instance of ConnectionString.]*/
    /*Tests_SRS_NODE_CONNSTR_05_003: [It shall accept a string argument of the form 'name=value[;name=value…]' and for each name extracted it shall create a new property on the ConnectionString object instance.]*/
    /*Tests_SRS_NODE_CONNSTR_05_004: [The value of the property shall be the value extracted from the source argument for the corresponding name.]*/
    /*Tests_SRS_NODE_CONNSTR_05_006: [The generated ConnectionString object shall be returned to the caller.]*/
    /*Tests_SRS_NODE_CONNSTR_05_007: [If requiredFields is falsy, parse shall not validate fields.]*/
    it('returns an object with all the properties of the connection string', function () {
      var str = 'HostName=name;DeviceId=id;SharedAccessKey=key;GatewayHostName=name';
      var cn = ConnectionString.parse(str);
      assert.deepEqual(cn, {
        HostName: 'name',
        DeviceId: 'id',
        SharedAccessKey: 'key',
        GatewayHostName: 'name'
      });
    });

    /*Tests_SRS_NODE_CONNSTR_05_001: [The input argument source shall be converted to string if necessary.]*/
    it('accepts an argument that can be converted to String', function () {
      var obj = {
        value: 'HostName=name;DeviceId=id;SharedAccessKey=key;GatewayHostName=name',
        toString: function () { return this.value; }
      };
      var cn = ConnectionString.parse(obj);
      assert.deepEqual(cn, {
        HostName: 'name',
        DeviceId: 'id',
        SharedAccessKey: 'key',
        GatewayHostName: 'name'
      });
    });
  });
});