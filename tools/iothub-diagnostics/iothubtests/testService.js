'use strict';

const logger = require('../lib').logger;
const config = require('../config');
const EventHubClient = require('azure-event-hubs').Client;
const ServiceClient = require('azure-iothub').Client;
const Message = require('azure-iot-common').Message;

var connectionString = config.iotHubInfo.connectionString;
var serviceConnection = ServiceClient.fromConnectionString(connectionString);
var eventHubConnection = EventHubClient.fromConnectionString(connectionString);

// Called whenever an error occurs in either the message callback or the  eventhub connection setup
function errorCb(err) {
  logger.crit(err.message);
};

// Called whenever we receive a telemetry message from the client
function messageReceivedCb(message) {
  logger.debug('Service successfully received telemetry from client.');
  logger.trace(JSON.stringify(message));
  var targetDevice = message.body;

  if(!targetDevice) {
    logger.crit('Client telemetry message did not contain the device, unable to respond.');
  } else {
    serviceConnection.send(targetDevice, JSON.stringify(config.testCommand));
  }
};

// Called to start the service
function open(ready) {
  serviceConnection.open(function(err) {
    if (err) {
      logger.fatal('Unable to open connection to Eventhub: ' + err.message);
      return;
    }

    logger.trace('Test service open.');

    eventHubConnection.open()
      .then(eventHubConnection.getPartitionIds.bind(eventHubConnection))
      .then(function (partitionIds) {
        return partitionIds.map(function (partitionId) {
          return eventHubConnection.createReceiver('$Default', partitionId, { 'startAfterTime' : Date.now()}).then(function(receiver) {
            logger.trace('Created partition receiver: ' + partitionId)
            receiver.on('errorReceived', errorCb);
            receiver.on('message', messageReceivedCb);
            });
        });
      })
      .then(function() { ready() })
      .catch(errorCb);
  });
};

// Closes the connection to the eventhub and to the DeviceClient.
function close() {
  eventHubConnection.close();
  serviceConnection.close();
}

module.exports = {
  open: open,
  close: close
};
