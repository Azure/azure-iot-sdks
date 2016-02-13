// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var ArgumentError = require('../lib/errors.js').ArgumentError;
var FormatError = require('../lib/errors.js').FormatError;
var SharedAccessSignature = require('../lib/shared_access_signature.js');

var key = new Buffer('password').toString('base64');

describe('SharedAccessSignature', function () {
  describe('#create', function () {
    /*Tests_SRS_NODE_COMMON_SAS_05_009: [If resourceUri, key, or expiry are falsy (i.e., undefined, null, or empty), create shall throw ReferenceException.]*/
    it('throws when \'resourceUri\' is falsy', function () {
      throwsWhenFalsy('resourceUri');
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_009: [If resourceUri, key, or expiry are falsy (i.e., undefined, null, or empty), create shall throw ReferenceException.]*/
    it('throws when \'key\' is falsy', function () {
      throwsWhenFalsy('key');
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_009: [If resourceUri, key, or expiry are falsy (i.e., undefined, null, or empty), create shall throw ReferenceException.]*/
    it('throws when \'expiry\' is falsy', function () {
      throwsWhenFalsy('expiry');
    });

    function throwsWhenFalsy(argName) {
      var args = {
        resourceUri: 'r',
        keyName: 'n',
        key: 'k',
        expiry: 1
      };

      function createSharedAccessSignature() {
        return SharedAccessSignature.create(args.resourceUri, args.keyName, args.key, args.expiry);
      }

      function throws(element) {
        args[argName] = element;
        var messageMatch = new RegExp('^Argument \'' + argName + '\' is ' + element);
        assert.throws(createSharedAccessSignature, ReferenceError, messageMatch);
      }

      [undefined, null, '', 0].forEach(throws);
    }

    /*Tests_SRS_NODE_COMMON_SAS_05_008: [The create method shall accept four arguments:
    resourceUri - the resource URI to encode into the token
    keyName - an identifier associated with the key
    key - a base64-encoded key value
    expiry - an integer value representing the number of seconds since the epoch 00:00:00 UTC on 1 January 1970.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_010: [The create method shall create a new instance of SharedAccessSignature with properties: sr, sig, se, and optionally skn.]*/
    it('returns a SharedAccessSignature', function () {
      var sas = SharedAccessSignature.create('uri', 'name', 'key', 123);
      assert.instanceOf(sas, SharedAccessSignature);
      assert.sameMembers(Object.keys(sas), ['sr', 'sig', 'se', 'skn']);
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_011: [The sr property shall have the value of resourceUri.]*/
    it('populates sr with the value of resourceUri', function () {
      var sas = SharedAccessSignature.create('uri', 'name', 'key', 123);
      assert.propertyVal(sas, 'sr', 'uri');
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_013: [<signature> shall be an HMAC-SHA256 hash of the value <stringToSign>, which is then base64-encoded.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_014: [<stringToSign> shall be a concatenation of resourceUri + '\n' + expiry.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_012: [The sig property shall be the result of URL-encoding the value <signature>.]*/
    it('populates sig with the signature generated from resourceUri and expiry', function () {
      var sas = SharedAccessSignature.create('uri', 'name', key, 12345);
      assert.propertyVal(sas, 'sig', 'kcWxXAK2OAaAoq2oCkaGzX2NMqDB8nCyNuq2fPYCDhk%3D');
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_017: [<urlEncodedKeyName> shall be the URL-encoded value of keyName.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_016: [The skn property shall be the value <urlEncodedKeyName>.]*/
    it('populates skn with the URL-encoded value of keyName', function () {
      var sas = SharedAccessSignature.create('uri', 'encode(\'Me\')', key, 12345);
      assert.propertyVal(sas, 'skn', 'encode%28%27Me%27%29');
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_015: [The se property shall have the value of expiry.]*/
    it('populates se with the value of expiry', function () {
      var sas = SharedAccessSignature.create('uri', 'encode(\'Me\')', key, 12345);
      assert.propertyVal(sas, 'se', 12345);
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_018: [If the keyName argument to the create method was falsy, skn shall not be defined.]*/
    it('does not populate skn if keyName wasn\'t given', function () {
      ['', null, undefined].forEach(function (falsyKeyName) {
        var sas = SharedAccessSignature.create('uri', falsyKeyName, 'key', 12345);
        assert.notProperty(sas, 'skn');
      });
    });
  });

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

    /*Tests_SRS_NODE_COMMON_SAS_05_006: [The parse method shall throw ArgumentError if any of fields in the requiredFields argument are not found in the source argument.]*/
    ['sr', 'sig', 'skn', 'se'].forEach(function (key) {
      it('throws if shared access signature is missing ' + key, function () {
        assert.throws(function () {
          SharedAccessSignature.parse(utils.makeStringWithout(key), utils.fields());
        }, ArgumentError, utils.errorMessage(key));
      });
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_002: [The parse method shall create a new instance of SharedAccessSignature.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_003: [It shall accept a string argument of the form 'name=value[&name=value…]' and for each name extracted it shall create a new property on the SharedAccessSignature object instance.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_004: [The value of the property shall be the value extracted from the source argument for the corresponding name.]*/
    /*Tests_SRS_NODE_COMMON_SAS_05_007: [The generated SharedAccessSignature object shall be returned to the caller.]*/
    it('returns an object with all the properties of the shared access signature', function () {
      var sas = SharedAccessSignature.parse(utils.makeString());
      assert.deepEqual(utils.makePlainObject(sas), utils.properties);
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_001: [The input argument source shall be converted to string if necessary.]*/
    it('accepts an argument that can be converted to String', function () {
      var obj = {
        toString: function () { return utils.makeString(); }
      };
      var sas = SharedAccessSignature.parse(obj);
      assert.deepEqual(utils.makePlainObject(sas), utils.properties);
    });
  });

  describe('#toString', function () {
    /*Tests_SRS_NODE_COMMON_SAS_05_019: [The toString method shall return a shared-access signature token of the form:
    SharedAccessSignature sr=<resourceUri>&sig=<urlEncodedSignature>&se=<expiry>&skn=<urlEncodedKeyName>
    where the skn segment is optional.]*/
    it('returns a signature token', function () {
      var expect = 'SharedAccessSignature sr=uri&sig=kcWxXAK2OAaAoq2oCkaGzX2NMqDB8nCyNuq2fPYCDhk%3D&skn=encode%28%27Me%27%29&se=12345';
      var sas = SharedAccessSignature.create('uri', 'encode(\'Me\')', key, 12345);
      assert.equal(expect, sas.toString());
    });

    /*Tests_SRS_NODE_COMMON_SAS_05_020: [The skn segment is not part of the returned string if the skn property is not defined.]*/
    it('does not include the skn segment if the skn property wasn\'t defined', function () {
      var expect = 'SharedAccessSignature sr=uri&sig=kcWxXAK2OAaAoq2oCkaGzX2NMqDB8nCyNuq2fPYCDhk%3D&se=12345';
      var sas = SharedAccessSignature.create('uri', null, key, 12345);
      assert.equal(expect, sas.toString());
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

  fields: function () {
    return Object.keys(this.properties);
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
  },

  makePlainObject: function (src) {
    var dst = {};
    Object.keys(src).forEach(function (key) {
      dst[key] = src[key];
    });
    return dst;
  }
};
