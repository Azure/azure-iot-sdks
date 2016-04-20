// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var assert = require('chai').assert;
var JobResponse = require('../lib/job_response.js');

var JobRespJson = JSON.stringify({ jobId:'jobId-value',type:'firmwareUpdate',status:'enqueued',progress:0 });
var invalidJobResp = JSON.stringify({ jobId: '' });
var initializeJobResp = {
    jobId: 'job-id-test',
    startTimeUtc: '2015-08-20T18:08:49.9738417',
    endTimeUtc: '2015-08-20T18:08:49.9738417',
    failureReason: '',
    type: 'firmwareUpdate',
    status: 'enqueued',
    progress: 35
};
var fullJobResponse = JSON.stringify(initializeJobResp);
var testJobId = 'jobId-value';
var newJobId = 'newJobId';
var tempValue = 'testValue';
var newFailureReason = 'testFailureReason';

function throwsRedefineError(jobResponse, fieldName, descriptor) {
  assert.throws(function () {
    Object.defineProperty(jobResponse, fieldName, descriptor);
  }, TypeError, 'Cannot redefine property: '+fieldName);
}

describe('JobResponse', function () {
  describe('#enumerable', function() {
    it('is enumerable', function () {
      var jobResp = new JobResponse(JobRespJson);

      var allProps = [];
      for (var prop in jobResp) {
        if (jobResp.hasOwnProperty(prop)) {
          allProps.push(prop.toString());
        }
      }
      assert.include(allProps, 'jobId');
      assert.include(allProps, 'startTimeUtc');
      assert.include(allProps, 'endTimeUtc');
      assert.include(allProps, 'failureReason');
      assert.include(allProps, 'type');
      assert.include(allProps, 'status');
      assert.include(allProps, 'progress');
    });
  });

  describe('#constructor', function () {
    it('creates a JobResponse with the given id', function () {
      var jobResp = new JobResponse(JobRespJson);
      assert.equal(jobResp.jobId, testJobId);
    });

    it('throws if \'jobId\' is falsy', function () {
      function throwsReferenceError(ctorArg) {
        assert.throws(function () {
          return new JobResponse(ctorArg);
        }, ReferenceError);
      }
      throwsReferenceError(invalidJobResp);
    });
    
    it('is created correctly if the argument is a JSON string', function() {
      var jobResp = new JobResponse(fullJobResponse);
      
      assert.equal(jobResp.jobId, initializeJobResp.jobId);
      assert.equal(jobResp.startTimeUtc, initializeJobResp.startTimeUtc);
      assert.equal(jobResp.endTimeUtc, initializeJobResp.endTimeUtc);
      assert.equal(jobResp.failureReason, initializeJobResp.failureReason);
      assert.equal(jobResp.type, initializeJobResp.type);
      assert.equal(jobResp.status, initializeJobResp.status);
      assert.equal(jobResp.progress, initializeJobResp.progress);
    });

    it('is created correctly if the argument is an object', function() {
      var jobResp = new JobResponse(initializeJobResp);
      
      assert.equal(jobResp.jobId, initializeJobResp.jobId);
      assert.equal(jobResp.startTimeUtc, initializeJobResp.startTimeUtc);
      assert.equal(jobResp.endTimeUtc, initializeJobResp.endTimeUtc);
      assert.equal(jobResp.failureReason, initializeJobResp.failureReason);
      assert.equal(jobResp.type, initializeJobResp.type);
      assert.equal(jobResp.status, initializeJobResp.status);
      assert.equal(jobResp.progress, initializeJobResp.progress);
    });
  });

  describe('#jobId', function () { 
    it('cannot be configured or deleted', function () {
      var jobResp = new JobResponse(fullJobResponse);
      
      throwsRedefineError(jobResp, 'jobId', { configurable: true });
      throwsRedefineError(jobResp, 'jobId', { enumerable: false });
      throwsRedefineError(jobResp, 'jobId', { set: function () { } });
      throwsRedefineError(jobResp, 'jobId', { get: function () { return tempValue; } });
      throwsRedefineError(jobResp, 'jobId', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(jobResp, 'jobId', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete jobResp.jobId;
      }, TypeError, 'Cannot delete property \'jobId\' of #<JobResponse>');
      
      jobResp.jobId = newJobId;
      assert.equal(jobResp.jobId, newJobId);
    });
  });

  describe('#startTimeUtc', function () { 
    it('cannot be configured or deleted', function () {
      var jobResp = new JobResponse(fullJobResponse);
      
      throwsRedefineError(jobResp, 'startTimeUtc', { configurable: true });
      throwsRedefineError(jobResp, 'startTimeUtc', { enumerable: false });
      throwsRedefineError(jobResp, 'startTimeUtc', { set: function () { } });
      throwsRedefineError(jobResp, 'startTimeUtc', { get: function () { return tempValue; } });
      throwsRedefineError(jobResp, 'startTimeUtc', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(jobResp, 'startTimeUtc', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete jobResp.startTimeUtc;
      }, TypeError, 'Cannot delete property \'startTimeUtc\' of #<JobResponse>');
    });
  });
  
  describe('#endTimeUtc', function () { 
    it('cannot be configured or deleted', function () {
      var jobResp = new JobResponse(fullJobResponse);
      
      throwsRedefineError(jobResp, 'endTimeUtc', { configurable: true });
      throwsRedefineError(jobResp, 'endTimeUtc', { enumerable: false });
      throwsRedefineError(jobResp, 'endTimeUtc', { set: function () { } });
      throwsRedefineError(jobResp, 'endTimeUtc', { get: function () { return tempValue; } });
      throwsRedefineError(jobResp, 'endTimeUtc', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(jobResp, 'endTimeUtc', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete jobResp.endTimeUtc;
      }, TypeError, 'Cannot delete property \'endTimeUtc\' of #<JobResponse>');
    });
  });

  describe('#failureReason', function () { 
    it('cannot be configured or deleted', function () {
      var jobResp = new JobResponse(fullJobResponse);
      
      throwsRedefineError(jobResp, 'failureReason', { configurable: true });
      throwsRedefineError(jobResp, 'failureReason', { enumerable: false });
      throwsRedefineError(jobResp, 'failureReason', { set: function () { } });
      throwsRedefineError(jobResp, 'failureReason', { get: function () { return tempValue; } });
      throwsRedefineError(jobResp, 'failureReason', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(jobResp, 'failureReason', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete jobResp.failureReason;
      }, TypeError, 'Cannot delete property \'failureReason\' of #<JobResponse>');
      
      jobResp.failureReason = newFailureReason;
      assert.equal(jobResp.failureReason, newFailureReason);      
    });
  });

  describe('#type', function () { 
    it('cannot be configured or deleted', function () {
      var jobResp = new JobResponse(fullJobResponse);
      
      throwsRedefineError(jobResp, 'type', { configurable: true });
      throwsRedefineError(jobResp, 'type', { enumerable: false });
      throwsRedefineError(jobResp, 'type', { set: function () { } });
      throwsRedefineError(jobResp, 'type', { get: function () { return tempValue; } });
      throwsRedefineError(jobResp, 'type', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(jobResp, 'type', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete jobResp.type;
      }, TypeError, 'Cannot delete property \'type\' of #<JobResponse>');
    });
  });

  describe('#status', function () { 
    it('cannot be configured or deleted', function () {
      var jobResp = new JobResponse(fullJobResponse);
      
      throwsRedefineError(jobResp, 'status', { configurable: true });
      throwsRedefineError(jobResp, 'status', { enumerable: false });
      throwsRedefineError(jobResp, 'status', { set: function () { } });
      throwsRedefineError(jobResp, 'status', { get: function () { return tempValue; } });
      throwsRedefineError(jobResp, 'status', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(jobResp, 'status', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete jobResp.status;
      }, TypeError, 'Cannot delete property \'status\' of #<JobResponse>');
    });
  });

  describe('#progress', function () { 
    it('cannot be configured or deleted', function () {
      var jobResp = new JobResponse(fullJobResponse);
      
      throwsRedefineError(jobResp, 'progress', { configurable: true });
      throwsRedefineError(jobResp, 'progress', { enumerable: false });
      throwsRedefineError(jobResp, 'progress', { set: function () { } });
      throwsRedefineError(jobResp, 'progress', { get: function () { return tempValue; } });
      throwsRedefineError(jobResp, 'progress', { value: 'world' });

      assert.doesNotThrow(function () {
        Object.defineProperty(jobResp, 'progress', { enumerable: true }); // redefine to same value is ok
      });

      assert.throws(function () {
        delete jobResp.progress;
      }, TypeError, 'Cannot delete property \'progress\' of #<JobResponse>');
    });
  });
    
});
