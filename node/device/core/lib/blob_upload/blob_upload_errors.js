// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/**
 * @class         module:azure-iot-device.BlobSasError
 * @classdesc     Error used when the client fails to get a blob shared access signature from the IoT Hub service.
 * 
 * @params        {string}  message  Error message
 * @augments      {Error}
 */
function BlobSasError (message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

BlobSasError.prototype = Object.create(Error.prototype);
BlobSasError.prototype.constructor = BlobSasError;
BlobSasError.prototype.name = 'BlobSasError';

/**
 * @class         module:azure-iot-device.BlobUploadNotificationError
 * @classdesc     Error used when the client fails to notify the IoT Hub service that the upload is complete.
 * 
 * @params        {string}  message  Error message
 * @augments      {Error}
 */
function BlobUploadNotificationError (message) {
  this.message = message;
  this.stack = (new Error()).stack;
  Error.call(this, message);
}

BlobUploadNotificationError.prototype = Object.create(Error.prototype);
BlobUploadNotificationError.prototype.constructor = BlobUploadNotificationError;
BlobUploadNotificationError.prototype.name = 'BlobUploadNotificationError';

module.exports = {
  BlobSasError: BlobSasError,
  BlobUploadNotificationError: BlobUploadNotificationError
};