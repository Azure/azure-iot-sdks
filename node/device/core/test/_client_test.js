// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var sinon = require('sinon');
var stream = require('stream');

var Client = require('../lib/client.js');
var SimulatedHttp = require('./http_simulated.js');
var runTests = require('./_client_common_testrun.js');

describe('Client', function () {
  describe('#constructor', function () {
    /*Tests_SRS_NODE_DEVICE_CLIENT_05_001: [The Client constructor shall throw ReferenceError if the transport argument is falsy.]*/
    it('throws if transport arg is falsy', function () {
      [null, undefined, '', 0].forEach(function (transport) {
        assert.throws(function () {
          return new Client(transport);
        }, ReferenceError, 'transport is \'' + transport + '\'');
      });
    });
  });

  describe('#fromConnectionString', function () {
    var connectionString = 'HostName=host;DeviceId=id;SharedAccessKey=key';

    /*Tests_SRS_NODE_DEVICE_CLIENT_05_003: [The fromConnectionString method shall throw ReferenceError if the connStr argument is falsy.]*/
    it('throws if connStr arg is falsy', function () {
      [null, undefined, '', 0].forEach(function (value) {
        assert.throws(function () {
          return Client.fromConnectionString(value);
        }, ReferenceError, 'connStr is \'' + value + '\'');
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_05_006: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(new Transport(...)).]*/
    it('returns an instance of Client', function () {
      var DummyTransport = function () { };
      var client = Client.fromConnectionString(connectionString, DummyTransport);
      assert.instanceOf(client, Client);
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_027: [If a connection string argument is provided, the Client shall automatically generate and renew SAS tokens.] */
    it('automatically renews the SAS token before it expires', function (done) {
      this.clock = sinon.useFakeTimers();
      var clock = this.clock;
      var tick = 0;
      var firstTick = 1800000;
      var secondTick = 1200000;
      var sasUpdated = false;
      var DummyTransport = function () {
        this.updateSharedAccessSignature = function () {
          sasUpdated = true;
          assert.equal(tick, secondTick);
          clock.restore();
          done();
        };
      };

      var client = Client.fromConnectionString(connectionString, DummyTransport);
      assert.instanceOf(client, Client);
      tick = firstTick;
      this.clock.tick(tick); // 30 minutes. shouldn't have updated yet.
      assert.isFalse(sasUpdated);
      tick = secondTick;
      this.clock.tick(tick); // +20 => 50 minutes. should've updated so the callback will be called and the test will terminate.
    });
  });

  describe('#fromSharedAccessSignature', function () {
    /*Tests_SRS_NODE_DEVICE_CLIENT_16_029: [The fromSharedAccessSignature method shall throw a ReferenceError if the sharedAccessSignature argument is falsy.] */
    it('throws if sharedAccessSignature arg is falsy', function () {
      [null, undefined, '', 0].forEach(function (value) {
        assert.throws(function () {
          return Client.fromSharedAccessSignature(value);
        }, ReferenceError, 'sharedAccessSignature is \'' + value + '\'');
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_030: [The fromSharedAccessSignature method shall return a new instance of the Client object] */
    it('returns an instance of Client', function () {
      var DummyTransport = function () { };
      var sharedAccessSignature = '"SharedAccessSignature sr=hubName.azure-devices.net/devices/deviceId&sig=s1gn4tur3&se=1454204843"';
      var client = Client.fromSharedAccessSignature(sharedAccessSignature, DummyTransport);
      assert.instanceOf(client, Client);
    });

    it('create a correct config when sr is not URI-encoded', function () {
      var sharedAccessSignature = '"SharedAccessSignature sr=hubName.azure-devices.net/devices/deviceId&sig=s1gn4tur3&se=1454204843"';
      var DummyTransport = function (config) {
        assert.strictEqual(config.host, 'hubName.azure-devices.net');
        assert.strictEqual(config.deviceId, 'deviceId');
        assert.strictEqual(config.hubName, 'hubName');
        assert.strictEqual(config.sharedAccessSignature, sharedAccessSignature);
      };
      Client.fromSharedAccessSignature(sharedAccessSignature, DummyTransport);
    });

    it('create a correct config when sr is URI-encoded', function () {
      var sharedAccessSignature = '"SharedAccessSignature sr=hubName.azure-devices.net%2Fdevices%2FdeviceId&sig=s1gn4tur3&se=1454204843"';
      var DummyTransport = function (config) {
        assert.strictEqual(config.host, 'hubName.azure-devices.net');
        assert.strictEqual(config.deviceId, 'deviceId');
        assert.strictEqual(config.hubName, 'hubName');
        assert.strictEqual(config.sharedAccessSignature, sharedAccessSignature);
      };
      Client.fromSharedAccessSignature(sharedAccessSignature, DummyTransport);
    });
  });
  
  describe('uploadToBlob', function() {
    /*Tests_SRS_NODE_DEVICE_CLIENT_16_037: [The `uploadToBlob` method shall throw a `ReferenceError` if `blobName` is falsy.]*/
    [undefined, null, ''].forEach(function (blobName) {
      it('throws a ReferenceError if \'blobName\' is ' + blobName + '\'', function() {
        var client = new Client({}, null, {});
        assert.throws(function() {
          client.uploadToBlob(blobName, new stream.Readable(), 42, function() {});
        });
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_038: [The `uploadToBlob` method shall throw a `ReferenceError` if `stream` is falsy.]*/
    [undefined, null, ''].forEach(function (stream) {
      it('throws a ReferenceError if \'stream\' is ' + stream + '\'', function() {
        var client = new Client({}, null, {});
        assert.throws(function() {
          client.uploadToBlob('blobName', stream, 42, function() {});
        });
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_039: [The `uploadToBlob` method shall throw a `ReferenceError` if `streamLength` is falsy.]*/
    [undefined, null, '', 0].forEach(function (streamLength) {
      it('throws a ReferenceError if \'streamLength\' is ' + streamLength + '\'', function() {
        var client = new Client({}, null, {});
        assert.throws(function() {
          client.uploadToBlob('blobName', new stream.Readable(), streamLength, function() {});
        });
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_040: [The `uploadToBlob` method shall call the `done` callback with an `Error` object if the upload fails.]*/
    it('calls the done callback with an Error object if the upload fails', function(done) {
      var DummyBlobUploader = function () {
        this.uploadToBlob = function(blobName, stream, streamLength, callback) {
          callback(new Error('fake error'));
        };
      };

      var client = new Client({}, null, new DummyBlobUploader());
      client.uploadToBlob('blobName', new stream.Readable(), 42, function(err) {
        assert.instanceOf(err, Error);
        done();
      });
    });

    /*Tests_SRS_NODE_DEVICE_CLIENT_16_041: [The `uploadToBlob` method shall call the `done` callback no parameters if the upload succeeds.]*/
    it('calls the done callback with no parameters if the upload succeeded', function (done) {
      var DummyBlobUploader = function () {
        this.uploadToBlob = function(blobName, stream, streamLength, callback) {
          callback();
        };
      };

      var client = new Client({}, null, new DummyBlobUploader());
      client.uploadToBlob('blobName', new stream.Readable(), 42, done);
    });
  });
});

function makeConnectionString(host, device, key) {
  return 'HostName=' + host + ';DeviceId=' + device + ';SharedAccessKey=' + key;
}

var connectionString = makeConnectionString('host', 'device', 'key');
var badConnStrings = [
  makeConnectionString('bad', 'device', 'key'),
  makeConnectionString('host', 'bad', 'key'),
  makeConnectionString('host', 'device', 'bad')
];

describe('Over simulated HTTPS', function () {
  runTests(SimulatedHttp, connectionString, badConnStrings);
});
