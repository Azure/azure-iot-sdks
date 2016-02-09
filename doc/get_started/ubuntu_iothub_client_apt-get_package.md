# Using Debian apt-get

## Requirements

These instructions assume that your system contains the following:

- Using Ubuntu version 14.04, 15.04, and 15.10

1. Add the azure ppa to your repository and update the machine repository:
    ```
    sudo add-apt-repository ppa:aziotsdklinux/ppa-azure-iot-sdk-c
    sudo apt-get update
    sudo apt-get dist-upgrade
    ```
2. Install the iothub_client development package for the apt-get package
    ```
    sudo apt-get install azure-iot-sdk-c-dev
    ```

###This installs the azure-iot-sdk and it's dependencies to the following locations:
  
/usr/lib  
    * libiothub_client_amqp_transport.a  
    * libiothub_client_http_transport.a  
    * libiothub_client_mqtt_transport.a  
    * libserializer.a  
    * libiothub_client.a    
  
/usr/include/azureiot  
    * [all azure-iot-sdk] header file  

###Dependencies for iot-sdk

/usr/lib
    * libumqtt.a
    * libamqp.a
    * libaziotsharedutil.a

###To remove the package
    ```
    sudo apt-get --purge remove azure-iot-sdk-c
    ```
