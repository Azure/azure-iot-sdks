// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/*Codes_SRS_NODE_COMMON_DICTIONARY_05_001: [The function createDictionary shall accept as arguments the source string and a field separator string.]*/
function createDictionary(source, separator) {
  var dict = Object.create(null);
  /*Codes_SRS_NODE_COMMON_DICTIONARY_05_003: [createDictionary shall search the source string for elements of the form 'key=value', where the element is found either at the beginning of the source string, or immediately following an instance of the field separator string.]*/
  /*Codes_SRS_NODE_COMMON_DICTIONARY_05_002: [createDictionary shall convert the source and separator arguments to type String before using them.]*/
  /*Codes_SRS_NODE_COMMON_DICTIONARY_05_006: [If the source string is falsy, createDictionary shall return an object with no properties.]*/
  var elems = String(source).split(String(separator));

  elems.forEach(function (elem) {
    var pos = elem.indexOf('=');
    if (pos < 0) return;

    var name = elem.substring(0, pos);
    var value = elem.substring(pos + 1);

    if (name && value) {
      /*Codes_SRS_NODE_COMMON_DICTIONARY_05_004: [If there are multiple pairs with the same key, createDictionary shall keep the last pair.]*/
      dict[name] = value;
    }
  });

  /*Codes_SRS_NODE_COMMON_DICTIONARY_05_005: [createDictionary shall return an object with properties matching each discovered element in the source string.]*/
  return dict;
}

module.exports = createDictionary;
