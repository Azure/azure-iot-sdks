Node-Red node to connect to Azure Table Storage
==============================

<a href="http://nodered.org" target="_new">Node-RED</a> nodes to talk to Azure Storage.

Some code of Azure are under MIT License.

Install
-------

Run the following command in your Node-RED user directory - typically `~/.node-red`

        npm install node-red-contrib-azure-blob-storage

Usage
-----

Azure node. Can be used to upload and download on Azure Blob Storage using 2 nodes:

 - Supports :
 
◦Create/Delete Containers
◦Create Blobs/Read

Still working on Update/Delete Blobs

##Save Blob
Use `msg.payload` to send a file to save on Azure Blob Storage.

Ex: 'msg.payload' -> filename that you need to upload. Ex: filename.txt

This file must to be in the same folder of Node-RED user directory - typically `~/.node-red`

##Get Blob
Use `msg.payload` to send a filename that will is save at local.

Ex: 'msg.payload' -> filename. Ex: filename.txt

This file will is save in the same folder of Node-RED user directory - typically `~/.node-red`


-----

Read more about Azure Storage on <a href="https://azure.microsoft.com/pt-br/documentation/services/storage/">Azure Storage</a>.


