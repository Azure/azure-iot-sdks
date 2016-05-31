# azure-iot-device.blobUpload.BlobUploadResult requirements

# Overview
`BlobUploadResult` describes the result of an upload to a blob, whether it succeeded or failed.

# Public API
## BlobUploadResult(isSuccess, statusCode, statusDescription) [constructor]
**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_001: [** The `isSuccess` parameter shall be assigned to the the `isSuccess` property of the newly created `BlobUploadResult` instance. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_002: [** The `statusCode` parameter shall be assigned to the the `statusCode` property of the newly created `BlobUploadResult` instance. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_003: [** The `statusDescription` parameter shall be assigned to the the `statusDescription` property of the newly created `BlobUploadResult` instance. **]**

## fromAzureStorageCallbackArgs(err, body, response) [static factory]
**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_004: [** If the `err` argument is not `null`, the `BlobUploadResult` error shall have the `isSuccess` property set to `false`. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_005: [** If `err`, `body` and `response` are not `null` (HTTP error), the `BlobUploadResult` error shall have the `statusCode` property set to the HTTP error code of the blob upload response. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_006: [** If `err`, `body` and `response` are not `null` (HTTP error), the `BlobUploadResult` error shall have the `statusDescription` property set to the HTTP error body of the blob upload response. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_007: [** If the `err` argument is not `null` but body and response are `undefined` (non HTTP error), the `BlobUploadResult` error shall have the `statusCode` property set to -1. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_008: [** If the `err` argument is not `null` but body and response are `undefined` (non HTTP error), the `BlobUploadResult` error shall have the `statusDescription` property set to the error message. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_009: [** If `err` is null but `body` and `reponse` are provided, the `BlobUploadResult` error shall have the `isSuccess` property set to `false`. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_010: [** If `err` is null but `body` and `reponse` are provided, the `BlobUploadResult` error shall have the `statusCode` property set to the HTTP status code of the blob upload response. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_011: [** If `err` is null but `body` and `reponse` are provided, the `BlobUploadResult` error shall have the `statusDescription` property set to the HTTP response body of the blob upload response. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_RESULT_16_012: [** If `err` is null and `body` and/or `response` is/are falsy, `fromAzureStorageCallbackArgs` shall throw a `ReferenceError`. **]**