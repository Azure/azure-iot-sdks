// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var SharedAccessSignature = require('../lib/shared_access_signature.js');

var incompleteSignatures = {
  sr: 'SharedAccessSignature sig=signature&skn=keyname&se=expiry',
  sig: 'SharedAccessSignature sr=audience&skn=keyname&se=expiry',
  skn: 'SharedAccessSignature sr=audience&sig=signature&se=expiry',
  se: 'SharedAccessSignature sr=audience&sig=signature&skn=keyname'
};

describe('SharedAccessSignature', function () {
  describe('#create', function () {
    /*Tests_SRS_NODE_IOTHUB_SAS_05_003: [The create method shall return the result of calling azure-iot-common.SharedAccessSignature.create with following arguments:
    resourceUri - host
    keyName - policy
    key - key
    expiry - expiry]*/
    it('creates a shared access signature', function () {
      var expect = 'SharedAccessSignature sr=host&sig=88JmrIsVYOGmRSjCO1x1LLbv0K001Gikh1rjfJqbQXA%3D&skn=policy&se=12345';
      var sas = SharedAccessSignature.create('host', 'policy', 'key', 12345);
      assert.equal(expect, sas.toString());
    });
  });

  describe('#parse', function () {
    /*Tests_SRS_NODE_IOTHUB_SAS_05_001: [The parse method shall return the result of calling azure-iot-common.SharedAccessSignature.parse.]*/
    /*Tests_SRS_NODE_IOTHUB_SAS_05_002: [It shall throw ArgumentError if any of 'sr', 'sig', 'skn' or 'se' fields are not found in the source argument.]*/
    ['sr', 'sig', 'skn', 'se'].forEach(function (key) {
      it('throws if shared access signature is missing ' + key, function () {
        assert.throws(function () {
          SharedAccessSignature.parse(incompleteSignatures[key]);
        }, ArgumentError);
      });
    });
  });
});
