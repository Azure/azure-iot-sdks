// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

export declare class ArgumentError extends Error {
    constructor(message: string);
}

export declare class DeviceMaximumQueueDepthExceededError extends Error {
    constructor(message: string);
}

export declare class DeviceNotFoundError extends Error {
    constructor(message: string);
}

export declare class FormatError extends Error {
    constructor(message: string);
}

export declare class UnauthorizedError extends Error {
    constructor(message: string);
}

export declare class NotImplementedError extends Error {
    constructor(message: string);
}

export declare class NotConnectedError extends Error {
    constructor(message: string);
}

export declare class IotHubQuotaExceededError extends Error {
    constructor(message: string);
}

export declare class MessageTooLargeError extends Error {
    constructor(message: string);
}

export declare class InternalServerError extends Error {
    constructor(message: string);
}

export declare class ServiceUnavailableError extends Error {
    constructor(message: string);
}

export declare class IotHubNotFoundError extends Error {
    constructor(message: string);
}

export declare class JobNotFoundError extends Error {
    constructor(message: string);
}

export declare class TooManyDevicesError extends Error {
    constructor(message: string);
}

export declare class ThrottlingError extends Error {
    constructor(message: string);
}

export declare class DeviceAlreadyExistsError extends Error {
    constructor(message: string);
}

export declare class InvalidEtagError extends Error {
    constructor(message: string);
}
