# Cross Compiling the Azure IoT Hub SDK
## Background

The SDK for the Azure IoT Hub was written to use the C99 standard in order to retain a high level of compatibility with hardware platforms that have not yet seen an official release of the SDK. This should ease the burden of porting the SDK code to those platforms.

One of the challenges that might be encountered though is that the platform in question is not capable of building the code. Many such platforms may require that the executables are compiled on a host with a different chipset architecture from its own. This process of building executables on a host to target a disparate platform is known as cross compiling.

## Purpose of this document

This document presents an example of how to cross compile the Azure IoT Hub SDK using the make system, [cmake](https://cmake.org), that is employed by the project. In this example it will demonstrate the process of cross compiling the SDK on a Debian 64-bit system targeting a Raspberry Pi. It demonstrates how one must set up the file system and the cmake options to achieve this which should assist developers attempting to cross compile for other targets.

## Procedure

### Version Information

The host machine is running Debian GNU/Linux 8 (jessie) amd64

The target machine is running Raspbian GNU/Linux 8 (jessie)

__Note:__ This example was built and tested on an __amd64__ build of Debian on the host and will use the __64-bit version__ of the Raspbian toolchain. You will need to select a different target toolchain if your host is not running a 64-bit Linux operating system.

Though it may be possible to use a host machine running a variant of Windows this would likely be very complex to set up and thus is not addressed in this document.

### Setting up the Build Environment

Open a terminal prompt on your host machine in the manner you prefer.

We need to acquire the SDK source code. This is available in a GitHub repository at https://github.com/Azure/azure-iot-sdks.git. We clone this too our host machine as follows:
```
cd ~
mkdir Source
cd Source
git clone --recursive https://github.com/Azure/azure-iot-sdks.git
```
Further information regarding this step, which also includes important other set up requirements, can be found at <https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md>. This step is only included in this document to establish the directory structure used for the rest of the example.

You might consider building the SDK for your local platform at this point simply to ensure you have all the required components. At the very least, you must ensure that the SDK's prerequisite libraries are installed on your Raspberry Pi. You can achieve this by running the script _setup.sh_ found in _azure-iot-sdks/c/build\_all/linux_.

In order to cross compile for a different target the first requirement is to set up an environment containing the required toolchain, system libraries and system headers that may be required to build the code. In the instance of the Raspberry Pi this is simplified by the existence of a GitHub project that has much of that work already done for us. Change to your home directory, create a new directory for the Raspberry Pi Tools and clone the project at https://github.com/raspberrypi/tools into that directory. For example:
```
cd ~
mkdir RPiTools
cd RPiTools
git clone https://github.com/raspberrypi/tools.git
```
Unfortunately, this does not give us all the files that are required to build the project. At this point you will need to copy some files from a running Raspberry Pi to your host machine. Make sure you can access your Raspberry Pi via SSH (you can enable this using [raspi-config](https://www.raspberrypi.org/documentation/configuration/raspi-config.md) and select “9. Advanced Options”. Further instructions are beyond the scope of this document). On your host’s terminal enter
```
cd ~/RPiTools/tools/arm-bcm2708/\
gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf
rsync -rl --safe-links pi@<your Pi identifier>:/{lib,usr} .
```
In the above command replace &lt;*your Pi identifier*&gt; with the IP address of your Raspberry Pi. If you no longer have a user identity on your Raspberry Pi called pi, then you will need to substitute an existing user identity.

This command will copy many more files to your host than you actually need but, for brevity, we copy them all. If you wish, you could optimize the sync action to exclude files that are not required.

### Setting up cmake to cross compile

In order to tell cmake that it is performing a cross compile we need to provide it with a toolchain file. To save us some typing and to make our whole procedure less dependent upon the current user we are going to export a value. Whilst in the same directory as above enter the following command
```
export RPI_ROOT=$(pwd)
```
You can use *export -p* to verify RPI\_ROOT has been added to the environment.

Now we need to switch to the SDK directory tree. Enter this command
```
cd ~/Source/azure-iot-sdks/c/build_all/linux
```
Using the text editor of your choice, create a new file in this directory and call it toolchain-rpi.cmake. Into this file place the following lines

```cmake
INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Linux)     # this one is important
SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much

# this is the location of the amd64 toolchain targeting the Raspberry Pi
SET(CMAKE_C_COMPILER $ENV{RPI_ROOT}/../bin/arm-linux-gnueabihf-gcc)

# this is the file system root of the target
SET(CMAKE_FIND_ROOT_PATH $ENV{RPI_ROOT})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```
and save the toolchain file. Your cross compilation environment is now complete.

### Building the SDK

The final step in the process is to run the actual build. For this you will need to be in the Linux build directory as shown above. Enter the following commands
```
cd ~/Source/azure-iot-sdks/c/build_all/linux
./build.sh --toolchain-file toolchain-rpi.cmake --skip-unittests -cl --sysroot=$RPI_ROOT
```
This will tell cmake to build the SDK using the toolchain file toolchain-rpi.cmake and skip running all tests which is important since the executables will (probably) not run successfully on the host anyway. Finally, and absolutely critical is the use of the *--sysroot* option. Without this the compiler will fail to find required headers and libraries.

### Specifying Additional Build Flags

If you need to provide additional build flags for your cross compile to function you can add further _-cl_ parameters to the build script's command line. For example, adding _-cl -v_ will turn on the verbose output from the compile and link process or to pass options only to the linker for example to pass -v to the linker you can use _-cl Wl,-v_.

See this page for a summary of available gcc flags: https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html.

### Notes
These instructions have been tested on both the Raspberry Pi 2 and 3.

### Known Issues and Circumventions

If you encounter the error _error adding symbols: DSO missing from command line_ try adding a reference to libdl with  _-cl -ldl_ added to your build script command line.

## Summary

This document has demonstrated how to cross compile the Azure IoT SDK on a 64-bit Debian host targeting the Raspbian operating system.

## References

<https://github.com/Azure/azure-iot-sdks>

<https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md>

<https://github.com/raspberrypi/tools>

<https://cmake.org/Wiki/CMake_Cross_Compiling>

https://www.raspberrypi.org

<http://stackoverflow.com/questions/19162072/installing-raspberry-pi-cross-compiler> (See answer)
