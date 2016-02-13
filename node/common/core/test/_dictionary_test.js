// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var createDictionary = require('../lib/dictionary.js');

describe('createDictionary', function () {
  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_001: [The function createDictionary shall accept as arguments the source string and a field separator string.]*/
  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_003: [createDictionary shall search the source string for elements of the form 'key=value', where the element is found either at the beginning of the source string, or immediately following an instance of the field separator string.]*/
  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_005: [createDictionary shall return an object with properties matching each discovered element in the source string.]*/
  it('finds a key/value pair in the source string', function () {
    var dict = createDictionary('key=value');
    assert.propertyVal(dict, 'key', 'value');
  });

  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_003: [createDictionary shall search the source string for elements of the form 'key=value', where the element is found either at the beginning of the source string, or immediately following an instance of the field separator string.]*/
  it('finds more than one key/value pair in the source string', function () {
    var dict = createDictionary('key1=value1;key2=value2;key3=value3', ';');
    assert.propertyVal(dict, 'key1', 'value1');
    assert.propertyVal(dict, 'key2', 'value2');
    assert.propertyVal(dict, 'key3', 'value3');
  });

  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_004: [If there are multiple pairs with the same key, createDictionary shall keep the last pair.]*/
  it('keeps the last value when duplicate keys exist in the connection string', function () {
    var dict = createDictionary('key=value1;key=value2', ';');
    assert.propertyVal(dict, 'key', 'value2');
  });

  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_002: [createDictionary shall convert the source and separator arguments to type String before using them.]*/
  it('accepts any truthy source argument that is convertible to string', function () {
    var source = { source: 'key=value', toString: function () { return this.source; } };
    var dict = createDictionary(source);
    assert.propertyVal(dict, 'key', 'value');
  });

  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_006: [If the source string is falsy, createDictionary shall return an object with no properties.]*/
  it('creates no properties when the source string is falsy', function () {
    assert.deepEqual(createDictionary(0), {});
    assert.deepEqual(createDictionary(''), {});
    assert.deepEqual(createDictionary(null), {});
    assert.deepEqual(createDictionary(false), {});
  });

  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_003: [createDictionary shall search the source string for elements of the form 'key=value', where the element is found either at the beginning of the source string, or immediately following an instance of the field separator string.]*/
  it('creates at most one property when the separator is falsy', function () {
    assert.deepEqual(createDictionary('key1=value1;key2=value2'), { key1: 'value1;key2=value2'});
  });

  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_003: [createDictionary shall search the source string for elements of the form 'key=value', where the element is found either at the beginning of the source string, or immediately following an instance of the field separator string.]*/
  it('ignores consecutive delimiters', function () {
    var dict = createDictionary(';;', ';');
    assert.deepEqual(dict, {});
  });

  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_003: [createDictionary shall search the source string for elements of the form 'key=value', where the element is found either at the beginning of the source string, or immediately following an instance of the field separator string.]*/
  it('ignores keys without values', function () {
    var dict = createDictionary('key-without:a/value;key=',';');
    assert.deepEqual(dict, {});
  });

  /*Tests_SRS_NODE_COMMON_DICTIONARY_05_003: [createDictionary shall search the source string for elements of the form 'key=value', where the element is found either at the beginning of the source string, or immediately following an instance of the field separator string.]*/
  it('returns everything after the first equal sign (=) as the property value', function () {
    var dict = createDictionary('key1=value1=;key2=value2=value3', ';');
    assert.propertyVal(dict, 'key1', 'value1=');
    assert.propertyVal(dict, 'key2', 'value2=value3');
  });
});