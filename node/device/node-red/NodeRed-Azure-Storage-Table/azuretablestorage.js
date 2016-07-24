module.exports = function (RED) {

    var Client = require('azure-storage');
    var globaltable = null;
    var clientTableService = null;
    var clientAccountName = "";
    var clientAccountKey = "";
    var node = null;
    var nodeConfig = null;

    var entityClass = {};

    var statusEnum = {
        disconnected: { color: "red", text: "Disconnected" },
        sending: { color: "green", text: "Sending" },
        sent: { color: "blue", text: "Sent message" },
        error: { color: "grey", text: "Error" }
    };

    var setStatus = function (status) {
        node.status({ fill: status.color, shape: "dot", text: status.text });
    }

    var senddata = function () {
            node.log('Saving data into Azure Table Storage :\n   data: ' + entityClass.partitionKey + " - " + entityClass.rowKey + " - " + entityClass.data + " - " + entityClass.tableName);
            // Create a message and send it to the Azure Table Storage
            var entGen = Client.TableUtilities.entityGenerator;
            node.log('creating entity...');
            var entity = {
                PartitionKey: entGen.String(entityClass.partitionKey),
                RowKey: entGen.String(entityClass.rowKey),
                data: entGen.String(JSON.stringify(entityClass.data)),
            };
            node.log('entity created successfully');
            clientTableService.insertEntity(entityClass.tableName, entity, function(err, result, response) {
                node.log('trying to insert');
                if (err) {
                    node.error('Error while trying to save data:' + err.toString());
                    setStatus(statusEnum.error);
                } else {
                    node.log('data saved.');
                    setStatus(statusEnum.sent);
                    node.send('data saved.');
                }
            });
        };

    var readdata = function (table, pkey, rkey) {
            node.log('Reading data from Azure Table Storage :\n   data: ' + entityClass.partitionKey + " - " + entityClass.rowKey);
            clientTableService.retrieveEntity(entityClass.tableName, entityClass.partitionKey, entityClass.rowKey, function(err, result, response) {
                if (err) {
                    node.error('Error while trying to read data:' + err.toString());
                    setStatus(statusEnum.error);
                } else {
                    node.log(result.data._);
                    setStatus(statusEnum.sent);
                    node.send(result.data._);
                }
            });
         }

    var deleteTable = function (table) {
        node.log("Deleting table");
        clientTableService.deleteTable(table, function (err) {
             if (err) {
                node.error('Error while trying to delete table:' + err.toString());
                setStatus(statusEnum.error);
            } else {
                node.log('table deleted');
                setStatus(statusEnum.sent);
                node.send('table deleted');
            }   
        });
    }

    var uptadeEntity = function () {
            node.log('updating entity');
            var entity = {
                PartitionKey: entGen.String(entityClass.partitionKey),
                RowKey: entGen.String(entityClass.rowKey),
                data: entGen.String(entityClass.data),
            };
            clientTableService.insertOrReplaceEntity(entityClass.tableName, entity, function(err, result, response){
                if (err) {
                    node.error('Error while trying to update entity:' + err.toString());
                    setStatus(statusEnum.error);
                } else {
                    node.log('entity updated');
                    setStatus(statusEnum.sent);
                    node.send('entity updated');
                } 
            });
         }

    var deleteEntity = function () {
            node.log('deleting entity');
            var entity = {
                PartitionKey: entGen.String(entityClass.partitionKey),
                RowKey: entGen.String(entityClass.rowKey),
                data: entGen.String(entityClass.data),
            };
            clientTableService.deleteEntity(entityClass.tableName, entity, function(err, result, response){
                if (err) {
                    node.error('Error while trying to delete entity:' + err.toString());
                    setStatus(statusEnum.error);
                } else {
                    node.log('entity deleted');
                    setStatus(statusEnum.sent);
                    node.send('entity deleted');
                } 
            }); 
         }


    var queryEntity = function (table, fromcolumn, where, selectdata) {
        node.log('query entity');
        var query = new Client.TableQuery()
            .top(1)
            .where(entityClass.fromcolumn + ' eq ?', entityClass.where);
        clientTableService.queryEntities(entityClass.tableName, query, null, function(err, result, response){
            if (err) {
                node.error('Error while trying to query entity:' + err.toString());
                setStatus(statusEnum.error);
            } else {
                //node.log(JSON.stringify(result.entries.data));
                //setStatus(statusEnum.sent);
                //node.send(result.entries.data._);
            } 
        });
    }

    var disconnectFrom = function () { 
         if (clientTableService) { 
             node.log('Disconnecting from Azure'); 
             clientTableService.removeAllListeners(); 
             clientTableService = null;
             entityClass = {}; 
             setStatus(statusEnum.disconnected); 
         } 
     } 


    function createTable(callback) {
        node.log('Creating a table if not exists');
        var tableService = Client.createTableService(clientAccountName, clientAccountKey);
        clientTableService = tableService;
        tableService.createTableIfNotExists(entityClass.tableName, function(error, result, response) {
        if (!error) {
                // result contains true if created; false if already exists
                globaltable = entityClass.tableName;
                callback()
         }
         else {
             node.error(error);
         }
        });
    }

    // Main function called by Node-RED    
    function AzureTableStorage(config) {
        // Store node for further use
        node = this;
        nodeConfig = config;

        // Create the Node-RED node
        RED.nodes.createNode(this, config);
        clientAccountName = node.credentials.accountname
        clientAccountKey = node.credentials.key;

        this.on('input', function (msg) {

            var messageJSON = null;

            if (typeof (msg.payload) != "string") {
                node.log("JSON");
                messageJSON = msg.payload;
            } else {
                node.log("String");
                //Converting string to JSON Object if msg.payload is String
                //Sample string: {"tableName": "name", "action": "I" "partitionKey": "part1", "rowKey": "row1", "data": "data"}
                messageJSON = JSON.parse(msg.payload);
            }

            entityClass = messageJSON;
            //var tableselect = createTable(messageJSON.tableName);
            node.log('Received the input: ' + messageJSON.tableName);
            var action = messageJSON.action;
            // Sending data to Azure Table Storage
            setStatus(statusEnum.sending);
            switch (action) {
                case "I":
                    node.log('Trying to insert entity');
                    createTable(senddata);
                    break;
                case "R":
                    node.log('Trying to read entity');
                    createTable(readdata);
                    break;
                case "DT":
                    node.log('Trying to delete table');
                    deleteTable(entityClass.tableName);
                    break;
                case "Q":
                    //node.log('Trying to query data');
                    //queryEntity(messageJSON.tableName, messageJSON.fromColumn, messageJSON.where, messageJSON.selectData);
                    break;
                case "U":
                    node.log('trying to update entity');
                    createTable(uptadeEntity);
                    break;
                case "D":
                    node.log('trying to delete entity');
                    createTable(deleteEntity);
                    break;
                default:
                    node.log('action was not detected');
                    node.error('action was not detected');
                    setStatus(statusEnum.error);
                    break;
            }    
        });

        this.on('close', function () {
            disconnectFrom(this);
        });
    }

    // Registration of the node into Node-RED
    RED.nodes.registerType("Table Storage", AzureTableStorage, {
        credentials: {
            accountname: { type: "text" },
            key: { type: "text" },
        },
        defaults: {
            name: { value: "Azure Table Storage" },
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