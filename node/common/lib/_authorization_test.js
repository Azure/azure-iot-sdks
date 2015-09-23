// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var internal = require('./authorization_internals.js');
var authorization = require('./authorization.js');

var key = new Buffer('password').toString('base64');

describe('authorization', function () {

  describe('internals', function () {

    describe('#stringToSign', function () {

      /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_006: [<stringToSign> shall be a concatenation of <resourceUri> + '\n' + <expiry>.]*/
      it('returns <resourceUri>\\n<expiry>', function () {
        var stringToSign = internal.stringToSign('blah/blah', 12345);
        assert.equal('blah/blah\n12345', stringToSign);
      });

    });
  
    describe('#hmacHash', function () {

      /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_005: [<signature> shall be an HMAC-SHA256 hash of <stringToSign>, which is then base64-encoded.]*/
      it('returns base64-encoded HMAC SHA-256 hash of string-to-sign', function () {
        var hash = internal.hmacHash(key, 'string-to-sign');
        assert.equal('o1n1hiIVgzVA2+krq9ty1Z4xsIJKJZP3dYT8MOi65Y4=', hash);
      });

    });

    describe('#deviceResourceUri', function () {
  
      it('returns \'<host>/devices/<urlEncodedDeviceId>\'', function () {
        var uri = internal.deviceResourceUri('host', 'encode(\'Me\')');
        assert.equal('host/devices/encode%28%27Me%27%29', uri);
      });
  
    });
    
    describe('#serviceResourceUri', function () {
  
      it('returns \'<host>\'', function () {
        var uri = internal.serviceResourceUri('host');
        assert.equal('host', uri);
      });
  
    });

  });

  describe('Token', function () {

    describe('#constructor', function () {

      /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_002: [If resourceUri, key, or expiry are falsy (i.e., undefined, null, or empty), the Token constructor shall throw a reference exception.]*/
      it('throws when \'resourceUri\' is falsy', function () {
        throwsWhenFalsy('resourceUri');
      });
  
      it('throws when \'key\' is falsy', function () {
        throwsWhenFalsy('key');
      });
  
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
      
        function createAuthToken() {
          return new authorization.Token(args.resourceUri, args.keyName, args.key, args.expiry);
        }
      
        function throws(element, index, array) {
          args[argName] = element;
          var messageMatch = new RegExp('^Argument \'' + argName + '\' is ' + element);
          assert.throws(createAuthToken, ReferenceError, messageMatch);
        }
      
        [undefined, null, '', 0].forEach(throws);
      }

    });
  
    describe('#toString', function () {

      /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_001: [The Token constructor shall accept four arguments:
      resourceUri - the resource URI to encode into the token
      keyName - an identifier associated with the key 
      key - a base64-encoded key value
      expiry - an integer value representing the number of seconds since the epoch 00:00:00 UTC on 1 January 1970.]*/
      /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_003: [The toString method shall return a shared-access signature token of the form:
      SharedAccessSignature sr=<resourceUri>&sig=<urlEncodedSignature>&se=<expiry>&skn=<urlEncodedKeyName>]*/
      /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_004: [<urlEncodedSignature> shall be the URL-encoded <signature>.]*/
      /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_007: [<urlEncodedKeyName> shall be the URL-encoded <keyName>.]*/
      it('returns a signature token', function () {
        var expect = 'SharedAccessSignature sr=uri&sig=kcWxXAK2OAaAoq2oCkaGzX2NMqDB8nCyNuq2fPYCDhk%3D&se=12345&skn=encode%28%27Me%27%29';
        var token = new authorization.Token('uri', 'encode(\'Me\')', key, 12345);
        assert.equal(expect, token.toString());
      });
      
      /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_008: [If the keyName argument to the Token constructor was falsy, <urlEncodedKeyName> shall be empty.]*/
      it('returns a signature token with an empty skn', function () {
        var expect = 'SharedAccessSignature sr=uri&sig=kcWxXAK2OAaAoq2oCkaGzX2NMqDB8nCyNuq2fPYCDhk%3D&se=12345&skn=';
        ['', null, undefined].forEach(function (falsyKeyName) {
          var token = new authorization.Token('uri', falsyKeyName, key, 12345);
          assert.equal(expect, token.toString());
        });
      });

    });

  });
  
  /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_009: [DeviceToken shall return an object created as if by passing the following arguments to the Token constructor:
  resourceUri - <host>/devices/<urlEncodedDeviceId>
  keyName - null
  key - <key>
  expiry - <expiry>]*/
  describe('DeviceToken', function () {
    it('creates a device token', function () {
      var expect = 'SharedAccessSignature sr=host/devices/device&sig=cYyu26VeeH%2Boy96wrcS2I5guVNSzKNeenQIBvUAguD4%3D&se=12345&skn=';
      var token = new authorization.DeviceToken('host', 'device', 'key', 12345);
      assert.equal(expect, token.toString());
    });
  });

  /*Tests_SRS_NODE_COMMON_AUTHORIZATION_05_010: [ServiceToken shall return an object created as if by passing the following arguments to the Token constructor:
  resourceUri - <host>
  keyName - <policy>
  key - <key>
  expiry - <expiry>]*/
  describe('ServiceToken', function () {
    it('creates a service token', function () {
      var expect = 'SharedAccessSignature sr=host&sig=88JmrIsVYOGmRSjCO1x1LLbv0K001Gikh1rjfJqbQXA%3D&se=12345&skn=policy';
      var token = new authorization.ServiceToken('host', 'policy', 'key', 12345);
      assert.equal(expect, token.toString());
    });
  });

});
