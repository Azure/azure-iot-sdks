// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Registry = require('azure-iothub').Registry;
var ServiceClient = require('azure-iothub').Client;
var ConnectionString = require('azure-iothub').ConnectionString;
var deviceSdk = require('azure-iot-device');
var util = require('util');
var deviceMqtt = require('azure-iot-device-mqtt').Mqtt;
var uuid = require('uuid');
var assert = require('chai').assert;
var debug = require('debug')('e2etests:devicemethod');

module.exports = function(hubConnectionString) {
  describe('Device Method', function() {
    this.timeout(120000);
    var deviceClient;
    var deviceDescription = {
        deviceId:  '0000e2etest-delete-me-node-device-method-' + uuid.v4(),
        status: 'enabled',
          authentication: {
          symmetricKey: {
            primaryKey: new Buffer(uuid.v4()).toString('base64'),
            secondaryKey: new Buffer(uuid.v4()).toString('base64')
          }
        }
      };

    // create a new device for every test
    beforeEach(function (done) {
      this.timeout(20000);

      var registry = Registry.fromConnectionString(hubConnectionString);
      registry.create(deviceDescription, function (err) {
        if (err) return done(err);
        debug('created test device: ' + deviceDescription.deviceId);
        var host = ConnectionString.parse(hubConnectionString).HostName;
        deviceClient = deviceSdk.Client.fromConnectionString(
          util.format('HostName=%s;DeviceId=%s;SharedAccessKey=%s',
            host, deviceDescription.deviceId,
            deviceDescription.authentication.symmetricKey.primaryKey
          ),
          deviceMqtt
        );
        deviceClient.open(done);
      });
    });

    // nuke the test device after every test
    afterEach(function (done) {
      this.timeout(20000);
      if (!!deviceClient) {
        deviceClient.close(function(err) {
          if (!!err) {
            console.warn('Could not close connection to device ' +
              deviceDescription.deviceId + '. Deleting device anyway.'
            );
          }

          debug('deleting test device: ' + deviceDescription.deviceId);
          Registry
            .fromConnectionString(hubConnectionString)
            .delete(deviceDescription.deviceId, done);
        });
      } else {
        done();
      }
    });

    [null, '', 'foo', { k1: 'v1' }, {}].forEach(function(testPayload) {
      it('makes and receives a method call', function(done) {
        debug('---------- new method test ------------');
        var methodName = 'method1';
        var methodResult = 200;

        // setup device to handle the method call
        deviceClient.onDeviceMethod(methodName, function(request, response) {
          // validate request
          assert.isNotNull(request);
          assert.strictEqual(request.methodName, methodName);
          assert.deepEqual(request.payload, testPayload);
          debug('device: received method request');
          debug(JSON.stringify(request, null, 2));
          // validate response
          assert.isNotNull(response);

          // send the response
          response.send(methodResult, testPayload, function(err) {
            debug('send method response with statusCode: ' + methodResult);
            if(!!err) {
              console.error('An error ocurred when sending a method response:\n' +
                  err.toString());
            }
          });
        });

        setTimeout(function() {
          // make the method call via the service
          var methodParams = {
            methodName: methodName, 
            payload: testPayload,
            timeoutInSeconds: 20
          };
          debug('service sending method call:');
          debug(JSON.stringify(methodParams, null, 2));
          ServiceClient
              .fromConnectionString(hubConnectionString)
              .invokeDeviceMethod(
                  deviceDescription.deviceId,
                  methodParams,
                  function(err, result) {
                    if(!err) {
                      debug('got method results');
                      debug(JSON.stringify(result, null, 2));
                      assert.strictEqual(result.status, methodResult);
                      assert.deepEqual(result.payload, testPayload);
                    }
                    debug('---------- end method test ------------');
                    done(err);
                  });
        }, 1000);
      });
    });
  });
};
