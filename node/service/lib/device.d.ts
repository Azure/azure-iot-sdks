// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

declare class Device {
    deviceId: string;
    generationId: string;
    etag: string;
    connectionState: string;
    status: 'enabled' | 'disabled';
    statusReason: string;
    connectionStateUpdateTime: string;
    statusUpdateTime: string;
    lastActivityTime: string;
    cloudToDeviceMessageCount: string;
    authentication: Device.Authentication;

    constructor(jsonData?: string);
}

declare namespace Device {
    // tslint:disable-next-line:class-name
    interface _SymmetricKey {
        primaryKey: string;
        secondaryKey: string;
    }

    interface Authentication {
        SymmetricKey?: _SymmetricKey;
        x509Thumbprint?: string;
    }
}

export = Device;
