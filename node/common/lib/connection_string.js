// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

function ConnectionString(value, segments) {
  var valueStr = String(value);

  if (Object.prototype.toString.call(segments) !== '[object Array]') {
    segments = [segments];
  }

  segments.forEach(function (name) {
    var exp = '(?:^|;)' + name + '=([^;]+)';
    var match = valueStr.match(new RegExp(exp));
    if (!!match) this[name] = match[1];
  }.bind(this));
}

module.exports = ConnectionString;
