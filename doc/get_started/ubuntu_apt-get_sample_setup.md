# Use apt-get to create a C device client project on Ubuntu

This document describes how to create a program that uses the *azure-iot-sdk-c-dev* package on Ubuntu versions 15.04  and 15.10. The package contains the binaries you need to build an IoT Hub client application using C. The document describes how to install the package using **apt-get** and then build an application using **CMake**.

Before you start, make sure you have **CMake**, **g++**, and **gcc** installed on your development machine:

```
sudo apt-get install cmake gcc g++
```

## Update and install the azure-iot-sdk-c-dev package on your machine

Complete the following steps to install the AzureIoT binaries on your development machine:

1. Add the AzureIoT repository to the machine:

    ```
    sudo add-apt-repository ppa:aziotsdklinux/ppa-azureiot  
    sudo apt-get update  
    ```

2. Install The azure-iot-sdk-c-dev package

    ```
    sudo apt-get install -y azure-iot-sdk-c-dev  
    ```

## Create the application using CMake

The following steps outline how you can use CMake to build an IoT Hub client application after you have installed the azure-iot-sdk-c-dev package on your development machine.

1. Create a CMakeLists.txt file for the application. The example shown here assumes you are using the *serializer* module in your code, that your application uses the AMQP protocol, and that your source code is contained in the files **sample.c**, **sample.h**, and **main.c**. You can add source files by changing the contents of the **sample_application_c_files** and **sample_application_h_files** sets. If you are not using the serializer module, you can omit it from **target_link_libraries**. If you are using a different protocol (such as MQTT) you need to change the content of **target_link_libraries** accordingly.

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
        serializer
        iothub_client
        iothub_client_amqp_transport
        aziotsharedutil
        uamqp
        pthread
        curl
        ssl
        crypto
    )
    ```

3. Create a directory that will store the make files that CMake creates and then run the **cmake** and **make** commands as follows:

    ```
    mkdir cmake
    cd cmake
    cmake ../[Directory that contains your CMakeLists.txt File]
    make
    ```

## Notes

The Azure IoT client libraries and their dependencies install to the following locations.
  
Libraries install in /usr/lib:

* libiothub_client_amqp_transport.a  
* libiothub_client_http_transport.a  
* libiothub_client_mqtt_transport.a  
* libserializer.a  
* libiothub_client.a    
  
All azure-iot-sdk header files install in /usr/include/azureiotsdk.

Dependencies install in /usr/lib:

* libumqtt.a
* libamqp.a
* libaziotsharedutil.a

To remove the azure-iot-sdk-c-dev package, run the following command:

```
sudo apt-get --purge remove azure-iot-sdk-c-dev
```