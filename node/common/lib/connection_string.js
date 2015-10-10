// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/*Codes_SRS_NODE_COMMON_CONNSTR_05_001: [The ConnectionString constructor shall accept as an argument the connection string to parse.]*/
function ConnectionString(value) {
  /*Codes_SRS_NODE_COMMON_CONNSTR_05_006: [If the value argument is falsy the constructor shall not add any properties to itself.]*/
  if (!value) return;

  /*Codes_SRS_NODE_COMMON_CONNSTR_05_002: [The constructor shall convert the value argument to type String before using it.]*/
  var segments = String(value).split(';');
  
  segments.forEach(function (nameValue) {
    /*Codes_SRS_NODE_COMMON_CONNSTR_05_003: [The constructor shall search value for name/value pairs of the form: 'element=somevalue', where the pair is found either at the beginning of value, or immediately following a semicolon (';').]*/
    var pair = nameValue.split('=');
    if (pair.length === 2) {
      /*Codes_SRS_NODE_COMMON_CONNSTR_05_004: [If there are multiple pairs with the same name, the last value shall be used.]*/
      /*Codes_SRS_NODE_COMMON_CONNSTR_05_005: [For each pair, the constructor shall create a property on the ConnectionString object instance with the same name as the pair.  The property’s value shall be the pair value.]*/
      this[pair[0]] = pair[1];
    }
    else if (pair.length === 3 && !pair[2]) {
      // base64-encoded values can have a trailing equal sign ('='), which we need to preserve
      this[pair[0]] = pair[1] + '=';
    }
  }.bind(this));
}

module.exports = ConnectionString;
