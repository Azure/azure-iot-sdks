# azure-iot-common.errors Requirements

## Overview
the `errors` module contain custom errors used by the SDK to abstract user code from transport-specific errors.

## Requirements: All errors

**SRS_NODE_COMMON_ERRORS_16_001: [** All custom error types shall inherit from the standard Javascript error object. **]**

**SRS_NODE_COMMON_ERRORS_16_002: [** All custom error types shall contain a valid call stack. **]**

**SRS_NODE_COMMON_ERRORS_16_003: [** All custom error types shall accept a message as an argument of the constructor and shall populate their message property with it. **]**
