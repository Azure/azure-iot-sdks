# Cross Compiling the Azure IoT Hub SDK on Android
## Purpose of present document

Present document is directly evolved by the great tutorial available [here](SDK_cross_compile_example.md). It shows hot to cross-compile the Azure IoT Hub SDK a Ubuntu 64-bit system, targeting an Android device.

## Procedure

### Version Information

The host machine is running Ubuntu 16.04.1 amd64

The target machine is running Android on arm architecture (32 bit)

### Setting up the Build Environment

Get the SDK source code from GitHub repository at https://github.com/Azure/azure-iot-sdks.git:
```
cd ~
mkdir Source
cd Source
git clone --recursive https://github.com/Azure/azure-iot-sdks.git
```

### Build the cross compiler toolchain

The Android NDK package available at https://developer.android.com/ndk/downloads/index.html contains the toolchain needed to target Android devices. Download the Linux package, e.g. _android-ndk-rxx-linux-x86_64.zip_, and extract the archive content into your home folder. You will have a directory _android-ndk-rxx_ in your home folder.

Now you have to build the Android NDK toolchain. To allow access in next steps to the cross compiler for Android, enter your just created ndk root directory, and save:
```
cd ~/android-ndk-rxx
export ANDROID_NDK_ROOT=$(pwd)
```
You can use *export -p* to verify ANDROID\_NDK\_ROOT has been added to the environment. Next, add the ANDROID\_NDK\_ROOT content to PATH environment variable:
```
export PATH=${ANDROID_NDK_ROOT}:${PATH}
```
Following commands will make standalone toolchain binaries in the `/tmp/my-android-toolchain` location.
```
cd build/tools
./make_standalone_toolchain.py --arch arm --install-dir /tmp/my-android-toolchain
cd /tmp/my-android-toolchain
export MY_TOOLCHAIN=$(pwd)
export PATH=${MY_TOOLCHAIN}/bin:$PATH
export CC=arm-linux-androideabi-gcc
export CXX=arm-linux-androideabi-g++
```

### Cross-compiling libuuid

Unfortunately, the Azure sdk libraries need the presence of a library not included in the android toolchain support binaries. Download it from <https://sourceforge.net/projects/libuuid/>.

Untar the libuuid archive in your home directory, obtaining a new folder similar to `~/libuuid-x.x.x`. Then type following commands>

```
cd ~/libuuid-x.x.x
./configure CC=arm-linux-androideabi-gcc --prefix=/tmp/my-android-toolchain-deps --host=arm-linux-androideabi --with-sysroot=${MY_TOOLCHAIN}/sysroot
make
make install
```

### Cross-compiling libcurl

This is another dependency required by Azure sdk libraries. Download from <https://curl.haxx.se/download.html> and unpack it to `~/curl-x.x.x`. So type next commands:

```
cd ~/curl-x.x.x
./configure CC=arm-linux-androideabi-gcc --prefix=/tmp/my-android-toolchain-deps --host=arm-linux-androideabi --with-sysroot=${MY_TOOLCHAIN}/sysroot
make
make install
```

### Cross-compiling Openssl

Obtain `setenv-android-x.x.sh` from <http://openssl.com/fips/2.0/platforms/android/> and save it in your home directory. Open the script and 

Obtain OpenSSL sources from <http://www.openssl.org/source/>, unpacking it in a directory similar to `~/openssl-1.1.0b`.



### Setting up cmake to cross compile

Now we need to switch to the SDK directory tree. Enter this command
```
cd ~/Source/azure-iot-sdks/c/build_all/linux
```
Using the text editor of your choice, create a new file in this directory and call it toolchain-android.cmake. Into this file place the following lines

```cmake
INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Android)

# this is the location of the toolchain targeting the arm architecture
SET(CMAKE_C_COMPILER $ENV{MY_TOOLCHAIN}/bin/arm-linux-androideabi-gcc)

# this is the file system root of the target
SET(CMAKE_FIND_ROOT_PATH $ENV{MY_TOOLCHAIN})

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
./setup.sh
./build.sh --toolchain-file toolchain-android.cmake --skip-unittests -cl --sysroot=${MY_TOOLCHAIN}/sysroot -cl -I/tmp/my-android-toolchain-deps/include -cl -L/tmp/my-android-toolchain-deps -cl -llibuuid -cl -llibcurl
```
This will tell cmake to build the SDK using the toolchain file toolchain-android.cmake and skip running all tests which is important since the executables will (probably) not run successfully on the host anyway. Absolutely critical is the use of the *--sysroot* option. Without this the compiler will fail to find required headers and libraries. Note the *-cl* option, used to pass a single custom option to the compiler. With the *-I* option the cross-compiler will search additional libraries include files also in `/tmp/my-android-toolchain-deps/include`. With the *-L* option the cross-compiler is set to search libreries also in `/tmp/my-android-toolchain-deps`. With the *-llibuuid* option the cross-compiler will search the shared library `libuuid`, and in a similar way the shared library `libcurl`.

## References

<https://github.com/Azure/azure-iot-sdks>

<https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md>

<https://cmake.org/Wiki/CMake_Cross_Compiling>

<https://developer.android.com/ndk/guides/standalone_toolchain.html>

<https://kvurd.com/blog/compiling-a-cpp-library-for-android-with-android-studio/>

<http://www.fabriziodini.eu/posts/cross_compile_tutorial/>

<https://wiki.openssl.org/index.php/Android>

<http://openssl.com/fips/2.0/platforms/android/OpenSSL_FIPS_Library_and_Android_Guide.pdf>
