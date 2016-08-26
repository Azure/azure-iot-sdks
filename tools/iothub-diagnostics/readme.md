# Diagnosing issues with your IoT Hub

This tool is provided to help diagnose issues with a device connecting to Azure IoT Hubs.

## Setup

Before running the tool, you will need to modify the settings in the [config](config/index.js). Specifically you should provide:

- hostName: This is  the hostname of the IOT Hub that you want to use for testing purposes.
- sharedAccessKeyName: The name of the shared access key that should be used to access the hub. This key should have registry write, service connect and device connect permissions.
- sharedAccessKey: The key (primary or secondary) for accessing the iot hub.


## Execution

To run the tool, from an admin command prompt execute `npm start`.
    
The tool will run, and will provide high level information about success and failure to the command prompt. Specifically it will:

1. It will attempt a DNS lookup, Ping and Traceroute of www.microsoft.com
  * If this fails then you should troubleshoot the devices connectivity to the internet.
  * If this is successful, then you have basic internet connectivity.
2. Attempt to use the Azure IOT SDK to communicate with the service.
  * If this fails, then first you should verify that the credentials you are using are correct.
  * If the credentials are correct, then likely this is an issue related to TLS.
  * If this is successful (but the original tooling is failing) then that would indicate a client side software bug.
    
