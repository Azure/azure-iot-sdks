#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

if [[ '' != $1 ]]
then
  if [[ -d $1 ]]
  then
    mkdir -p $1/iotdm-edison-sample/files
    cp iotdm-edison-sample.bb  $1/iotdm-edison-sample/
    cp iotdm_edison_sample.service  $1/iotdm-edison-sample/files/
  else
    echo "Path '$1' does NOT exist. Please get Edison sources and/or adjust <recipes-support_dir> as necessary"
  fi
else
  echo USAGE: './do_populate <recipes-support_dir>'
fi
