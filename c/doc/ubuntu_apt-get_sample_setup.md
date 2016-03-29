# Using apt-get to create a project

This document describes how to create a program that uses the AzureIoT apt-get package

### Update and install binaries on machine 
1. Add the AzureIoT repository to the machine
    ```
    sudo add-apt-repository ppa:aziotsdklinux/ppa-azureiot  
    sudo apt-get update  
    sudo apt-get upgrade  
    ```

2. Install The azure-iot-sdk development package
    ```
    sudo apt-get install -y azure-iot-sdk-c-dev  
    ```

### Creating the application

1. Create a CMakeFiles.txt file for the application
```
cmake_minimum_required(VERSION 2.8.11)

set(AZUREIOT_INC_FOLDER ".." "/usr/include/azureiot")

include_directories(${AZUREIOT_INC_FOLDER})

set(sample_application_c_files
    ./sample.c
    ./main.c
)

set(sample_application_h_files
    ./sample.h
)

add_executable(sample_app ${sample_application_c_files} ${sample_application_h_files})

target_link_libraries(sample_app
    iothub_client
    iothub_client_mqtt_transport
    aziotsharedutil
    umqtt
    pthread
    curl
    ssl
    crypto
)
```

3. Create a directory that will store the make files that will be created with CMAKE and run cmake.
```
mkdir cmake
cd cmake
cmake ../[Directory to CMake File]
make
```
