// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var SharedAccessSignature = require('./shared_access_signature.js');

var incompleteSignatures = {
  sr:  'SharedAccessSignature sig=signature&se=expiry',
  sig: 'SharedAccessSignature sr=audience&se=expiry',
  se:  'SharedAccessSignature sr=audience&sig=signature'
};

describe('SharedAccessSignature', function () {
  describe('#parse', function () {
    /*Tests_SRS_NODE_DEVICE_SAS_05_001: [The parse method shall return the result of calling azure-iot-common.SharedAccessSignature.parse.]*/
    /*Tests_SRS_NODE_DEVICE_SAS_05_002: [It shall throw ArgumentError if any of 'sr', 'sig', 'se' fields are not found in the source argument.]*/
    ['sr', 'sig', 'se'].forEach(function (key) {
      it('throws if shared access signature is missing ' + key, function () {
        assert.throws(function () {
          SharedAccessSignature.parse(incompleteSignatures[key]);
        }, ArgumentError);
      });
    });
    
    it('does not throw if shared access signature is missing skn', function () {
      assert.doesNotThrow(function () {
        SharedAccessSignature.parse('SharedAccessSignature sr=audience&sig=signature&skn=keyname&se=expiry');
      }, ArgumentError);
    });
  });
});
