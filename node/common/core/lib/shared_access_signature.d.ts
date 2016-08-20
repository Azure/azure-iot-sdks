// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

declare class SharedAccessSignature {
    sr: string;
    sig: string;
    skn: string;
    se: string;

    toString(): string;

    static create(resourceUri: string, keyName: string, key: string, expiry: string): SharedAccessSignature;
    static parse(source: string, requiredFields?: string[]): SharedAccessSignature;
}

export = SharedAccessSignature;
