module.exports = function (RED) {

    var Connection = require('tedious').Connection; 
    var Request = require('tedious').Request;  
    var TYPES = require('tedious').TYPES;
    var client = null;
    var dbAddress = "";
    var dbName = "";
    var username = "";
    var pass = "";
    var queryString = "";
    var messageJSON = "";
    var node = null;
    var nodeConfig = null;

    var statusEnum = {
        disconnected: { color: "red", text: "Disconnected" },
        sending: { color: "green", text: "Sending" },
        sent: { color: "blue", text: "Sent message" },
        error: { color: "grey", text: "Error" },
        connect: { color: "yellow", text: "Connected"}
    };

    var setStatus = function (status) {
        node.status({ fill: status.color, shape: "dot", text: status.text });
    }


//---------------------------------------------------------- DATABASE--------------------------------------------------------------------
 function connectToDatabase(callback) { 
    if (!client) {
        client.close();
        var config = {  
            userName: username,  
            password: pass,  
            server: dbAddress,  
            options: {encrypt: true, database: dbName}  
        };  
        client = new Connection(config);  
        client.on('connect', function(err) {  
            if (err) {
                setStatus(statusEnum.error);
                node.error('Error: ' +JSON.stringify(err));
                node.log('Error: ' +JSON.stringify(err));
            } else {
                node.log("Connected");
                setStatus(statusEnum.connect);
                callback();
            }
        });
    }
 }

 function executeStatement() {  
        request = new Request(queryString, function(err) {  
            if (err) {  
                setStatus(statusEnum.error);
                node.error('Error: ' +JSON.stringify(err));
                node.log('Error: ' +JSON.stringify(err));
            }  
        });  
        var result = "";  
        request.on('row', function(columns) {  
            columns.forEach(function(column) {  
              if (column.value === null) {  
                node.log('NULL');  
              } else {  
                result += column.value + " | ";  
              }  
            });  
            node.log(result);
            node.send(result);
            setStatus(statusEnum.sent);  
            result ="";  
        });  
  
        request.on('done', function(rowCount, more) {  
            node.log(rowCount + ' rows returned');  
        });  
        client.execSql(request);  
    } 

    function executeStatementToInsert() {  
        request = new Request(queryString, function(err) {  
            if (err) {  
                setStatus(statusEnum.error);
                node.error('Error: ' +JSON.stringify(err));
                node.log('Error: ' +JSON.stringify(err));
            }  
        });  
        request.addParameter('Name', TYPES.NVarChar,'SQL Server Express 2014');  
        request.addParameter('Number', TYPES.NVarChar , 'SQLEXPRESS2014');  
        request.addParameter('Cost', TYPES.Int, 11);  
        request.addParameter('Price', TYPES.Int,11);  
        request.on('row', function(columns) {  
            columns.forEach(function(column) {  
              if (column.value === null) {  
                node.log('NULL');
                setStatus(statusEnum.sent);  
              } else {  
                node.log("Product id of inserted item is " + column.value);
                node.send("Product id of inserted item is " + column.value);
                setStatus(statusEnum.sent);  
              }  
            });  
        });       
        client.execSql(request);  
    }  

//---------------------------------------------------------- GENERAL--------------------------------------------------------------------
    var disconnectFrom = function () { 
         if (client) { 
             node.log('Disconnecting from Azure SQL');
             client.close(); 
             client = null; 
             setStatus(statusEnum.disconnected); 
         } 
     } 

    // Main function called by Node-RED    
    function AzureSQL(config) {
        // Store node for further use
        node = this;
        nodeConfig = config;

        // Create the Node-RED node
        RED.nodes.createNode(this, config);
        dbAddress = node.credentials.serverAddress;
        dbName = node.credentials.databaseName;
        username = node.credentials.login;
        pass = node.credentials.password;

        this.on('input', function (msg) {
            //Connecting to Database and querying
            var messageJSON = null;

            if (typeof (msg.payload) != "string") {
                node.log("JSON");
                messageJSON = msg.payload;
            } else {
                node.log("String");
                //Converting string to JSON Object
                //Sample string to QUERY : {"action": "Q", "query" : "SELECT * FROM table WHERE firstName = 'Lucas'"}
                messageJSON = JSON.parse(msg.payload);
            }
            var action = messageJSON.action;
            queryString = messageJSON.query;
            // Sending action to Azure SQL
            setStatus(statusEnum.sending);
            switch (action) {
                case "I":
                    node.log('Trying to insert data into Database');
                    //connectToDatabase(executeStatementToInsert);
                    break;
                case "Q":
                    node.log('Trying to query document');
                    connectToDatabase(executeStatement);
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

    // Registration of the node into Node-RED to manage Databases
    RED.nodes.registerType("Azure SQL", AzureSQL, {
        credentials: {
            serverAddress: { type: "text" },
            databaseName: { type: "text" },
            login: { type: "text" },
            password: { type: "password" },
        },
        defaults: {
            name: { value: "Azure SQL" },
        }
    });

    // Helper function to print results in the node
    function printResultFor(op) {
        return function printResult(err, res) {
            if (err) node.error(op + ' error: ' + err.toString());
            if (res) node.log(op + ' status: ' + res.constructor.name);
        };
    }
}