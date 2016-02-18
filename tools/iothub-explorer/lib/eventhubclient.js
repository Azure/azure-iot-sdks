// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var amqp10 = require('amqp10');
var Promise = require('bluebird');

var anHourFromNow = require('azure-iot-common').anHourFromNow;
var ConnectionString = require('azure-iot-common').ConnectionString;
var EventHubReceiver = require('./eventhubreceiver.js');
var SharedAccessSignature = require('azure-iothub').SharedAccessSignature;

var managementEndpoint = '$management';

function createConfig(connectionString, entityPath) {
  var cn = ConnectionString.parse(connectionString);
  var host = cn.HostName || (cn.Endpoint || '').slice('sb://'.length);
  return {
    host: host,
    namespace: host.split('.')[0],
    keyName: cn.SharedAccessKeyName,
    key: cn.SharedAccessKey,
    eventHubName: entityPath
  };
}

/**
 * @class EventHubClient
 * @classdesc Constructs a {@linkcode EventHubClient} object with the given connection string
 * @param {String}  connString    The EventHub connection string
 * @param {String}  path          The EventHub path. Optional.
 */
function EventHubClient(connString, path) {
  this.config = createConfig(connString, path);

  this.connectPromise = null;

  if (!this.config.eventHubName) {
    throw new Error('No event hub name specified');
  }

  var token = SharedAccessSignature.create(this.config.host, this.config.keyName, this.config.key, anHourFromNow()).toString();

  this.uri = 'amqps://' +
  encodeURIComponent(this.config.keyName + '@sas.root.' + this.config.namespace) + ':' +
  encodeURIComponent(token) + '@' +
  this.config.host;

  this.amqpClient = new amqp10.Client(amqp10.Policy.EventHub);
}

EventHubClient.prototype._connect = function () {
  var self = this;
  if (!this.connectPromise) {
    this.connectPromise = new Promise(function (resolve, reject) {
      self.amqpClient.connect(self.uri)
        .then(function () {
          var rxOptions = { attach: { target: { address: 'rx-name' } } };
          return self.amqpClient.createReceiver(self.config.eventHubName, rxOptions);
        })
        .then(function (receiver) {
          receiver.on('errorReceived', function (rx_err) {
            if (rx_err.condition.contents === 'amqp:link:redirect') {
              var res = rx_err.errorInfo.address.match('amqps://([^/]*)/([^/]*)');
              self.config.eventHubName = res[2];
              self.uri = 'amqps://' +
              encodeURIComponent(self.config.keyName) + ':' +
              encodeURIComponent(self.config.key) + '@' +
              rx_err.errorInfo.hostname;
              self.amqpClient.disconnect()
                .then(function () {
                  self.amqpClient = new amqp10.Client(amqp10.Policy.EventHub);
                  return self.amqpClient.connect(self.uri);
                })
                .then(function () {
                  resolve();
                });
            }
            else {
              reject(new Error('error receiving reply from Event Hub management endpoint: ' + rx_err.description));
            }
          });
          receiver.on('attach', function () {
            console.log("attached");
            resolve();
          });
        });
    });
  }

  return this.connectPromise;
};

/**
 * The [GetPartitionIds]{@linkcode EventHubClient#GetPartitionIds} method gets the partition Ids for an EventHub.
 */
EventHubClient.prototype.GetPartitionIds = function () {
  var self = this;
  return new Promise(function (resolve, reject) {
    var rxName = 'eventhubclient-rx';
    var rxOptions = { attach: { target: { address: rxName } } };

    self._connect()
      .then(function () {
        /* create a receiver for the management endpoint to receive the partition count */
        return self.amqpClient.createReceiver(managementEndpoint, rxOptions);
      })
      .then(function (receiver) {
        receiver.on('errorReceived', function (rx_err) {
          reject(new Error('error receiving reply from Event Hub management endpoint: ' + rx_err));
        });
        receiver.on('message', function (msg) {
          resolve(msg.body.partition_ids);
        });
        /* create a sender to send the request to the $management endpoint */
        return self.amqpClient.createSender(managementEndpoint);
      })
      .then(function (sender) {
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
EventHubClient.prototype.CreateReceiver = function (consumerGroup, partitionId) {
  var self = this;
  return new Promise(function (resolve) {
    self._connect().then(function () {
      var endpoint = self.config.eventHubName +
        '/ConsumerGroups/' + consumerGroup +
        '/Partitions/' + partitionId;
  	   resolve(new EventHubReceiver(self.amqpClient, endpoint));
    });
  });
};

module.exports = EventHubClient;
