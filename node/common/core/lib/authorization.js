// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var crypto = require('crypto');

/**
 * Returns the seconds elapsed since 1 January 1970 00:00:00 UTC until one
 * hour from now.
 * @function anHourFromNow
 */
function anHourFromNow() {
  var raw = (Date.now() / 1000) + 3600;
  return Math.ceil(raw);
}

/*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_004: [<urlEncodedSignature> shall be the URL-encoded <signature>.]*/
/*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_007: [<urlEncodedKeyName> shall be the URL-encoded <keyName>.]*/
function encodeUriComponentStrict(str) {
  // this stricter version of encodeURIComponent is a recommendation straight out of the MDN docs, see:
  // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/encodeURIComponent#Description
  return encodeURIComponent(str).replace(/[!'()*]/g, function(c) {
    return '%' + c.charCodeAt(0).toString(16);
  });
}

/*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_006: [<stringToSign> shall be a concatenation of <resourceUri> + '\n' + <expiry>.]*/
function stringToSign(resourceUri, expiry) {
  return resourceUri + '\n' + expiry;
}

/*Codes_SRS_NODE_COMMON_AUTHORIZATION_05_005: [<signature> shall be an HMAC-SHA256 hash of <stringToSign>, which is then base64-encoded.]*/
function hmacHash(password, stringToSign) {
  var decodedPassword = new Buffer(password, 'base64').toString('binary');
  var hmac = crypto.createHmac('sha256', decodedPassword);
  hmac.update(stringToSign);
  return hmac.digest('base64');
}

module.exports = {
  anHourFromNow: anHourFromNow,
  encodeUriComponentStrict: encodeUriComponentStrict,
  stringToSign: stringToSign,
  hmacHash: hmacHash
};
