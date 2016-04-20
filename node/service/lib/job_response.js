// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

/**
 * @class           module:azure-iothub.JobResponse
 * @classdesc       Creates a representation of a JobResponse for use in the
 *                  identity {@link module:azure-iothub.JobClient} APIs.
 * @param {String}  jsonData        An optional JSON representation of the
 *                                  jobResponse, which will be mapped to properties
 *                                  in the object. If no argument is provided,
 *                                  Device roperties will be assigned default
 *                                  values.
 * @prop {String}   jobId           jobId
 * @prop {String}   startTimeUtc    startTimeUtc
 * @prop {String}   endTimeUtc      endTimeUtc
 * @prop {String}   failureReason   failureReason
 * @prop {String}   type            type
 * @prop {String}   status          status
 * @prop {String}   progress        progress
 */
module.exports = function JobResponse(jsonData) {
  var body;
  if (jsonData) {
    if (typeof jsonData === 'string') {
      body = JSON.parse(jsonData);
    } else {
      body = jsonData;
    }

    if (!body.jobId) {
      throw new ReferenceError('Argument \'jobId\' is ' + body.jobId);
    }
  } else {
    body = {
      jobId: null,
      startTimeUtc: null,
      endTimeUtc: null,
      failureReason: '',
      type: null,
      status: null,
      progress: 0
    };
  }
  Object.defineProperties(this, {
    'jobId': {
      enumerable: true,
      get: function () {
        return body.jobId;
      },
      set: function (value) {
        body.jobId = value;
      }
    },
    'startTimeUtc': {
      enumerable: true,
      get: function () {
        return body.startTimeUtc;
      }
    },
    'endTimeUtc': {
      enumerable: true,
      get: function () {
        return body.endTimeUtc;
      }
    },
    'failureReason': {
      enumerable: true,
      get: function () {
        return body.failureReason;
      },
      set: function (value) {
        body.failureReason = value;
      }
    },
    'type': {
      enumerable: true,
      get: function () {
        return body.type;
      },
    },
    'status': {
      enumerable: true,
      get: function () {
        return body.status;
      }
    },
    'progress': {
      enumerable: true,
      get: function () {
        return body.progress;
      }
    }
  });  
};  

