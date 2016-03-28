# Using apt-get to create a project

This document describes how 

### Update and install binaries on machine 
1. Add repository to the mahcine
    ```
    sudo add-apt-repository ppa:aziotsdklinux/ppa-azureiot  
    sudo apt-get update  
    sudo apt-get upgrade  
    ```

2. Install The azure-iot-sdk development package
    ```
    sudo apt-get install -y azure-iot-sdk-c-dev  
    ```

### Creating application

1. Create a cmake file that adds the include directory of /usr/include/azureiot

2. Add the target libraries to the specified transport (amqp/mqtt/http)
    - (See cmake iothub_client sample directory under the linux directory)

3. Create the build libraries and make the project.
