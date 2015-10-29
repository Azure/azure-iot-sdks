// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var amqp10 = require('amqp10');
var Promise = require('bluebird');

var EventHubReceiver = require('./eventhubreceiver');
var ServiceToken = require('azure-iot-common').authorization.ServiceToken;

var managementEndpoint = '$management';

function parseEventHubConnString(connString)
{
  var config = {
    host: '',
    eventHubName: '',
    keyName: '',
    key: ''
  };
  
  var configArray = connString.split(';');
  configArray.forEach(function(element) {
    var res = element.match('Endpoint=sb://([^/]*)');
    if (res !== null) {
        config.host = res[1];
        res = config.host.split('.');
        config.namespace = res[0];
    }
    res = element.match('HostName=(.*)');
    if (res !== null) {
        config.host = res[1];
        res = config.host.split('.');
        config.namespace = res[0];
    }
    res = element.match('SharedAccessKeyName=(.*)');
    if (res !== null) {
        config.keyName = res[1];
    }
    res = element.match('SharedAccessKey=(.*)');
    if (res !== null) {
        config.key = res[1];
    }
    res = element.match('EntityPath=(.*)');
    if (res !== null) {
        config.eventHubName = res[1];
    }
  });
  return config;
}

function anHourFromNow () {
  var raw = (Date.now() / 1000) + 3600;
  return Math.ceil(raw);
}


/**
 * @class EventHubClient
 * @classdesc Constructs a {@linkcode EventHubClient} object with the given connection string
 * @param {String}  connString    The EventHub connection string
 * @param {String}  path          The EventHub path. Optional.
 */
function EventHubClient(connString, path) {
  this.config = parseEventHubConnString(connString);
  
  this.connectPromise = null;
  
  if (path) {
    this.config.eventHubName = path;
  }
  
  if (!this.config.eventHubName || this.config.eventHubName.length === 0)
  {
    throw new Error('No event hub name specified');
  }

  this.config.actualHost = this.config.host;
  var token = new ServiceToken(this.config.actualHost, this.config.keyName, this.config.key, anHourFromNow());
  var strToken = token.toString();

  this.uri = 'amqps://' +
    encodeURIComponent(this.config.keyName + '@sas.root.' + this.config.namespace) + ':' +
    encodeURIComponent(strToken) + '@' +
    this.config.actualHost;

  this.amqpClient = new amqp10.Client(amqp10.Policy.EventHub);
}

/**
 * The [Send]{@linkcode EventHubClient#Send} method sends one Event to the Event Hub.
 */
EventHubClient.prototype.Send = function(event) {
    /* Not implemented yet */
    throw new Error('Not implemented (trying to send event:' + JSON.stringify(event) + ')');
};

/**
 * The [SendBatch]{@linkcode EventHubClient#SendBatch} method sends a batch of Events to the Event Hub.
 */
EventHubClient.prototype.SendBatch = function(events) {
    /* Not implemented yet */
    throw new Error('Not implemented (trying to send events:' + JSON.stringify(events) + ')');
};

EventHubClient.prototype._connect = function() {
  var self = this;
  if (!this.connectPromise) {
    this.connectPromise = new Promise(function(resolve, reject) {
      self.amqpClient.connect(self.uri).then(function() {
        var rxName = 'rx-name';
        var rxOptions = { attach: { target: { address: rxName } } };
        self.amqpClient.createReceiver(self.config.eventHubName, rxOptions).then(function(receiver) {
          receiver.on('errorReceived', function (rx_err) {
            if (rx_err.condition.contents === 'amqp:link:redirect')
            {
              self.config.actualHost = rx_err.errorInfo.hostname;
              var res = rx_err.errorInfo.address.match('amqps://([^/]*)/([^/]*)');
              self.config.eventHubName = res[2];
              self.endpointPrefix = '';
              self.uri = 'amqps://' +
                encodeURIComponent(self.config.keyName) + ':' +
                encodeURIComponent(self.config.key) + '@' +
                self.config.actualHost;
              self.amqpClient.disconnect().then(function() {
                self.amqpClient = new amqp10.Client(amqp10.Policy.EventHub);
                self.amqpClient.connect(self.uri).then(function() {
                  resolve();
                });
              });
            }
            else
            {
              reject(new Error('error receiving reply from Event Hub management endpoint: ' + rx_err.description));
            }
          });
          receiver.on('attach', function () {
            console.log("attached");
            resolve();
          });
        });
      });
    });
  }
  
  return this.connectPromise;
};

/**
 * The [GetPartitionIds]{@linkcode EventHubClient#GetPartitionIds} method gets the partition Ids for an EventHub.
 */
EventHubClient.prototype.GetPartitionIds = function() {
    var self = this;
    return new Promise(function (resolve, reject) {
        var rxName = 'eventhubclient-rx';
        var rxOptions = { attach: { target: { address: rxName } } };

        self._connect().then(function () {
            /* create a receiver for the management endpoint to receive the partition count */
            return self.amqpClient.createReceiver(managementEndpoint, rxOptions);
        }).then(function (receiver) {
            receiver.on('errorReceived', function (rx_err) {
                reject(new Error('error receiving reply from Event Hub management endpoint: ' + rx_err));
            });
            receiver.on('message', function (msg) {
                resolve(msg.body.partition_ids);
            });
            /* create a sender to send the request to the $management endpoint */
            return self.amqpClient.createSender(managementEndpoint);
        }).then(function (sender) {
            sender.on('errorReceived', function (tx_err) {
                reject(new Error('error sending request to Event Hub management endpoint: ' + tx_err));
            });
            var request = { body: 'stub', properties: { messageId: '424242', replyTo: rxName }, applicationProperties: { operation: 'READ', name: self.config.eventHubName, type: 'com.microsoft:eventhub' } };
            return sender.send(request);
        });
    });
};

/**
 * The [CreateReceiver]{@linkcode EventHubClient#CreateReceiver} method creates a new 
 * {@linkcode EventHubReceiver} instance.
 * @param {String}  consumerGroup  The consumer group to use for the new receiver.
 * @param {String}  partitionId    The partition Id to use for the new receiver.
 */
EventHubClient.prototype.CreateReceiver = function(consumerGroup, partitionId) {
  var self = this;
  return new Promise(function(resolve) {
    self._connect().then(function() {
  	   resolve(new EventHubReceiver(self.amqpClient, self.config.eventHubName + '/ConsumerGroups/' + consumerGroup + '/Partitions/' + partitionId));
    });
  });
};

module.exports = EventHubClient;
