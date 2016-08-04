'use strict'

const logger = require('../lib').logger;
const iotClient = require('./iotClient');
const deviceManager = require('./deviceManager');
const testService = require('./testService');
const async  = require('async');

var deviceId = 'device' + require('node-uuid').v4();

function startTestService(done) {
  logger.trace('Starting test service.');
  testService.open(done);
}

function createDevice(done) {
  logger.trace('Registering device');
  deviceManager.createDevice(deviceId, done);
}

function runTest(deviceConnectionString, protocol, label, done) {
  logger.info('');
  logger.info('Starting ' + label + ' Test...');
  iotClient.runTest(deviceConnectionString, protocol, deviceId, function(err) {
    if(err) {
      logger.crit('--> Failed to run ' + label + ' test, error: ' + err);
    } else {
      logger.info('--> Successfully ran ' + label + ' test.');
    }

    // Don't pass out error (if the test completed, it will just state it failed, but still run the next test)
    return done(null, deviceConnectionString);
  });
}

function amqpTest(deviceConnectionString, done) {
  runTest(deviceConnectionString, require('azure-iot-device-amqp').Amqp, 'AMQP', done);
};

function amqpWsTest(deviceConnectionString, done) {
  runTest(deviceConnectionString, require('azure-iot-device-amqp-ws').AmqpWs, 'AMQP-WS', done);
}

function httpTest(deviceConnectionString, done) {
  runTest(deviceConnectionString, require('azure-iot-device-http').Http, 'HTTP', done);
}

function mqttTest(deviceConnectionString, done) {
  runTest(deviceConnectionString, require('azure-iot-device-mqtt').Mqtt, 'Mqtt', done);
};

function cleanup(deviceConnectionString) {
  testService.close();
  deviceManager.deleteDevice(deviceId, function () {});
}

function run(done) {
  async.waterfall([
      // Step 1, start event hub reader
      startTestService,
      // Step 2, create the device
      createDevice,
      // Step 3, run the tests
      amqpTest,
      amqpWsTest,
      httpTest,
      mqttTest,
      // Step 4, cleanup
      cleanup,
    ],
    done);
};

module.exports = {
  run: run
};
