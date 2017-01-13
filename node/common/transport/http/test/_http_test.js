'use strict';

var Http = require('../lib/http.js');
var assert = require('chai').assert;

describe('Http', function() {
  describe('#parseErrorBody', function(){
    /*Tests_SRS_NODE_DEVICE_HTTP_16_001: [If the error body can be parsed the `parseErrorBody` function shall return an object with the following properties:
    - `name`: the name of the error that the server is returning
    - `message`: the human-readable error message]*/
    it('returns a object with the error code and message if the error is properly formatted', function(){
      var body = '{\"Message\":\"ErrorCode:FakeErrorCode;Fake Error Message\"}';
      var result = Http.parseErrorBody(body);
      assert.equal(result.code, 'FakeErrorCode');
      assert.equal(result.message, 'Fake Error Message');
    });

    /*Tests_SRS_NODE_DEVICE_HTTP_16_002: [If the error body cannot be parsed the `parseErrorBody` function shall return `null`.]*/
    [
      { errorBody: '{ Incomplete JSON', issueDescription: 'has incomplete JSON' },
      { errorBody: '{\"Message\":\"ErrorCode:NoErrorMessageCode"}', issueDescription: 'has no error message' },
      { errorBody: '{\"Message\":\"NoErrorCode;Fake Error Message\"}', issueDescription: 'has no error code' }
    ].forEach(function(testParam) {
      it('returns null if the error ' + testParam.issueDescription, function(){
        var result = Http.parseErrorBody(testParam.errorBody);
        assert.isNull(result);
      });
    });
  });

  describe('#toMessage', function() {
    /*Tests_SRS_NODE_HTTP_13_001: [ If the HTTP response has a header with the prefix iothub-app- then a new property with the header name and value as the key and value shall be added to the message. ]*/
    it('adds HTTP headers with prefix iothub-app- as message properties', function() {
      var mockResponse = {
        statusCode: 200,
        headers: {
          'iothub-app-k1': 'v1',
          'iothub-app-k2': 'v2',
          'iothub-app-k3': 'v3'
        }
      };
      var mockBody = 'boo';
      var httpTransport = new Http();
      var message = httpTransport.toMessage(mockResponse, mockBody);

      assert.isOk(message);
      assert.isOk(message.properties);
      var count = message.properties.count();
      assert.strictEqual(count, 3);

      for(var i = 1; i <= count; ++i) {
          var key = 'iothub-app-k' + i.toString();
          var val = 'v' + i.toString();
          var item = message.properties.getItem(i - 1);
          assert.isOk(item);
          assert.isOk(item.key);
          assert.isOk(item.value);
          assert.strictEqual(item.key, key);
          assert.strictEqual(item.value, val);
        }
    });
  });
});