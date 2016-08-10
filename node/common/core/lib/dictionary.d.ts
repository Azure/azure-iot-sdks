// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

declare function createDictionary(source: string, seperator: string): createDictionary.Dictionary<string>;

declare namespace createDictionary {
    interface Dictionary<T> {
        [key: string]: T;
    }
}

export = createDictionary;
