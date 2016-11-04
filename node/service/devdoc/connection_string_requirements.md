# azure-iothub.ConnectionString Requirements

## Overview
`ConnectionString` is a type representing an IoT Hub connection string.  It exposes a static factory method for creating a connection string object from a string, and exposes properties for each of the parsed fields in the string.  It also validates the required properties of the connection string.

## Example usage
```javascript
'use strict';
var ConnectionString = require('azure-iothub').ConnectionString;

var cn = ConnectionString.parse('[Connection string]');
console.log('HostName=' + cn.HostName);
console.log('SharedAccessKeyName=' + cn.SharedAccessKeyName);
console.log('SharedAccessKey=' + cn.SharedAccessKey);
```

## Public Interface
### ConnectionString constructor
Creates a new instance of the object.

### parse(source) [static]
The `parse` static method returns a new instance of the `ConnectionString` object with properties corresponding to each `name=value` field found in source.
**SRS_NODE_IOTHUB_CONNSTR_05_001: [** The `parse` method shall return the result of calling `azure-iot-common.ConnectionString.parse`.**]**  
**SRS_NODE_IOTHUB_CONNSTR_05_002: [** It shall throw `ArgumentError` if any of `HostName`, `SharedAccessKeyName`, or `SharedAccessKey` fields are not found in the source argument.**]**   
