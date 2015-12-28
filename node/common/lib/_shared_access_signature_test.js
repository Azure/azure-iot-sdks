// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('./errors.js').ArgumentError;
var FormatError = require('./errors.js').FormatError;
var SharedAccessSignature = require('./shared_access_signature.js');

describe('SharedAccessSignature', function () {
  describe('#parse', function () {
    /*Tests_SRS_NODE_COMMON_SAS_05_005: [The parse method shall throw FormatError if the shared access signature string does not start with 'SharedAccessSignature<space>'.]*/
    it('throws if signature doesn\'t start with \'SharedAccessSignature<space>\'', function () {
      [
        'blah',                          // no 'SharedAccessSignature'
        ' SharedAccessSignature  field', // space before 'SharedAccessSignature'
        'SharedAccessSignature  field',  // two spaces after 'SharedAccessSignature'
        'SharedAccessSignature fie ld'   // space in the fields
      ].forEach(function (sas) {
        assert.throws(function () {
          return SharedAccessSignature.parse(sas);
        }, FormatError, 'Malformed signature');
      });
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_006: [The parse method shall throw ArgumentError if any of 'sr', 'sig', 'skn', or 'se' fields are not found in the input argument.]*/
    ['sr', 'sig', 'skn', 'se'].forEach(function (key) {
      it('throws if shared access signature is missing ' + key, function () {
        assert.throws(function () {
          SharedAccessSignature.parse(utils.makeStringWithout(key));
        }, ArgumentError, utils.errorMessage(key));
      });
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_002: [The parse method shall create a new instance of SharedAccessSignature.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_003: [It shall accept a string argument of the form 'name=value[;name=value…]' and for each name extracted it shall create a new property on the SharedAccessSignature object instance.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_004: [The value of the property shall be the value extracted from the input argument for the corresponding name.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_007: [The generated SharedAccessSignature object shall be returned to the caller.]*/
    it('returns an object with all the properties of the shared access signature', function () {
      var sas = SharedAccessSignature.parse(utils.makeString());
      assert.deepEqual(sas, utils.properties);
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_001: [The input argument source shall be converted to string if necessary.]*/
    it('accepts an argument that can be converted to String', function () {
      var obj = {
        toString: function () { return utils.makeString(); }
      };
      var sas = SharedAccessSignature.parse(obj);
      assert.deepEqual(sas, utils.properties);
    });
  });
});


var utils = {
  properties: {
    sr: 'audience',
    sig: 'signature',
    skn: 'keyname',
    se: 'expiry'
  },

  makeStringWithout: function (skipKey) {
    var props = [];
    Object.keys(utils.properties).forEach(function (key) {
      if (key !== skipKey) {
        props.push(key + '=' + utils.properties[key]);
      }
    });
    return 'SharedAccessSignature ' + props.join('&');
  },

  makeString: function () {
    return utils.makeStringWithout('');
  },

  errorMessage: function (key) {
    return 'The shared access signature is missing the property: ' + key;
  }
};
