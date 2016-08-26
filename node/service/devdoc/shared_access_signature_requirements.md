# azure-iothub.SharedAccessSignature Requirements

## Overview
`SharedAccessSignature` is a type representing an IoT Hub shared access signature.  It exposes a static factory method for creating a shared access signature object from a string, and exposes properties for each of the parsed fields in the string.  It also validates the required properties of the shared access signature.

## Example usage
```javascript
'use strict';
var SharedAccessSignature = require('azure-iothub'). SharedAccessSignature;

var sas = SharedAccessSignature.parse('[Shared access signature]');
console.log('sr=' + sas.sr);
console.log('sig=' + sas.sig);
console.log('skn=' + sas.skn);
console.log('se=' + sas.se);
```

## Public Interface

### SharedAccessSignature constructor
Creates a new instance of the object.  Normally callers will use one of the static factory methods (`create`, `parse`) to create a `SharedAccessSignature`.

### create(host, policy, key, expiry) [static]
The `create` static method returns a new instance of the `SharedAccessSignature` object with `sr`, `sig`, `skn`, and `se` properties.

**SRS_NODE_IOTHUB_SAS_05_003: [** The `create` method shall return the result of calling `azure-iot-common.SharedAccessSignature.create` with following arguments:
```
resourceUri - host
keyName - policy
key - key
expiry - expiry
```
**]**  

### parse(source) [static]
The `parse` static method returns a new instance of the `SharedAccessSignature` object with properties corresponding to each `name=value` field found in source.

**SRS_NODE_IOTHUB_SAS_05_001: [** The `parse` method shall return the result of calling `azure-iot-common.SharedAccessSignature.parse`.**]**  
**SRS_NODE_IOTHUB_SAS_05_002: [** It shall throw `ArgumentError` if any of `sr`, `sig`, `skn` or `se` fields are not found in the source argument.**]**   
