// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

function FakeBlobService() {}

FakeBlobService.prototype.createBlockBlobFromStream = function (containerName, blobName, stream, streamLength, done) {
  done('fakeError', 'fakeBody', 'fakeResponse');
};

function createBlobServiceWithSas (host, sasToken) {
  if (!host) throw new ReferenceError('host cannot be falsy');
  if (!sasToken) throw new ReferenceError('host cannot be falsy');
  
  return new FakeBlobService();
}

module.exports = {
  createBlobServiceWithSas: createBlobServiceWithSas
};