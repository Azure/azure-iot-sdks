// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/*Codes_SRS_NODE_COMMON_CONNSTR_05_001: [The ConnectionString constructor shall accept two arguments:
value - the connection string to parse
segments - the name of the property to expect when parsing value, or an array of such names]*/
function ConnectionString(value, segments) {
  /*Codes_SRS_NODE_COMMON_CONNSTR_05_002: [The constructor shall convert the value argument to type String before using it.]*/
  var valueStr = String(value);

  /*Codes_SRS_NODE_COMMON_CONNSTR_05_003: [If the segments argument is not of type Array, the constructor shall convert it to type Array before using it.]*/
  if (Object.prototype.toString.call(segments) !== '[object Array]') {
    segments = [segments];
  }

  /*Codes_SRS_NODE_COMMON_CONNSTR_05_008: [If either value or segments arguments are falsy, or if the segments array is empty, the constructor shall not add any properties to itself.]*/
  segments.forEach(function (name) {
    /*Codes_SRS_NODE_COMMON_CONNSTR_05_004: [The constructor shall convert all elements in the segments argument to type String before using them.]*/
    var exp = '(?:^|;)' + name + '=([^;]+)';
    /*Codes_SRS_NODE_COMMON_CONNSTR_05_005: [For each element in the segments array, the constructor shall search value for a match of the form: 'element=somevalue', where this match is found either at the beginning of value, or immediately following a semicolon (';').]*/
    /*Codes_SRS_NODE_COMMON_CONNSTR_05_006: [If there are multiple matches for a given element, only the first shall be used.]*/
    var match = valueStr.match(new RegExp(exp));
    /*Codes_SRS_NODE_COMMON_CONNSTR_05_007: [For each element that has a match, the constructor shall create a property on the ConnectionString object instance with the same name as the element value.  The property’s value shall be the sequence of characters in the connection string following the equal sign ('=') up to but not including the next semicolon (';'), or to the end of the string if no semicolon exists.]*/
    if (!!match) this[name] = match[1];
  }.bind(this));
}

module.exports = ConnectionString;
