// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var deviceMqtt = require('azure-iot-device-mqtt').Mqtt;
var Registry = require('azure-iothub').Registry;
var _ = require('lodash');
var deviceSas = require('azure-iot-device').SharedAccessSignature;
var anHourFromNow = require('azure-iot-common').anHourFromNow;
var deviceSdk = require('azure-iot-device');
var uuid = require('uuid');
var ConnectionString = require('azure-iothub').ConnectionString;

var newProps = {
  foo : 1,
  bar : {
    baz : 2,
    tweedle : {
      dee : 3
    } 
  }
};

var moreNewProps = {
  bar : {
    baz : 3
  }
};

var mergeResult =  _.merge(newProps, moreNewProps);

var runTests = function (hubConnectionString) {
  describe('Device Twin', function() {

    this.timeout(120000);
    var deviceClient, deviceTwin;
    var serviceTwin;

    var deviceDescription;

    beforeEach(function (done) {
      this.timeout(20000);

      deviceDescription = {
        deviceId:  '0000e2etest-delete-me-twin-e2e-' + uuid.v4(),
        status: 'enabled',
          authentication: {
          SymmetricKey: {
            primaryKey: new Buffer(uuid.v4()).toString('base64'),
            secondaryKey: new Buffer(uuid.v4()).toString('base64')
          }
        }
      };

      var registry = Registry.fromConnectionString(hubConnectionString);

      registry.create(deviceDescription, function (err) {
        if (err) throw err;

        var host = ConnectionString.parse(hubConnectionString).HostName;
        var deviceConnectionString = deviceSas.create(host, deviceDescription.deviceId, deviceDescription.authentication.SymmetricKey.primaryKey, anHourFromNow()).toString();

        deviceClient = deviceSdk.Client.fromSharedAccessSignature(deviceConnectionString, deviceMqtt);

        deviceClient.open(function(err) {
          if (err) throw err;
          deviceClient.getDeviceTwin(function(err, twin) {
            if (err) throw err;
            deviceTwin = twin;

            registry.getDeviceTwin(deviceDescription.deviceId, function(err, twin) {
              if (err) throw err;
              serviceTwin = twin;
              done();
            });
          });
        });
      });
    });

    afterEach(function (done) {
      this.timeout(20000);
      if (deviceClient) {
        deviceClient.close(function(err) {
          if (err) throw err;
            
          var registry = Registry.fromConnectionString(hubConnectionString);
          registry.delete(deviceDescription.deviceId, function(err) {
            if (err) throw err;
            done();
          });
        });
      } else {
        done();
      }
    });

    var assertObjectIsEmpty = function(props) {
      _.every(_.keys(props), function(key) {
        if (typeof props[key] !== 'function') {
          assert(key.startsWith('$'), 'key ' + key + ' should not be in empty twin');
        }
      });
    };

    var assertObjectsAreEqual = function(left, right) {
      var compare = function(left, right) {
        _.every(_.keys(right), function(key) {
          if (typeof right[key] !== 'function' && !key.startsWith('$')) {
            assert.equal(left[key],right[key],'key ' + key + ' not matched between service and device'); 
          }
        });
      };
      compare(left, right);
      compare(right, left);
    };

    it ('round-trips reported properties', function(done) {
      assertObjectIsEmpty(serviceTwin.properties.reported);
      assertObjectIsEmpty(deviceTwin.properties.reported);

      deviceTwin.properties.reported.update(newProps, function(err) {
        if (err) throw err;
        serviceTwin.get(function(err) {
          if (err) throw err;
          assertObjectsAreEqual(newProps, serviceTwin.properties.reported);
          done();
        });
      });
    });

    it ('round trips and merges reported properties', function(done) {
      deviceTwin.properties.reported.update(newProps, function(err) {
        if (err) throw err;
        deviceTwin.properties.reported.update(moreNewProps, function(err) {
          if (err) throw err;
          serviceTwin.get(function(err) {
            if (err) throw err;
            assertObjectsAreEqual(mergeResult, serviceTwin.properties.reported);
            done();
          });
        });
      });
    });

    it ('round-trips desired properties', function(done) {
      assertObjectIsEmpty(deviceTwin.properties.desired);

      deviceTwin.on('properties.desired', function() {
        assertObjectsAreEqual(newProps, deviceTwin.properties.desired);
        done();
     });

      serviceTwin.update( { properties : { desired : newProps } }, function(err) {
        if (err) throw err;
      });
    });

    it ('round-trips and merges desired properties', function(done) {
      serviceTwin.update( { properties : { desired : newProps } }, function(err) {
        if (err) throw err;

        deviceTwin.on('properties.desired', function() {
          assertObjectsAreEqual(mergeResult, deviceTwin.properties.desired);
          done();
        });
        
        serviceTwin.update( { properties : { desired : moreNewProps } }, function(err) {
          if (err) throw err;
        });
      });
    });

    it ('round-trips and merges desired properties using etag *', function(done) {
      serviceTwin.update( { properties : { desired : newProps } }, function(err) {
        if (err) throw err;

        assert.isDefined(serviceTwin.etag);
        assert.notEqual(serviceTwin.etag, "*");
        serviceTwin.etag = "*";

        deviceTwin.on('properties.desired', function() {
          assertObjectsAreEqual(mergeResult, deviceTwin.properties.desired);
          done();
        });
        
        serviceTwin.update( { properties : { desired : moreNewProps } }, function(err) {
          if (err) throw err;
        });
      });
    });

    it ('can get and set tags', function(done) {
      assertObjectIsEmpty(serviceTwin.tags);

      serviceTwin.update( { tags : newProps }, function(err) {
        if (err) throw err;

        assertObjectsAreEqual(newProps, serviceTwin.tags);
        done();
      });
    });

    it ('can merge tags', function(done) {
      assertObjectIsEmpty(serviceTwin.tags);

      serviceTwin.update( { tags : newProps }, function(err) {
        if (err) throw err;
        serviceTwin.update( { tags: moreNewProps }, function(err) {
          if (err) throw err;
          assertObjectsAreEqual(mergeResult, serviceTwin.tags);
          done();
        });
      });
    });

    it('can merge tags using etag *', function(done) {
      assertObjectIsEmpty(serviceTwin.tags);

      serviceTwin.update( { tags : newProps }, function(err) {
        if (err) throw err;

        assert.isDefined(serviceTwin.etag);
        assert.notEqual(serviceTwin.etag, "*");
        serviceTwin.etag = "*";

        serviceTwin.update( { tags: moreNewProps }, function(err) {
          if (err) throw err;
          assertObjectsAreEqual(mergeResult, serviceTwin.tags);
          done();
        });
      });
    });

  });
};

module.exports = runTests;