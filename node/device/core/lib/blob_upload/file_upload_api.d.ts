// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import { X509, SharedAccessSignature } from 'azure-iot-common';
import { Http } from 'azure-iot-http-base';

import BlobUploadClient = require('./blob_upload_client');
import BlobUploadResult = require('./blob_upload_result');

declare class FileUploadApi implements BlobUploadClient.FileUpload {
    constructor(deviceId: string, hostName: string, httpTransport: Http);

    getBlobSharedAccessSignature(blobName: string, auth: X509 | SharedAccessSignature, done: (err: Error, uploadParams?: BlobUploadClient.UploadParams) => void): void;
    notifyUploadComplete(correlationId: string, auth: X509 | SharedAccessSignature, uploadResult: BlobUploadResult, done: (err?: Error) => void): void;
}

export = FileUploadApi;
