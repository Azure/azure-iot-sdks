// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

export declare class BlobSasError implements Error {
    name: string;
    message: string;
    stack: string;

    constructor(message: string);
}


export declare class BlobUploadNotificationError implements Error {
    name: string;
    message: string;
    stack: string;

    constructor(message: string);
}
