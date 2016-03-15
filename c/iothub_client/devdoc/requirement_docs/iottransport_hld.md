# IotHubTransport HLD

THe high level IotHubTransport is created to manage the need for having multiple IoTHub Device clients connect via the same transport connection.  

## User Experience

Ideally, when a user wants to make multiple IoTHubClient devices use the same connection, the user might create them like this:

```c
TRANSPORT_HANDLE  myTransportHandle = IoTHubTransport_HL_Create(HTTPProtocol, "myHubName", "example.com");
IOTHUB_CLIENT_HANDLE myDevice1 = IoTHubClient_CreateWithTransport(myTransportHandle, &config1);
IOTHUB_CLIENT_HANDLE myDevice2 = IoTHubClient_CreateWithTransport(myTransportHandle, &config2);
``` 

Each device has a unique handle, but sending and receiving data is happening across a single connection, as defined by the protocol.   

The user would then use the device client handle as normal, sending and receiving messages. The objective is not to change any existing interface, if possible.
 
When the user would like to shut down the service, it would be done like this:

```c
IoTHubClient_Destroy(myDevice1);
IoTHubClient_Destroy(myDevice2);
IoTHubTransport_HL_Destroy(myTransportHandle);
```

## Sharing the transport

Since this transport is going to be used across multiple threads and multiple devices, the transport needs to provide the lock needed to guarantee thread safety.  

Since there is only one connection, we want to limit the number of threads created.  Therefore, the high level transport will create and maintain the worker thread. 

This transport will contain:
- the lower layer transport
- the lock handle
- the worker thread

## Changes to IoTHubClient

When created through IoTHubClient_CreateWithTransport, the IoTHubClient will not create a lock, rather it will use the lock provided by the high level transport.

When IoTHubClient_SendEventAsync or IoTHubClient_SetMessageCallback is called, IoTHubClient will attempt to create the worker thread.  If the IoTHubClient was created via IoTHubClient_CreateWithTransport, it should attempt to start and stop the worker thread via IoTHubTransport_HL_StartWorkerThread and IoTHubTransport_HL_EndWorkerThread.
