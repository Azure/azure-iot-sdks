# azure-iot-device.blobUpload.BlobUploadClient requirements

# Overview
`BlobUploadClient` is the class that implements data upload to Azure Blob Storage using the IoT Hub APIs.

# Public API

## BlobUploadClient(config, fileUploadApi, blobUploader) [constructor]
Initializes a new instance of the `BlobUploadClient` class.

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_001: [** `BlobUploadClient` shall throw a `ReferenceError` if `config` is falsy. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_002: [** If specified, `BlobUploadClient` shall use the `fileUploadApi` passed as a parameter instead of the default one. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_003: [** If specified, `BlobUploadClient` shall use the `blobUploader` passed as a parameter instead of the default one. **]**

## updateSharedAccessSignature(sharedAccessSignature)
Updates the current value shared access signature.

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_011: [** `updateSharedAccessSignature` shall update the value used by the `BlobUploadClient` instance to the value passed as an argument. **]**

## uploadToBlob(blobName, stream, streamLength, done)
Uploads the specified stream to the specified blob.

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_004: [** `uploadToBlob` shall obtain a blob SAS token using the IoT Hub service file upload API endpoint. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_005: [** `uploadToBlob` shall call the `done` callback with a `BlobSasError` parameter if retrieving the SAS token fails. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_006: [** `uploadToBlob` shall upload the stream to the specified blob using its BlobUploader instance. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_008: [** `uploadToBlob` shall notify the result of a blob upload to the IoT Hub service using the file upload API endpoint. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_009: [** `uploadToBlob` shall call the `done` callback with a `BlobUploadNotificationError` if notifying the IoT Hub instance of the transfer outcome fails. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_CLIENT_16_010: [** `uploadToBlob` shall call the `done` callback with no arguments if IoT Hub was successfully notified of the blob upload outcome, regardless of the success state of the transfer itself. **]**