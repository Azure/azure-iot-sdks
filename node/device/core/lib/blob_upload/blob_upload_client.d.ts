// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { Stream } from 'stream';
import { X509, SharedAccessSignature } from 'azure-iot-common';

import Client = require('../client');
import BlobUploadResult = require('./blob_upload_result');

declare class BlobUploadClient implements Client.BlobUpload {
    constructor(config: Client.Config, fileUploadApi?: BlobUploadClient.FileUpload, bloblUploader?: BlobUploadClient.BlobUploader);

    uploadToBlob(blobName: string, stream: Stream, steamLength: number, done: (err?: Error) => void): void;
    updateSharedAccessSignature(sharedAccessSignature: string);
}

declare namespace BlobUploadClient {
    interface UploadParams {
        hostName?: string;
        containerName?: string;
        blobName?: string;
        sasToken?: string;
    }

    interface FileUpload {
        getBlobSharedAccessSignature(blobName: string, auth: X509 | SharedAccessSignature, done: (err: Error, uploadParams?: UploadParams) => void): void;
        notifyUploadComplete(correlationId: string, auth: X509 | SharedAccessSignature, uploadResult: BlobUploadResult, done: (err?: Error) => void): void;
    }

    interface BlobUploader {
        uploadToBlob(uploadParams: UploadParams, stream: Stream, streamLength: number, done: (err: Error, body?: any, result?: { statusCode: number, body: string }) => void): void;
    }
}

export = BlobUploadClient;
