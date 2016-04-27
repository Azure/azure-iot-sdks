#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

RECIPE_LOCATION=~/src/edison/edison-src/meta-intel-edison/meta-intel-edison-distro/recipes-support

if [ -d "$RECIPE_LOCATION" ]
then
  mkdir -p $RECIPE_LOCATION/iotdm-edison-sample/files
  cp iotdm-edison-sample.bb  $RECIPE_LOCATION/iotdm-edison-sample/
  cp iotdm_edison_sample.service  $RECIPE_LOCATION/iotdm-edison-sample/files/
else
  echo Path $RECIPE_LOCATION doesn''t exist.  Please get Edison sources and/or adjust RECIPE_LOCATION in do_populate.sh as necessary
fi

