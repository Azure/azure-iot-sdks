// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var fs = require('fs');
var path = require('path');
var util = require('util');
var async = require('async');
var xpath = require('xpath');
var DOMParser = require('xmldom').DOMParser;
var XMLSerializer = require('xmldom').XMLSerializer;
var versions = require('./versions.json');
var inputs = require('./inputs');
var colors = require('colors');
var basePath = path.normalize(__dirname + '/../../..');

function main() {
    // process all inputs
    async.each(inputs, runTask, function(err) {
        if(err) {
            console.error(util.format('[%s]: %s', 'error'.red, err));
        }
    });
}

function runTask(input, callback) {
    // find out what task to run
    var taskRunner = TaskRunners[input.taskType];

    // if replaceString is a function then invoke it to
    // compute the string that is to be used
    if(input.replaceString) {
        if (typeof(input.replaceString) === 'function') {
            input.replaceString = input.replaceString(versions);
        }
        // if replaceString is a string then it's a property path
        // in the 'versions' object
        else if (typeof(input.replaceString) === 'string') {
            input.replaceString = PropPaths.getValue(versions, input.replaceString);
        }
    }

    // invoke the task runner
    taskRunner.call(
        null,
        path.join(basePath, input.filePath),
        input.search,
        input.replaceString,
        function(err) {
            if(!err) {
                console.log(util.format('[%s] processed %s',
                    'ok'.green, input.filePath.gray));
            }
            else {
                console.log(util.format('[%s] failed to process %s',
                    'err'.red, input.filePath.gray));
            }

            callback(err);
        },
        input
    );
}

var PropPaths = {
    _getProp: function(obj, propPath) {
        // propPath is a dot separated list of property names that
        // we are to use to navigate to the property in question whose
        // value needs to be retrieved
        var props = propPath.split('.');
        var targetProp = obj;
        while(props.length > 1) {
            targetProp = targetProp[props.shift()];
        }
        
        return {
            targetProp: targetProp,
            propName: props[0]
        };
    },
    
    setValue: function(obj, propPath, val) {
        // replace the prop value
        var result = this._getProp(obj, propPath);
        result.targetProp[result.propName] = val;
    },
    
    getValue: function(obj, propPath) {
        var result = this._getProp(obj, propPath);
        return result.targetProp[result.propName];
    }
};

var TaskRunners = {
    multiTask: function(filePath, search, replace_string, callback) {
        // search is an array with sub-tasks that need to be run
        // in series
        async.eachSeries(search, function(item, cb) {
            // build the input that 'runTask' expects
            var input = item;
            input.filePath = path.relative(basePath, filePath);

            // run the task!
            runTask(input, cb);
        }, callback);
    },

    jsonReplaceTask: function(filePath, propPath, replace_string, callback) {
        async.waterfall([
            function(cb) {
                fs.readFile(filePath, 'utf-8', cb);
            },
            function(data, cb) {
                // replace the prop value
                var obj = JSON.parse(data);
                PropPaths.setValue(obj, propPath, replace_string);

                // write the file back out
                fs.writeFile(filePath, JSON.stringify(obj, null, 2), cb);
            }
        ], callback);
    },
    
    xmlReplaceTask: function(filePath, selector, replace_string, callback, input) {
        async.waterfall([
            function(cb) {
                fs.readFile(filePath, 'utf-8', cb);
            },
            function(data, cb) {
                var doc = new DOMParser().parseFromString(data, 'text/xml');
                var select = xpath.select;
                if(input.nsmap) {
                    select = xpath.useNamespaces(input.nsmap);
                }

                // we expect selectors to match only one node
                var nodes = select(selector, doc);
                if(!!nodes && nodes.length > 0) {
                    var textNode = nodes[0].childNodes[0];
                    textNode.replaceData(0, textNode.data.length, replace_string);
    
                    // write the file back out
                    var serializer = new XMLSerializer();
                    var xml = serializer.serializeToString(doc);
                    fs.writeFile(filePath, xml, cb);
                }
                else {
                    cb(null);
                }
            }
        ], callback);
    },

    xmlAttributeReplaceTask: function(filePath, selector, replace_string, callback, input) {
        async.waterfall([
            function(cb) {
                fs.readFile(filePath, 'utf-8', cb);
            },
            function(data, cb) {
                var doc = new DOMParser().parseFromString(data, 'text/xml');

                var select = xpath.select;
                if(input.nsmap) {
                    select = xpath.useNamespaces(input.nsmap);
                }

                // we expect selectors to match only one node
                var node = select(selector, doc);
                if(!!node && node.length > 0) {
                    node[0].value = replace_string;

                    // write the file back out
                    var serializer = new XMLSerializer();
                    var xml = serializer.serializeToString(doc);
                    fs.writeFile(filePath, xml, cb);
                }
                else {
                    cb(null);
                }
            }
        ], callback);
    },
    
    regexReplaceTask: function(filePath, regex, replace_string, callback) {
        async.waterfall([
            function(cb) {
                fs.readFile(filePath, 'utf-8', cb);
            },
            function(data, cb) {
                // replace the string in question
                data = data.replace(new RegExp(regex, 'gm'), replace_string);
                
                // write the file back out
                fs.writeFile(filePath, data, cb);
            }
        ], callback);
    }
};

main();
