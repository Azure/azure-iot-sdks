#!/bin/bash

set -e

sh ~/release-script/gen_vivid_pkg.sh
sh ~/release-script/gen_wily_pkg.sh
sh ~/release-script/gen_trusty_pkg.sh
