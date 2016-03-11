// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';
var serviceSdk = require('azure-iothub');
var deviceSdk = require('azure-iot-device');
var deviceAmqp = require('azure-iot-device-amqp');
var deviceHttp = require('azure-iot-device-http');
var Message = require('azure-iot-common').Message;
var deviceSas = require('azure-iot-device').SharedAccessSignature;
var serviceSas = require('azure-iothub').SharedAccessSignature;
var anHourFromNow = require('azure-iot-common').anHourFromNow;

var EventHubClient = require('./eh/eventhubclient.js');

var ConnectionString = require('azure-iot-common').ConnectionString;

var assert = require('chai').assert;
var debug = require('debug')('e2etests');
var uuid = require('uuid');

var runTests = function (DeviceTransport, hubConnStr, deviceConStr, deviceName) {
  describe('Device connected over ' + DeviceTransport.name + ':', function () {

    var serviceClient, deviceClient, ehClient;

    beforeEach(function () {
      serviceClient = serviceSdk.Client.fromConnectionString(hubConnStr);
      deviceClient = deviceSdk.Client.fromConnectionString(deviceConStr, DeviceTransport);
      ehClient = new EventHubClient(hubConnStr, 'messages/events/');
    });

    afterEach(function (done) {
      serviceClient.close(function (err) {
        if (err) {
          done(err);
        } else {
          serviceClient = null;
          debug('Connection to the service closed.');
          deviceClient.close(function (err) {
            if (err) {
              done(err);
            } else {
              deviceClient = null;
              debug('Connection as the device closed.');
              ehClient.close().then(function () {
                ehClient = null;
                debug('Event hub connection closed');
                done();
              }).catch(function (err) {
                done(err);
              });
            }
          });
        }
      });
    });

    it('Service sends 1 C2D message and it is re-sent until the device completes it', function (done) {
      this.timeout(60000);
      var guid = uuid.v4();

      var abandonnedOnce = false;
      deviceClient.open(function (openErr) {
        if (openErr) {
          done(openErr);
        } else {
          deviceClient.on('message', function (msg) {
            debug('Received a message with guid: ' + msg.data);
            if (msg.data === guid) {
              if (!abandonnedOnce) {
                debug('Abandon the message with guid ' + msg.data);
                abandonnedOnce = true;
                deviceClient.abandon(msg, function (err, result) {
                  assert.isNull(err);
                  assert.equal(result.constructor.name, 'MessageAbandoned');
                });
              } else {
                debug('Complete the message with guid ' + msg.data);
                deviceClient.complete(msg, function (err, res) {
                  assert.isNull(err);
                  assert.equal(res.constructor.name, 'MessageCompleted');
                  done();
                });
              }
            } else {
              debug('not the message I\'m looking for, completing it to clean the queue (' + msg.data + ')');
              deviceClient.complete(msg, function (err, result) {
                assert.isNull(err);
                assert.equal(result.constructor.name, 'MessageCompleted');
              });
            }
          });
        }
      });

      serviceClient.open(function (serviceErr) {
        if (serviceErr) {
          done(serviceErr);
        } else {
          serviceClient.send(deviceName, guid, function (sendErr) {
            assert.isNull(sendErr);
            debug('Sent one message with guid: ' + guid);
          });
        }
      });
    });

    it('Service sends 1 C2D message and it is re-sent until the device rejects it', function (done) {
      this.timeout(60000);
      var guid = uuid.v4();

      var abandonnedOnce = false;
      deviceClient.open(function (openErr) {
        if (openErr) {
          done(openErr);
        } else {
          deviceClient.on('message', function (msg) {
            debug('Received a message with guid: ' + msg.data);
            if (msg.data === guid) {
              if (!abandonnedOnce) {
                debug('Abandon the message with guid ' + msg.data);
                abandonnedOnce = true;
                deviceClient.abandon(msg, function (err, result) {
                  assert.isNull(err);
                  assert.equal(result.constructor.name, 'MessageAbandoned');
                });
              } else {
                debug('Rejects the message with guid ' + msg.data);
                deviceClient.reject(msg, function (err, res) {
                  assert.isNull(err);
                  assert.equal(res.constructor.name, 'MessageRejected');
                  done();
                });
              }
            } else {
              debug('not the message I\'m looking for, completing it to clean the queue (' + msg.data + ')');
              deviceClient.complete(msg, function (err, result) {
                assert.isNull(err);
                assert.equal(result.constructor.name, 'MessageCompleted');
              });
            }
          });
        }
      });

      serviceClient.open(function (serviceErr) {
        if (serviceErr) {
          done(serviceErr);
        } else {
          serviceClient.send(deviceName, guid, function (sendErr) {
            assert.isNull(sendErr);
            debug('Sent one message with guid: ' + guid);
          });
        }
      });
    });

    it('Service sends 5 C2D messages and they are all received by the device', function (done) {
      this.timeout(60000);
      var deviceMessageCounter = 0;

      deviceClient.open(function (openErr) {
        if (openErr) {
          done(openErr);
        } else {
          deviceClient.on('message', function (msg) {
            deviceMessageCounter++;
            debug('Received ' + deviceMessageCounter + ' message(s)');
            deviceClient.complete(msg, function (err, result) {
              assert.isNull(err);
              assert.equal(result.constructor.name, 'MessageCompleted');
            });

            if (deviceMessageCounter === 5) {
              done();
            }
          });
        }
      });

      serviceClient.open(function (serviceErr) {
        if (serviceErr) {
          done(serviceErr);
        } else {
          var msgSentCounter = 0;
          var sendCallback = function (sendErr) {
            assert.isNull(sendErr);
            msgSentCounter++;
            debug('Sent ' + msgSentCounter + ' message(s)');
          };

          for (var i = 0; i < 5; i++) {
            debug('Sending message #' + i);
            var msg = new Message({ 'counter': i });
            msg.expiryTimeUtc = Date.now() + 10000; // Expire 10s from now, to reduce the chance of us hitting the 50-message limit on the IoT Hub
            serviceClient.send(deviceName, msg, sendCallback);
          }
        }
      });
    });

    it('Device sends a message and it is received by the service', function (done) {
      this.timeout(60000);
      var guid = uuid.v4();
      debug('GUID for message: ' + guid);
      var startTime = Date.now();

      deviceClient.open(function (openErr) {
        if (openErr) {
          done(openErr);
        } else {
          var message = new Message(guid);
          deviceClient.sendEvent(message, function (sendErr) {
            assert.isNull(sendErr);
            debug('Message with guid ' + guid + ' sent at ' + Date.now());
          });
        }
      });

      ehClient.GetPartitionIds().then(function (partitionIds) {
        partitionIds.forEach(function (partitionId) {
          ehClient.CreateReceiver('$Default', partitionId).then(function (receiver) {
            // start receiving
            receiver.StartReceive(startTime).then(function () {
              receiver.on('error', function (error) {
                done(error);
              });
              receiver.on('eventReceived', function (eventData) {
                if ((eventData.SystemProperties['iothub-connection-device-id'] === deviceName) &&
                  (eventData.SystemProperties['x-opt-enqueued-time'] >= startTime - 5000)) {
                  debug('Event received: ' + eventData.Bytes);
                  if (eventData.Bytes === guid) {
                    receiver.removeAllListeners();
                    ehClient.close();
                    done();
                  } else {
                    debug('eventData.Bytes: ' + eventData.Bytes + ' doesn\'t match ' + guid);
                  }
                }
              });
            });
            return receiver;
          }).catch(function (err) {
            done(err);
          });
        });
        return partitionIds;
      }).catch(function (err) {
        done(err);
      });
    });
  });

  describe('Using a SAS token over ' + DeviceTransport.name + ':', function() {
    it('Device can connect and send a message', function(done) {
      this.timeout(30000);
      var sas = deviceSas.create(host, deviceName, deviceKey, anHourFromNow()).toString();
      var client = deviceSdk.Client.fromSharedAccessSignature(sas, DeviceTransport);
      client.open(function(openErr, openResult) {
        if(openErr) {
          done(openErr);
        } else {
          assert.equal(openResult.constructor.name, 'Connected');
          var msg = new Message('foo');
          client.sendEvent(msg, function(sendErr, sendResult) {
            if(sendErr) {
              done(sendErr);
            } else {
              assert.equal(sendResult.constructor.name, 'MessageEnqueued');
              client.close(function(closeErr) {
                if (closeErr) {
                  done(closeErr);
                } else {
                  done();
                }
              });
            }
          })
        }
      });
    });

    it('Service can connect', function(done) {
      this.timeout(30000);
      var connStr = serviceSdk.ConnectionString.parse(hubConnectionString);
      var sas = serviceSas.create(connStr.HostName, connStr.SharedAccessKeyName, connStr.SharedAccessKey, anHourFromNow()).toString();
      var client = serviceSdk.Client.fromSharedAccessSignature(sas);
      client.open(function(err, result) {
        if(err) {
          done(err);
        } else {
          assert.equal(result.constructor.name, 'Connected');
          client.close(function(err) {
            if (err) {
              done(err);
            } else {
              done();
            }
          });
        }
      });
    });
  });
};

module.exports = runTests;

var deviceName = process.env.IOTHUB_DEVICE_ID;
var deviceKey = process.env.IOTHUB_DEVICE_KEY;
var hubConnectionString = process.env.IOTHUB_CONNECTION_STRING;
var host = ConnectionString.parse(process.env.IOTHUB_CONNECTION_STRING).HostName;
var deviceConnectionString = 'HostName=' + host + ';DeviceId=' + deviceName + ';SharedAccessKey=' + deviceKey;


runTests(deviceAmqp.Amqp, hubConnectionString, deviceConnectionString, deviceName);
runTests(deviceHttp.Http, hubConnectionString, deviceConnectionString, deviceName);
