// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;

var endpoint = require('../lib/endpoint.js');

describe('endpoint', function () {
    describe('#devicePath', function () {
    it('matches /devices/<device-id>', function () {
      var path = endpoint.devicePath('mydevice');
      assert.equal('/devices/mydevice', path);
    });
  });

  describe('#eventPath', function () {
    it('matches /devices/<device-id>/messages/events', function () {
      var path = endpoint.eventPath('mydevice');
      assert.equal('/devices/mydevice/messages/events', path);
    });
  });
  
  describe('#messagePath', function () {
    it('matches /devices/<device-id>/messages/devicebound', function () {
      var path = endpoint.messagePath('mydevice');
      assert.equal('/devices/mydevice/messages/devicebound', path);
    });
  });
  
  describe('#feedbackPath', function () {
    it('matches /devices/<device-id>/messages/devicebound/<lockToken>', function () {
      var path = endpoint.feedbackPath('mydevice', '55E68746-0AD9-4DCF-9906-79CDAC14FFBA');
      assert.equal('/devices/mydevice/messages/devicebound/55E68746-0AD9-4DCF-9906-79CDAC14FFBA', path);
    });
  });
});
