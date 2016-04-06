// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/**
 * @class       module:azure-iot-common.ArgumentError
 * @classdesc   Error thrown when an argument is invalid.
 *
 * @augments {Error}
 */
function ArgumentError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

ArgumentError.prototype = Object.create(Error.prototype);
ArgumentError.prototype.constructor = ArgumentError;
ArgumentError.prototype.name = 'ArgumentError';

/**
 * @class       module:azure-iot-common.DeviceMaximumQueueDepthExceededError
 * @classdesc   Error thrown when the message queue for a device is full.
 *
 * @augments {Error}
 */
function DeviceMaximumQueueDepthExceededError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

DeviceMaximumQueueDepthExceededError.prototype = Object.create(Error.prototype);
DeviceMaximumQueueDepthExceededError.prototype.constructor = DeviceMaximumQueueDepthExceededError;
DeviceMaximumQueueDepthExceededError.prototype.name = 'DeviceMaximumQueueDepthExceededError';

/**
 * @class       module:azure-iot-common.DeviceNotFoundError
 * @classdesc   Error thrown when a device cannot be found in the IoT Hub instance registry.
 *
 * @augments {Error}
 */
  function DeviceNotFoundError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

DeviceNotFoundError.prototype = Object.create(Error.prototype);
DeviceNotFoundError.prototype.constructor = DeviceNotFoundError;
DeviceNotFoundError.prototype.name = 'DeviceNotFoundError';

/**
 * @class       module:azure-iot-common.FormatError
 * @classdesc   Error thrown when a string that is supposed to have a specific formatting is not formatted properly.
 *
 * @augments {Error}
 */
function FormatError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

FormatError.prototype = Object.create(Error.prototype);
FormatError.prototype.constructor = FormatError;
FormatError.prototype.name = 'FormatError';

/**
 * @class       module:azure-iot-common.UnauthorizedError
 * @classdesc   Error thrown when the connection parameters are wrong and the server refused the connection.
 *
 * @augments {Error}
 */
function UnauthorizedError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

UnauthorizedError.prototype = Object.create(Error.prototype);
UnauthorizedError.prototype.constructor = UnauthorizedError;
UnauthorizedError.prototype.name = 'UnauthorizedError';

function NotImplementedError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

NotImplementedError.prototype = Object.create(Error.prototype);
NotImplementedError.prototype.constructor = NotImplementedError;
NotImplementedError.prototype.name = 'NotImplementedError';

function NotConnectedError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

NotConnectedError.prototype = Object.create(Error.prototype);
NotConnectedError.prototype.constructor = NotConnectedError;
NotConnectedError.prototype.name = 'NotConnectedError';

function IotHubQuotaExceededError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

IotHubQuotaExceededError.prototype = Object.create(Error.prototype);
IotHubQuotaExceededError.prototype.constructor = IotHubQuotaExceededError;
IotHubQuotaExceededError.prototype.name = 'IotHubQuotaExceededError';

function MessageTooLargeError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

MessageTooLargeError.prototype = Object.create(Error.prototype);
MessageTooLargeError.prototype.constructor = MessageTooLargeError;
MessageTooLargeError.prototype.name = 'MessageTooLargeError';

function InternalServerError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

InternalServerError.prototype = Object.create(Error.prototype);
InternalServerError.prototype.constructor = InternalServerError;
InternalServerError.prototype.name = 'InternalServerError';

function ServiceUnavailableError(message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

ServiceUnavailableError.prototype = Object.create(Error.prototype);
ServiceUnavailableError.prototype.constructor = ServiceUnavailableError;
ServiceUnavailableError.prototype.name = 'ServiceUnavailableError';

module.exports = {
  ArgumentError: ArgumentError,
  DeviceMaximumQueueDepthExceededError: DeviceMaximumQueueDepthExceededError,
  DeviceNotFoundError: DeviceNotFoundError,
  FormatError: FormatError,
  UnauthorizedError: UnauthorizedError,
  NotImplementedError: NotImplementedError,
  NotConnectedError: NotConnectedError,
  IotHubQuotaExceededError: IotHubQuotaExceededError,
  MessageTooLargeError: MessageTooLargeError,
  InternalServerError: InternalServerError,
  ServiceUnavailableError: ServiceUnavailableError
};
