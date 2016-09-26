from setuptools import setup, Distribution
import sys

class PlatformError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class BinaryDistribution(Distribution):
    def is_pure(self):
        return False

_long_description=()

try:
    if sys.version_info < (2, 7):
        raise PlatformError("Require Python 2.7 or greater")
    if sys.version_info >= (3, 0) and sys.version_info < (3, 4):
        raise PlatformError("Require Python 3.4 or greater")
except PlatformError as e:
    sys.exit(e.value)

try:
    from iothub_client import iothub_client
    _version = iothub_client.__version__
except Exception as e:
    sys.exit(e)

setup(
    name='iothub_client',
    version=_version+'.dev4', # todo: remov devX once moving from testpypy to pypy
    description='IoT Hub Client Library',
    license='Apache Software License',
    url='https://github.com/Azure/azure-iot-sdks/tree/master/python/device',
    author='aziotclb',
    author_email='aziotclb@microsoft.com',
    long_description='IoT Hub Client Library for Python 2.7 and 3.4 - iothub_client.pyd',
    packages=['iothub_client'],
    classifiers=[
        'Environment :: Win32 (MS Windows)',
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5'],
    package_data={
        'iothub_client': ['__init__.py','iothub_client.pyd'],
    },
    distclass=BinaryDistribution
) 
