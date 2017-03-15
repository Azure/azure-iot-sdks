module.exports = function (RED) {

    var Client = require('azure-storage');
    var fs = require('fs');
    var clientBlobService = null;
    var clientAccountName = "";
    var clientAccountKey = "";
    var clientContainerName = "";
    var clientBlobName = "";
    var node = null;
    var nodeConfig = null;

    var statusEnum = {
        disconnected: { color: "red", text: "Disconnected" },
        sending: { color: "green", text: "Sending" },
        sent: { color: "blue", text: "Sent message" },
        error: { color: "grey", text: "Error" }
    };

    var setStatus = function (status) {
        node.status({ fill: status.color, shape: "dot", text: status.text });
    }

    var downloadBlob = function (container, blob, filename) {
        node.log('Downloading data from Azure Blob Storage :\n   blob: ' + blob);
        clientBlobService.getBlobToStream(container, blob, fs.createWriteStream(filename), function(err, result, response){
            if (err) {
                node.error('Error while trying to download file:' + err.toString());
                setStatus(statusEnum.error);
            } else {
                node.log(JSON.stringify(result));
                setStatus(statusEnum.sent);
                node.send(filename + 'downloaded');
            }
        });
    };

    var uptadeBlob = function (container, blob, file) {
        node.log('Updating Blob');
        // 
    }

    var deleteBlob = function (container, blob) {
        node.log('deleting blob');
        clientBlobService.deleteBlob(container, blob, function(err, result, response){
            if (err) {
                node.error('Error while trying to delete blob:' + err.toString());
                setStatus(statusEnum.error);
            } else {
                node.log('Blob deleted');
                setStatus(statusEnum.sent);
                node.send('Blob deleted');
            } 
        });   
    }

    var disconnectFrom = function () { 
         if (clientBlobService) { 
             node.log('Disconnecting from Azure'); 
             clientBlobService.removeAllListeners(); 
             clientBlobService = null; 
             setStatus(statusEnum.disconnected); 
         } 
     } 


     function createContainer(containerName) {
        node.log('Creating a container if not exists');
        var blobService = Client.createBlobService(clientAccountName, clientAccountKey);
        clientBlobService = blobService;
        clientBlobService.createContainerIfNotExists(containerName, function(err, result, response) {
        if (!err) {
                // result contains true if created; false if already exists
                return containerName;
         }
         else {
             node.error(err);
         }
        });
    }

    function createBlob(container, blob, file) {
        var localcontainer = createContainer(container);
        node.log('Creating a blob if not exists on ' + container);
        clientBlobService.createBlockBlobFromLocalFile(container, blob, file, function(err, result, response) {
        if (err) {
                node.error('Error while trying to create blob:' + err.toString());
                setStatus(statusEnum.error);
         }
         else {
             node.log('Blob Created');
             setStatus(statusEnum.sent);
             node.send('Blob Created');
         }
        });
    }

    // Main function called by Node-RED    
    function AzureBlobStorage(config) {
        // Store node for further use
        node = this;
        nodeConfig = config;

        // Create the Node-RED node
        RED.nodes.createNode(this, config);
        clientAccountName = node.credentials.accountname
        clientAccountKey = node.credentials.key;
        clientContainerName = node.credentials.container;
        clientBlobName = node.credentials.blob;

        this.on('input', function (msg) {
            node.log()
            // Sending data to Azure Blob Storage
            setStatus(statusEnum.sending);
            createBlob(clientContainerName, clientBlobName, msg.filename);   
        });

        this.on('close', function () {
            disconnectFrom(this);
        });
    }

    function AzureBlobStorageDownload(config) {
        // Store node for further use
        node = this;
        nodeConfig = config;

        // Create the Node-RED node
        RED.nodes.createNode(this, config);
        clientAccountName = node.credentials.accountname
        clientAccountKey = node.credentials.key;
        clientContainerName = node.credentials.container;
        clientBlobName = node.credentials.blob;

        this.on('input', function (msg) {
            node.log('downloading blob')
            // Sending order to Azure Blob Storage
            createContainer(clientContainerName);
            setStatus(statusEnum.sending);
            downloadBlob(clientContainerName, clientBlobName, msg.payload);   
        });

        this.on('close', function () {
            disconnectFrom(this);
        });
    }

    // Registration of the node into Node-RED
    RED.nodes.registerType("Save Blob", AzureBlobStorage, {
        credentials: {
            accountname: { type: "text" },
            key: { type: "text" },
            container: { type: "text" },
            blob: { type: "text" },
        },
        defaults: {
            name: { value: "Save in Blob Storage" },
        }
    });

    // Registration of the node into Node-RED to download
    RED.nodes.registerType("Get Blob", AzureBlobStorageDownload, {
        credentials: {
            accountname: { type: "text" },
            key: { type: "text" },
            container: { type: "text" },
            blob: { type: "text" },
        },
        defaults: {
            name: { value: "Get Blob Storage" },
        }
    });


    // Helper function to print results in the console
    function printResultFor(op) {
        return function printResult(err, res) {
            if (err) node.error(op + ' error: ' + err.toString());
            if (res) node.log(op + ' status: ' + res.constructor.name);
        };
    }
}