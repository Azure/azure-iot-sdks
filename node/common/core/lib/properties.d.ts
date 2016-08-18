// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

declare class Properties {
    add(itemKey: string, itemValue: string): boolean;
    getItem(index: number): { key: string, value: string };
    count(): number;
}

export = Properties;
