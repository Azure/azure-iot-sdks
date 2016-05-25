# azure-iot-device.blobUpload.BlobUploader requirements

# Overview
`BlobUploader` uploads data from a given stream into an Azure block blob.

# Usage

```js
var fs = require('fs');

var filePath = './README.md';

fs.stat(filePath, function (err, fileStats) {
  var fileStream = fs.createReadStream(filePath);
  var uploader = new BlobUploader();
  var blobInfo = {
    hostName: '<storage.host.name>',
    containerName: '<container>',
    blobName: '<blobName>',
    sasToken: '<SAS Token>'
  };
  uploader.uploadToBlob(blobInfo, fileStream, fileStats.size, function (err) {
    if (err) {
      console.log('Upload failed: ' + err.message);
    } else {
      console.log('Upload succeeded');
    }
    fileStream.destroy();
  });
});
```

# Public API
## BlobUploader(storageApi) constructor
**SRS_NODE_DEVICE_BLOB_UPLOAD_16_007: [** `BlobUploader` should use the `azure-storage` package to upload data if `storageApi` is falsy. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_16_008: [** `BlobUploader` should use the `storageApi` object to upload data if `storageApi` is truthy. **]**

## uploadToBlob(blobInfo, stream, streamSize, done)
**SRS_NODE_DEVICE_BLOB_UPLOAD_16_001: [** `uploadToBlob` shall throw a `ReferenceError` if `blobInfo` is falsy. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_16_002: [** `uploadToBlob` shall throw a `ReferenceError` if `stream` is falsy. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_16_003: [** `uploadToBlob` shall throw a `ReferenceError` if `streamSize` is falsy. **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_16_004: [** `uploadToBlob` shall throw an `ArgumentError` if `blobInfo` is missing one or more of the following properties: `hostName`, `containerName`, `blobName`, `sasToken`). **]**

**SRS_NODE_DEVICE_BLOB_UPLOAD_16_005: [** `uploadToBlob` shall call the `done` calback with the result of the storage api call. **]**