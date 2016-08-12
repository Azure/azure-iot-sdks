'use strict';

const logger = require('../lib').logger;
const config = require('../config')
const iothub = require('azure-iothub');
const Message = require('azure-iot-device').Message;

var runTest = function(deviceConnectionString, protocol, deviceId, done) {
  var client = require('azure-iot-device').Client.fromConnectionString(deviceConnectionString, protocol);

  logger.trace('Opening the client.');

  // Connect to the service
  client.open(function (err) {
    if (err) {
      logger.fatal('Could not connect to IOT HUB: ' + err);
      return done(err);
    } else {
      logger.debug('Client connected, sending message');        

      // Listen for messages
      client.on('message', function (msg) {
        client.complete(msg, function(err, res) {
          if (err) logger.crit('Error completing message: ' + err.toString());
          return client.close(done);
        });

        if(JSON.parse(msg.data) != config.testCommand) {
          // ignore any error, already failed dont want log further errors and confuse logs
          client.close(function(err) {});

          return done(new Error("Incorrect command received from service '" + msg.data + "'"));
        } else {
          logger.debug('Client received command from service');
        }
      });

      // Send initial telemetry event
      var msg = new Message(deviceId);
      client.sendEvent(msg, function(err, res) {
        if(err) logger.crit('Error sending telemetry: ' + err);
        if(res) logger.debug('Telemetry sent, status: ' + res.constructor.name);
      });
    }
  });
}

module.exports.runTest = runTest;
