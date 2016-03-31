# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

# check Python version is supported for Azure IoT SDK

import os, platform, sys

class PlatformError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

try:
    if sys.version_info < (2, 7):
        raise PlatformError("Require Python 2.7 or greater")
    if sys.version_info >= (3, 0):
        raise PlatformError("Require Python 2.7, version 3.x is not supported")
    plat = platform.architecture()
    if (plat[0] == "64bit"):
        print ("SET build-platform=x64")
    else:
        if (plat[0] != "32bit"):
            raise PlatformError("Require Windows CPython 2.7 32bit or 64bit version")
    print ("SET PYTHON_PATH=%s" % os.path.dirname(sys.executable))
    sys.exit(0)
except PlatformError as e:
    sys.exit(e.value)
