// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

function ArgumentError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

ArgumentError.prototype = Object.create(Error.prototype);
ArgumentError.prototype.constructor = ArgumentError;
ArgumentError.prototype.name = 'ArgumentError';

function DeviceMaximumQueueDepthExceededError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

DeviceMaximumQueueDepthExceededError.prototype = Object.create(Error.prototype);
DeviceMaximumQueueDepthExceededError.prototype.constructor = DeviceMaximumQueueDepthExceededError;
DeviceMaximumQueueDepthExceededError.prototype.name = 'DeviceMaximumQueueDepthExceededError';

function DeviceNotFoundError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

DeviceNotFoundError.prototype = Object.create(Error.prototype);
DeviceNotFoundError.prototype.constructor = DeviceNotFoundError;
DeviceNotFoundError.prototype.name = 'DeviceNotFoundError';

function FormatError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

FormatError.prototype = Object.create(Error.prototype);
FormatError.prototype.constructor = FormatError;
FormatError.prototype.name = 'FormatError';

function UnauthorizedError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

UnauthorizedError.prototype = Object.create(Error.prototype);
UnauthorizedError.prototype.constructor = UnauthorizedError;
UnauthorizedError.prototype.name = 'UnauthorizedError';

module.exports = {
  ArgumentError: ArgumentError,
  DeviceMaximumQueueDepthExceededError: DeviceMaximumQueueDepthExceededError,
  DeviceNotFoundError: DeviceNotFoundError,
  FormatError: FormatError,
  UnauthorizedError: UnauthorizedError
};
