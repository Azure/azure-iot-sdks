module.exports = function (RED) {

    var DocumentDBClient = require('documentdb').DocumentClient
    var client = null;
    var dbendpoint = "";
    var dbkey = "";
    var dbName = "";
    var databaseUrl = "";
    var collectionName = "";
    var collectionUrl = "";
    var messageJSON = "";
    var node = null;
    var nodeConfig = null;
    var HttpStatusCodes = { NOTFOUND: 404 };

    var statusEnum = {
        disconnected: { color: "red", text: "Disconnected" },
        sending: { color: "green", text: "Sending" },
        sent: { color: "blue", text: "Sent message" },
        error: { color: "grey", text: "Error" }
    };

    var setStatus = function (status) {
        node.status({ fill: status.color, shape: "dot", text: status.text });
    }


//---------------------------------------------------------- DATABASE--------------------------------------------------------------------
    function getDatabase(callback) {
    node.log(`Getting database:\n${dbName}\n`);
    var dbdef = {id : dbName};
        return new Promise((resolve, reject) => {
            client.readDatabase(databaseUrl, (err, result) => {
                if (err) {
                    if (err.code == HttpStatusCodes.NOTFOUND) {
                        client.createDatabase(dbdef, (err, created) => {
                            if (err) reject(err)
                            else resolve(created);
                        });
                    } else {
                        reject(err);
                    }
                } else {
                    resolve(result);
                }
            });
        });
    }

    function deleteDatabase(callback) {
    node.log(`Getting database:\n${dbName}\n`);
        return new Promise((resolve, reject) => {
            client.readDatabase(databaseUrl, (err, result) => {
                if (err) {
                    if (err.code == HttpStatusCodes.NOTFOUND) {
                        client.deleteDatabase(databaseUrl, (err) => {
                            if (err) reject(err)
                            else resolve(null);
                        });
                    } else {
                        reject(err);
                    }
                } else {
                    resolve(result);
                }
            });
        });
    }

    function listDatabases(callback) {
        var queryIterator = client.readDatabases().toArray(function (err, dbs) {
            if (err) {
                setStatus(statusEnum.error);
                node.error('Completed with error ' +JSON.stringify(err));
                node.log('Completed with error ' +JSON.stringify(err));
            }
            callback(dbs);
        });
    }


//---------------------------------------------------------- COLLECTIONS--------------------------------------------------------------------
function getCollection() {
    node.log(`Getting collection:\n${collectionName}\n`);
    var colldef = {id : collectionName};
    return new Promise((resolve, reject) => {
        client.readCollection(collectionUrl, (err, result) => {
            if (err) {
                if (err.code == HttpStatusCodes.NOTFOUND) {
                    client.createCollection(databaseUrl, colldef, { offerThroughput: 400 }, (err, created) => {
                        if (err) reject(err)
                        else resolve(created);
                    });
                } else {
                    reject(err);
                }
            } else {
                resolve(result);
            }
        });
    });
}

function listCollections(databaseUrl, callback) {
    var queryIterator = client.readCollections(databaseUrl).toArray(function (err, cols) {
        if (err) {
            setStatus(statusEnum.error);
            node.error('Completed with error ' +JSON.stringify(err));
            node.log('Completed with error ' +JSON.stringify(err));
        } else {            
            node.log(cols.length + ' Collections found');
            callback(cols);
        }
    });
}

function deleteCollection(collectionId, callback) {
    var collLink = databaseUrl + '/colls/' + collectionId;
    client.deleteCollection(collLink, function (err) {
        if (err) {
            setStatus(statusEnum.error);
            node.error('Completed with error ' +JSON.stringify(err));
            node.log('Completed with error ' +JSON.stringify(err));
        } else {
            callback();
        }
    });
}

function readCollectionById(collectionId, callback) {
    var collLink = databaseUrl + '/colls/' + collectionId;
    client.readCollection(collLink, function (err, coll) {
        if (err) {
            setStatus(statusEnum.error);
            node.error('Completed with error ' +JSON.stringify(err));
            node.log('Completed with error ' +JSON.stringify(err));;
        } else {
            callback(coll);
        }
    });
}

//---------------------------------------------------------- Documents--------------------------------------------------------------------
function getDocument(document) {
    var documentUrl = `${collectionUrl}/docs/${document.id}`;
    node.log(`Getting document:\n${document.id}\n`);

    return new Promise((resolve, reject) => {
        node.log("trying read");
        client.readDocument(documentUrl, (err, result) => {
            node.log("reading");
            if (err) {
                node.log("error");
                if (err.code == HttpStatusCodes.NOTFOUND) {
                    node.log("creating");
                    client.createDocument(collectionUrl, document, (err, created) => {
                        node.log("try to create");
                        if (err) reject(err)
                        else resolve(created);
                    });
                } else {
                    reject(err);
                }
            } else {
                resolve(result);
            }
        });
    });
};

function deleteDocument(document) {
    var documentUrl = `${collectionUrl}/docs/${document.id}`;
    node.log(`Deleting document:\n${document.id}\n`);

    return new Promise((resolve, reject) => {
        client.deleteDocument(documentUrl, (err, result) => {
            if (err) reject(err);
            else {
                resolve(result);
            }
        });
    });
};

function replaceDocument(document) {
    var documentUrl = `${collectionUrl}/docs/${document.id}`;
    node.log(`Replacing document:\n${document.id}\n`);

    return new Promise((resolve, reject) => {
        client.replaceDocument(documentUrl, document, (err, result) => {
            if (err) reject(err);
            else {
                resolve(result);
            }
        });
    });
};

function queryDocuments(querystring) {
    node.log(`Querying collection through index:\n${collectionName}`);
    querystring = querystring.replace("'", "\"");
    querystring = querystring.replace("'", "\"");
    node.log("Query string -> " + querystring);

    return new Promise((resolve, reject) => {
        client.queryDocuments(
            collectionUrl,
            querystring
        ).toArray((err, results) => {
            if (err) reject(err)
            else {
                node.log("Results -> " + results);
                for (var queryResult of results) {
                    var resultString = JSON.stringify(queryResult);
                    node.log("Query returned " + resultString);
                }
                node.log("Query OK");
                resolve(results);
            }
        });
    });
};

function listDocuments(collLink, callback) {
    var queryIterator = client.readDocuments(collectionUrl).toArray(function (err, docs) {
        if (err) {
            setStatus(statusEnum.error);
            node.error('Completed with error ' +JSON.stringify(err));
            node.log('Completed with error ' +JSON.stringify(err));
        } else {
            node.log(docs.length + ' Documents found');
            callback(docs);
        }
    });
}


//---------------------------------------------------------- GENERAL--------------------------------------------------------------------
    var disconnectFrom = function () { 
         if (client) { 
             node.log('Disconnecting from Azure DocumentDB'); 
             client = null; 
             setStatus(statusEnum.disconnected); 
         } 
     } 

    // Main function called by Node-RED    
    function DocumentDBDatabase(config) {
        // Store node for further use
        node = this;
        nodeConfig = config;

        // Create the Node-RED node
        RED.nodes.createNode(this, config);
        dbendpoint = node.credentials.endpoint;
        dbkey = node.credentials.authkey;

        this.on('input', function (msg) {
            client = new DocumentDBClient(dbendpoint, { "masterKey": dbkey });
            var messageJSON = null;
            if (typeof (msg.payload) != "string") {
                node.log("JSON");
                messageJSON = msg.payload;
            } else {
                node.log("String");
                //Converting string to JSON Object
                //Sample string: {"dbname": "name", "action": "C"}
                messageJSON = JSON.parse(msg.payload);
            }
            var action = messageJSON.action;
            if  (action != "L")
            {
                dbName = messageJSON.dbname;
                databaseUrl = `dbs/${dbName}`;
            }
            // Sending action to Azure DocumentDB
            setStatus(statusEnum.sending);
            switch (action) {
                case "C":
                    node.log('Trying to create database');
                    getDatabase().then((resolve) => { 
                        node.log('Completed successfully: -> ' + JSON.stringify(resolve));
                        setStatus(statusEnum.sent);
                        node.send(messageJSON.dbname); 
                    }).catch((error) => { 
                        setStatus(statusEnum.error);
                        node.error('Completed with error ' +JSON.stringify(error));
                        node.log('Completed with error ' +JSON.stringify(error));
                    });
                    break;
                case "L":
                    node.log('Trying to list databases');
                    var listNames = [];
                    listDatabases(function (dbs) {
                        setStatus(statusEnum.sent);
                        if (dbs.length == 1) {
                            node.send(dbs[0].id)
                        } else {
                            for (var i = 0; i < dbs.length; i++) {
                                listNames.push(dbs[i].id);
                            }
                            node.send(JSON.stringify(listNames));
                        }
                    });
                    break;
                case "D":
                    node.log('Trying to delete database');
                    deleteDatabase().then((resolve) => { 
                        setStatus(statusEnum.sent);
                        node.log('Delete successfully: -> ' + JSON.stringify(resolve));
                     }).catch((error) => { 
                        setStatus(statusEnum.error);
                        node.error('Completed with error ' +JSON.stringify(error));
                        node.log('Completed with error ' +JSON.stringify(error));
                    });
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

    function DocumentDBCollections(config) {
        // Store node for further use
        node = this;
        nodeConfig = config;

        // Create the Node-RED node
        RED.nodes.createNode(this, config);
        dbendpoint = node.credentials.endpoint;
        dbkey = node.credentials.authkey;

        this.on('input', function (msg) {
            //working with collections
            client = new DocumentDBClient(dbendpoint, { "masterKey": dbkey });

            var messageJSON = null;

            if (typeof (msg.payload) != "string") {
                node.log("JSON");
                messageJSON = msg.payload;
            } else {
                node.log("String");
                //Converting string to JSON Object
                //Sample string: {"dbname": "name", "collName": "colletionName", "action": "C"}
                messageJSON = JSON.parse(msg.payload);
            }
            var action = messageJSON.action;
            if  (action != "L")
            {
                dbName = messageJSON.dbname;
                collectionName = messageJSON.collName;
                databaseUrl = `dbs/${dbName}`;
                collectionUrl = `${databaseUrl}/colls/${collectionName}`;
            }
            // Sending action to Azure DocumentDB
            setStatus(statusEnum.sending);
            switch (action) {
                case "C":
                    node.log('Trying to create Collection');
                    getCollection().then((resolve) => { 
                        node.log('Completed successfully ' + JSON.stringify(resolve));
                        setStatus(statusEnum.sent);
                        node.send(collectionName); 
                    }).catch((error) => { 
                        setStatus(statusEnum.error);
                        node.error('Completed with error ' +JSON.stringify(error));
                        node.log('Completed with error ' +JSON.stringify(error));
                    });
                    break;
                case "L":
                    node.log('Trying to list Collections');
                    var listNames = [];
                    listCollections(databaseUrl, function (cols) {
                        setStatus(statusEnum.sent);
                        if (cols.length == 1) {
                            node.send(cols[0].id)
                        } else {
                            for (var i = 0; i < cols.length; i++) {
                                listNames.push(cols[i].id);
                            }
                            node.send(JSON.stringify(listNames));
                        }
                    });
                    break;
                case "D":
                    node.log('Trying to delete Collection');
                    deleteCollection(collectionName, function () {
                        setStatus(statusEnum.sent);
                        node.log('Collection \'' + collectionId + '\'deleted');
                        node.send('Collection \'' + collectionId + '\'deleted');
                    });
                    break;
                case "R":
                    node.log('Trying to read Collection');
                    readCollectionById(collectionName, function (result) {
                        if (result) {
                            setStatus(statusEnum.sent);
                            node.log('Collection with id of \'' + collectionName + '\' was found its _self is \'' + result._self + '\'');
                            node.send(result._self);
                        }
                    });
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

    function DocumentDBDocuments(config) {
        // Store node for further use
        node = this;
        nodeConfig = config;

        // Create the Node-RED node
        RED.nodes.createNode(this, config);
        dbendpoint = node.credentials.endpoint;
        dbkey = node.credentials.authkey;

        this.on('input', function (msg) {
            //working with collections
            client = new DocumentDBClient(dbendpoint, { "masterKey": dbkey });
            node.log("Client started.");

            var messageJSON = null;

            if (typeof (msg.payload) != "string") {
                node.log("JSON");
                messageJSON = msg.payload;
            } else {
                node.log("String");
                //Converting string to JSON Object
                //Sample string: {"dbname": "name", "collName": "colletionName", "action": "C", "doc": {"id": "lucas.1", "firstname": "Lucas", "lastname": "Humenhuk"}}
                //Sample string to QUERY : {"dbname": "name", "collName": "colletionName", "action": "Q", "query" : "SELECT VALUE r.address FROM root r WHERE r.firstName = 'Lucas'"}
                messageJSON = JSON.parse(msg.payload);
            }
            var action = messageJSON.action;
            dbName = messageJSON.dbname;
            collectionName = messageJSON.collName;
            databaseUrl = `dbs/${dbName}`;
            collectionUrl = `${databaseUrl}/colls/${collectionName}`;
            // Sending action to Azure DocumentDB
            setStatus(statusEnum.sending);
            switch (action) {
                case "C":
                    node.log('Trying to create Document');
                    //node.log(JSON.parse(messageJSON.doc));
                    getDocument(messageJSON.doc).then((resolve) => { 
                       node.log('Completed successfully ' + JSON.stringify(resolve));
                        setStatus(statusEnum.sent);
                        node.send('Completed successfully ' + JSON.stringify(resolve));  
                    }).catch((error) => { 
                        setStatus(statusEnum.error);
                        node.error('Completed with error ' +JSON.stringify(error));
                       node.log('Completed with error ' +JSON.stringify(error));
                    });
                    break;
                case "L":
                    node.log('Trying to list Documents');
                    var listNames = [];
                    listDocuments(collectionUrl, function (docs) {
                        setStatus(statusEnum.sent);
                        if (docs.length == 1) {
                            node.send(docs[0].id)
                        } else {
                            for (var i = 0; i < docs.length; i++) {
                                listNames.push(docs[i].id);
                            }
                            node.send(JSON.stringify(listNames));
                        }
                    })
                    break;
                case "D":
                    node.log('Trying to delete Documents');
                    deleteDocument(messageJSON.doc).then((resolve) => { 
                        node.log('Delete successfully ' + JSON.stringify(resolve));
                        setStatus(statusEnum.sent);
                        node.send('Delete successfully ' + JSON.stringify(resolve)); 
                    }).catch((error) => { 
                        setStatus(statusEnum.error);
                        node.error('Delete with error ' +JSON.stringify(error));
                        node.log('Delete with error ' +JSON.stringify(error));
                });
                    break;
                case "U":
                    node.log('Trying to update document');
                    replaceDocument(messageJSON.doc).then((resolve) => { 
                        node.log('Updated successfully ' + JSON.stringify(resolve));
                        setStatus(statusEnum.sent);
                        node.send('Updated successfully ' + JSON.stringify(resolve)); 
                    }).catch((error) => { 
                        setStatus(statusEnum.error);
                        node.error('Updated with error ' +JSON.stringify(error));
                        node.log('Updated with error ' +JSON.stringify(error));
                });
                    break;
                case "Q":
                    node.log('Trying to query document');
                    queryDocuments(messageJSON.query).then((resolve) => { 
                        node.log('Query successfully ' + JSON.stringify(resolve));
                        setStatus(statusEnum.sent);
                        node.send('Query successfully ' + JSON.stringify(resolve)); 
                    }).catch((error) => { 
                        setStatus(statusEnum.error);
                        node.error('Query with error ' +JSON.stringify(error));
                        node.log('Query with error ' +JSON.stringify(error));
                });
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
    RED.nodes.registerType("Database", DocumentDBDatabase, {
        credentials: {
            endpoint: { type: "text" },
            authkey: { type: "text" },
        },
        defaults: {
            name: { value: "Azure DocumentDB - Database" },
        }
    });

    // Registration of the node into Node-RED to manage Collections
    RED.nodes.registerType("Collections", DocumentDBCollections, {
        credentials: {
            endpoint: { type: "text" },
            authkey: { type: "text" },
        },
        defaults: {
            name: { value: "Azure DocumentDB - Collections" },
        }
    });

    // Registration of the node into Node-RED to manage Documents
    RED.nodes.registerType("Documents", DocumentDBDocuments, {
        credentials: {
            endpoint: { type: "text" },
            authkey: { type: "text" },
        },
        defaults: {
            name: { value: "Azure DocumentDB - Documents" },
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