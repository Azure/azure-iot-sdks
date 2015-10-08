// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var createDictionary = require('./dictionary.js');
var ArgumentError = require('./errors.js').ArgumentError;
var FormatError = require('./errors.js').FormatError;

function SharedAccessSignature() {}

SharedAccessSignature.parse = function parse(source) {
  /*Codes_SRS_NODE_COMMON_SAS_05_001: [The input argument source shall be converted to string if necessary.]*/
  var parts = String(source).split(/\s/);
  /*Codes_SRS_NODE_COMMON_SAS_05_005: [The parse method shall throw FormatError if the shared access signature string does not start with 'SharedAccessSignature<space>'.]*/
  if (parts.length !== 2 || !parts[0].match(/SharedAccessSignature/)) {
    throw new FormatError('Malformed signature');
  }

  var dict = createDictionary(parts[1], '&');
  var err = 'The shared access signature is missing the property: ';

  /*Codes_SRS_NODE_COMMON_SAS_05_006: [The parse method shall throw ArgumentError if any of 'sr', 'sig', 'skn', or 'se' fields are not found in the input argument.]*/
  ['sr', 'sig', 'skn', 'se'].forEach(function (key) {
    if (!(key in dict)) throw new ArgumentError(err + key);
  });

  /*Codes_SRS_NODE_COMMON_SAS_05_002: [The parse method shall create a new instance of SharedAccessSignature.]*/
  var sas = new SharedAccessSignature();

  /*Codes_SRS_NODE_COMMON_SAS_05_003: [It shall accept a string argument of the form 'name=value[;name=value…]' and for each name extracted it shall create a new property on the SharedAccessSignature object instance.]*/
  /*Codes_SRS_NODE_COMMON_SAS_05_004: [The value of the property shall be the value extracted from the input argument for the corresponding name.]*/
  Object.keys(dict).forEach(function (key) {
    sas[key] = dict[key];
  });

  /*Codes_SRS_NODE_COMMON_SAS_05_007: [The generated SharedAccessSignature object shall be returned to the caller.]*/
  return sas;
};

module.exports = SharedAccessSignature;