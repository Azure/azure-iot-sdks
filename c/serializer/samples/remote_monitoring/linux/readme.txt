How to have a running the remote monitoring sample on Ubuntu 14.04 LTS

1. Get a virtual machine in Azure. (New->Compute->Virtual Machine->Quick Create). Use Ubuntu Server 14.04 LTS, Size "A1".
2. Once the virtual machine has been created, SSH into it (putty is a nice ssh tool on windows). Notice that the only user name available at this time is "azureuser"
3. The following are packages needed: cmake, uuid-dev, libcurl4-openssl-dev, g++. They can all be installed with one command:

sudo apt-get install git cmake libcurl4-openssl-dev build-essential

4. Once these packages have been installed, clone the repository by using

git clone https://github.com/Azure/azure-iot-sdks.git

    It will prompt for a user name and a password. The password needs to be a token from github.com : Personal Settings -> Applications -> Personal access tokens. It cannot be the account password.

5. Switch to directory azure-iot-sdks

cd azure-iot-sdks

6. Once in that directory, checkout branch develop by

git checkout develop

7. go to linux build folder in ./build_all/linux

cd build_all/linux

8. switch to the folder of the remote monitoring machine source code:

cd ../../serializer/samples/remote_monitoring/

9. edit main.c providing your credentials (nano editor can be used)

10. go back to the build folder (cd -) and build all the software (lib, samples, unittests). This will also run unittests. Might take some minutes.

cd -
bash ./build.sh

11. at this moment, remote_monitoring executable can be run by

../../serializer/samples/remote_monitoring/linux/remote_monitoring
