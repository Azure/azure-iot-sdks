'use strict'

const logger = require('./lib').logger;
const async  = require('async');
const config = require('./config');

logger.info('*******************************************')
logger.info('* Executing the Microsoft IOT Trace tool. *');
logger.info('*******************************************')

async.series([
  function(done) {
    logger.info('');
    logger.info('--- Executing network tests ---');
    require('./networktests').run(done);
  },
  function(done) {
    logger.info('');
    logger.info('--- Executing IOT Hub tests ---');

    if(!config.iotHubInfo.hostName || !config.iotHubInfo.sharedAccessKey || !config.iotHubInfo.sharedAccessKeyName) {
      logger.crit('Skipping IotHub tests because no IotHub configuration provided. Enter IOT Hub information in config/config.json.');
      return done();
    } else {
      require('./iothubtests').run(done);
    }
  }
]);