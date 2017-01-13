// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Stream } from 'stream';

import BlobUploadClient = require('./blob_upload_client');

declare class BlobUploader implements BlobUploadClient.BlobUploader {
    constructor(storageApi?: BlobUploader.StorageApi);

    uploadToBlob(uploadParams: BlobUploadClient.UploadParams, stream: Stream, streamLength: number, done: (err: Error, body?: any, result?: { statusCode: number, body: string }) => void): void;
}

declare namespace BlobUploader {
    interface BlobService {
        createBlockBlobFromStream(containerName: string, blobName: string, stream: Stream, streamLength: number, done: (err: Error, body?: any, result?: { statusCode: number, body: string }) => void): void;
    }

    interface StorageApi {
        createBlobServiceWithSas(hostName: string, sasToken: string): BlobService;
    }
}

export = BlobUploader;
