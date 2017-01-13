// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

export function anHourFromNow(): number;
export function encodeUriComponentStrict(str: string): string;
export function stringToSign(resourceUri: string, expiry: string): string;
export function hmacHash(password: string, stringToSign: string): string;
