// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

import BlobUploadClient = require('./blob_upload_client');

declare class BlobUploadResult {
    isSuccess: boolean;
    statusCode: number;
    statusDescription: string;

    constructor(isSuccess: boolean, statusCode: number, statusDescription: string);
    // Note: body param is never used
    static fromAzureStorageCallbackArgs(err: Error, body?: any, response?: { statusCode: number, body: string }): BlobUploadResult;
}

export = BlobUploadResult;
