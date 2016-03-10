// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var authorization = require('./authorization.js');
var ArgumentError = require('./errors.js').ArgumentError;
var createDictionary = require('./dictionary.js');
var FormatError = require('./errors.js').FormatError;

/**
 * @class           module:azure-iot-common.SharedAccessSignature
 * @classdesc       An object representing a shared access signature
 * @param {String}  resourceUri   The resource URI to encode into the signature
 * @param {String}  keyName       An identifier associated with the key
 * @param {String}  key           A base64-encoded key value
 * @param {String}  expiry        An integer value representing the number of
 *                                seconds since the epoch 00:00:00 UTC on
 *                                1 January 1970.
 */
function SharedAccessSignature() {
}

/**
 * @method          module:azure-iot-common.SharedAccessSignature.create
 * @description     Instantiate a SharedAccessSignature token with the given parameters.
 *
 * @param {String}  resourceUri     the resource URI to encode into the token.
 * @param {String}  keyName         an identifier associated with the key.
 * @param {String}  key             a base64-encoded key value.
 * @param {String}  expiry          an integer value representing the number of seconds since the epoch 00:00:00 UTC on 1 January 1970.
 *
 * @throws {ReferenceError}         Will be thrown if one of the arguments is falsy.
 *
 * @returns {SharedAccessSignature} A shared access signature token.
 */
/*Codes_SRS_NODE_COMMON_SAS_05_008: [The create method shall accept four arguments:
    resourceUri - the resource URI to encode into the token
    keyName - an identifier associated with the key
    key - a base64-encoded key value
    expiry - an integer value representing the number of seconds since the epoch 00:00:00 UTC on 1 January 1970.]*/
SharedAccessSignature.create = function create(resourceUri, keyName, key, expiry) {

  function throwRef(name, value) {
    throw new ReferenceError('Argument \'' + name + '\' is ' + value);
  }

  /*Codes_SRS_NODE_COMMON_SAS_05_009: [If resourceUri, key, or expiry are falsy (i.e., undefined, null, or empty), create shall throw ReferenceException.]*/
  if (!resourceUri) throwRef('resourceUri', resourceUri);
  if (!key) throwRef('key', key);
  if (!expiry) throwRef('expiry', expiry);

  /*Codes_SRS_NODE_COMMON_SAS_05_010: [The create method shall create a new instance of SharedAccessSignature with properties: sr, sig, se, and optionally skn.]*/
  var sas = new SharedAccessSignature();
  /*Codes_SRS_NODE_COMMON_SAS_05_011: [The sr property shall have the value of resourceUri.]*/
  sas.sr = resourceUri;
  /*Codes_SRS_NODE_COMMON_SAS_05_013: [<signature> shall be an HMAC-SHA256 hash of the value <stringToSign>, which is then base64-encoded.]*/
  /*Codes_SRS_NODE_COMMON_SAS_05_014: [<stringToSign> shall be a concatenation of resourceUri + '\n' + expiry.]*/
  var hash = authorization.hmacHash(key, authorization.stringToSign(resourceUri, expiry));
  /*Codes_SRS_NODE_COMMON_SAS_05_012: [The sig property shall be the result of URL-encoding the value <signature>.]*/
  sas.sig = authorization.encodeUriComponentStrict(hash);
  /*Codes_SRS_NODE_COMMON_SAS_05_018: [If the keyName argument to the create method was falsy, skn shall not be defined.]*/
  /*Codes_SRS_NODE_COMMON_SAS_05_017: [<urlEncodedKeyName> shall be the URL-encoded value of keyName.]*/
  /*Codes_SRS_NODE_COMMON_SAS_05_016: [The skn property shall be the value <urlEncodedKeyName>.]*/
  if (keyName) sas.skn = authorization.encodeUriComponentStrict(keyName);
  /*Codes_SRS_NODE_COMMON_SAS_05_015: [The se property shall have the value of expiry.]*/
  sas.se = expiry;

  return sas;
};

/**
 * @method          module:azure-iot-common.SharedAccessSignature.parse
 * @description     Instantiate a SharedAccessSignature token from a string.
 *
 * @param {String}  source          the string to parse in order to create the SharedAccessSignature token.
 * @param {Array}   requiredFields  an array of fields that we expect to find in the source string.
 *
 * @throws {FormatError}            Will be thrown if the source string is malformed.
 *
 * @returns {SharedAccessSignature} A shared access signature token.
 */
SharedAccessSignature.parse = function parse(source, requiredFields) {
  /*Codes_SRS_NODE_COMMON_SAS_05_001: [The input argument source shall be converted to string if necessary.]*/
  var parts = String(source).split(/\s/);
  /*Codes_SRS_NODE_COMMON_SAS_05_005: [The parse method shall throw FormatError if the shared access signature string does not start with 'SharedAccessSignature<space>'.]*/
  if (parts.length !== 2 || !parts[0].match(/SharedAccessSignature/)) {
    throw new FormatError('Malformed signature');
  }

  var dict = createDictionary(parts[1], '&');
  var err = 'The shared access signature is missing the property: ';

  requiredFields = requiredFields || [];

  /*Codes_SRS_NODE_COMMON_SAS_05_006: [The parse method shall throw ArgumentError if any of fields in the requiredFields argument are not found in the source argument.]*/
  requiredFields.forEach(function (key) {
    if (!(key in dict)) throw new ArgumentError(err + key);
  });

  /*Codes_SRS_NODE_COMMON_SAS_05_002: [The parse method shall create a new instance of SharedAccessSignature.]*/
  var sas = new SharedAccessSignature();

  /*Codes_SRS_NODE_COMMON_SAS_05_003: [It shall accept a string argument of the form 'name=value[&name=value…]' and for each name extracted it shall create a new property on the SharedAccessSignature object instance.]*/
  /*Codes_SRS_NODE_COMMON_SAS_05_004: [The value of the property shall be the value extracted from the source argument for the corresponding name.]*/
  Object.keys(dict).forEach(function (key) {
    sas[key] = dict[key];
  });

  /*Codes_SRS_NODE_COMMON_SAS_05_007: [The generated SharedAccessSignature object shall be returned to the caller.]*/
  return sas;
};


/**
 * @method          module:azure-iot-common.SharedAccessSignature#toString
 * @description     Formats a SharedAccessSignatureObject into a properly formatted string.
 *
 * @returns {String} A properly formatted shared access signature token.
 */
SharedAccessSignature.prototype.toString = function () {
  /*Codes_SRS_NODE_COMMON_SAS_05_019: [The toString method shall return a shared-access signature token of the form:
  SharedAccessSignature sr=<url-encoded resourceUri>&sig=<urlEncodedSignature>&se=<expiry>&skn=<urlEncodedKeyName>]*/
  var sas = 'SharedAccessSignature ';
  ['sr', 'sig', 'skn', 'se'].forEach(function (key) {
    /*Codes_SRS_NODE_COMMON_SAS_05_020: [The skn segment is not part of the returned string if the skn property is not defined.]*/
    if (this[key]) {
      if (sas[sas.length - 1] !== ' ') sas += '&';
      sas += key + '=' + this[key];
    }
  }.bind(this));

  return sas;
};

module.exports = SharedAccessSignature;