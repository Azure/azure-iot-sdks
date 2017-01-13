// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/**
 * The `properties` module exports the {@linkcode Properties} class which
 * is a collection type that allows you to store a collection of user
 * defined properties.
 *
 * @module common/properties
 */

'use strict';

/**
 * @class Properties
 * @classdesc Collection of user defined properties. Values can only be
 *            strings.
 */
var Properties = function () {
  this.propertyList = [];
};

/**
 * Adds the key-value pair to the collection. The strings `'Authorization'`
 * and `'iothub-to'` are considered as reserved key names and will cause the
 * function to return `false` if those key names are used.
 */
/* Codes_SRS_NODE_IOTHUB_PROPERTIES_07_003: [The getData function shall return a string representation of the body of the message.] */
Properties.prototype.add = function (itemKey, itemValue) {
  /* Codes_SRS_NODE_IOTHUB_PROPERTIES_07_004: [If itemKey contains any of the reserved key names then the add function will return false] */
  if (itemKey === 'Authorization' || itemKey === 'iothub-to') {
    return false;
  }
  else {
    var objProperty = { key: itemKey, value: itemValue };
    this.propertyList.push(objProperty);
    return true;
  }
};

/**
 * Returns the key-value pair corresponding to the given index.
 * @returns {Object}  Key-value pair corresponding to the given index. The
 *                    returned object has the properties `key` and `value`
 *                    corresponding to the key and value of the property.
 */
/* Codes_SRS_NODE_IOTHUB_PROPERTIES_07_001: [if the supplied index is greater or equal to zero and the is less than property map length then it shall return the property object.] */
Properties.prototype.getItem = function (index) {
  if (index >= 0 && index < this.propertyList.length)
    return this.propertyList[index];

  /*Codes_SRS_NODE_IOTHUB_PROPERTIES_13_001: [ If the supplied index is less than zero or greater than or equal to the property map length then it shall return undefined. ]*/
  return undefined;
};

/**
 * Returns the number of items in the collection.
 */
/* Codes_SRS_NODE_IOTHUB_PROPERTIES_07_002: [Properties.Count shall return the number of items in the Properties map.] */
Properties.prototype.count = function () {
  return this.propertyList.length;
};

module.exports = Properties;