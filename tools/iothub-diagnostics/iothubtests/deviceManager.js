'use strict'

const config = require('../config');
const logger = require('../lib').logger;
const iothub = require('azure-iothub');
const Registry = iothub.Registry;

var registry = Registry.fromConnectionString(config.iotHubInfo.connectionString);

// Create a new device
function createDevice(deviceId, done) {
 var device = {
   deviceId: deviceId,
   status: 'enabled'
 };
 logger.trace("Creating device '" + device.deviceId + "'");
 
 registry.create(device, function(err, deviceInfo, res) {
   if (err) {
     logger.fatal('Unable to register device, error: ' + err.toString());
     return done(err);
   }

   logger.trace('Device creation status code: ' + res.statusCode + ' ' + res.statusMessage);
   logger.trace('Created device: ' + JSON.stringify(deviceInfo));

   var deviceConnectionString = 'HostName=' + config.iotHubInfo.hostName + ';DeviceId=' + deviceInfo.deviceId + ';SharedAccessKey=' + deviceInfo.authentication.symmetricKey.primaryKey;
   logger.trace('Connectionstring: ' + deviceConnectionString);

   return done(null, deviceConnectionString);
 });
};

// Delete a device
function deleteDevice(deviceId, done) {
  registry.delete(deviceId, function(err, deviceInfo, res) {
    logger.trace('Deleted device ' + deviceId);
    return done(err);
  });
}

module.exports = {
  createDevice: createDevice,
  deleteDevice: deleteDevice
}
