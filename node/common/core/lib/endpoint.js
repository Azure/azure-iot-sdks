// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

function devicePath(id) {
  return '/devices/' + id;
}

function eventPath(id) {
  return devicePath(id) + '/messages/events';
}

function messagePath(id) {
  return devicePath(id) + '/messages/devicebound';
}

function feedbackPath(id, lockToken) {
  return messagePath(id) + '/' + lockToken;
}

function versionQueryString() {
  return '?api-version=2016-02-03';
}

var endpoint = {
  devicePath: devicePath,
  eventPath: eventPath,
  messagePath: messagePath,
  feedbackPath: feedbackPath,
  versionQueryString: versionQueryString
};

module.exports = endpoint;
