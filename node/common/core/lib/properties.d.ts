// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

declare class Properties {
    add(itemKey: string, itemValue: string): boolean;
    getItem(index: number): Properties.PropertyPair;
    count(): number;
}

declare namespace Properties {
    interface KeyValuePair<K, V> {
        key: K;
        value: V;
    }
    type PropertyPair = KeyValuePair<string, string>
}

export = Properties;
