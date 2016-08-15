// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

export function devicePath(id: string): string;
export function eventPath(id: string): string;
export function messagePath(id: string): string;
export function feedbackPath(id: string, lockToken: string): string;
export function versionQueryString(): string;
