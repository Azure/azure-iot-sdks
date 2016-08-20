// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Typescript only interface
export interface Result {
    tranportObj?: any;
}

export declare class MessageEnqueued implements Result {
    constructor(tranportObj?: any);
}

export declare class MessageCompleted implements Result {
    constructor(tranportObj?: any);
}

export declare class MessageRejected implements Result {
    constructor(tranportObj?: any);
}

export declare class MessageAbandoned implements Result {
    constructor(tranportObj?: any);
}

export declare class Connected implements Result {
    constructor(tranportObj?: any);
}

export declare class Disconnected implements Result {
    constructor(tranportObj: any, reason: string);
}
export declare class TransportConfigured implements Result {
    constructor(tranportObj?: any);
}

export declare class SharedAccessSignatureUpdated implements Result {
    constructor(tranportObj?: any);
}
