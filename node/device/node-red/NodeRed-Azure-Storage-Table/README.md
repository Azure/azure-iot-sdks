Node-Red node to connect to Azure Table Storage
==============================

<a href="http://nodered.org" target="_new">Node-RED</a> nodes to talk to Azure Storage.

Some code of Azure are under MIT License.

Install
-------

Run the following command in your Node-RED user directory - typically `~/.node-red`

        npm install node-red-contrib-azure-table-storage

Usage
-----

Azure node. Can be used to save Entity, retrieve Entity and query on Azure Table Storage:

 - Supports :
 
◦Create/Delete Tables
◦Create/Read/Delete/Update Entities

Still working on query Entities.


Use `msg.payload` to send a string with all data what you want to excute something on Azure Table Storage. Pay attention on each action variable. 

## To save entity
Ex: 'msg.payload' -> {"tableName": "name", "action": "I", "partitionKey": "part1", "rowKey": "row1", "data": "data"}

- If you send data as JSON, the node will save each key as column.
 
##To read entity
Ex: 'msg.payload' -> {"tableName": "name", "action": "R", "partitionKey": "part1", "rowKey": "row1"}

## To delete entity
Ex: 'msg.payload' -> {"tableName": "name", "action": "D", "partitionKey": "part1", "rowKey": "row1"}

## To update entity
Ex: 'msg.payload' -> {"tableName": "name", "action": "U", "partitionKey": "part1", "rowKey": "row1", "data": "data"}

## To query entity
Ex: 'msg.payload' -> {"tableName": "name", "action": "Q", "selectdata": "columnName", "fromcolumn": "from", "where": "where"}
 
##To delete table
Ex: 'msg.payload' -> {"tableName": "name", "action": "DT"}


Read more about Azure Storage on <a href="https://azure.microsoft.com/pt-br/documentation/services/storage/">Azure Storage</a>.


