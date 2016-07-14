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

    /*SRS_NODE_DEVICE_HTTP_16_002: [If the error body cannot be parsed the `parseErrorBody` function shall return `null`.]*/
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
});