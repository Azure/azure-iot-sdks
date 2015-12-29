Introduction
============

Azure IoTHub UWP SDK facilitates the service endpoint to send and
receive cloud messages to the device end points. This SDK can be used
for developing universal windows platform (UWP) in c\# for windows 10 OS
, developed by [**e-con
systems**](http://www.e-consystems.com/iot-gateway.asp) and contributed
to Azure IoT. This library uses
[**AMQPNetlite**](https://github.com/Azure/amqpnetlite) to communicate
with the Azure IoT hub. A sample application is provided to know the
usage of the APIs.

Features
========

-   Send the data to the Device client from cloud with optional
    receiving of feedback

-   Receive the data from the Client

-   Get the number of partition allocated for the IoT Hub

API Description 
================

All the APIs are within the class IoTHub under the namespace
UWPIoTHubSDK.

Constructor
-----------

### Signature:

IoTHub(string Namespace, string EventHubCompatibleNameSpace, string
DeviceName, String KeyName , string KeyValue)

### Parameters:

Namespace – Represents Namespace of the IoThub. This is same as the
string that preceded the path “azure-devices.net”

EventHubCompatibleNamespace – Represents a string representing the event
hub compatible name space as mentioned in your IoTHub Settings.

DeviceName – Represents name of the device to which you have to send the
data.

KeyName – Represents name of the access key which represents specific
permissions.

KeyValue – Represents value of the above mentioned key.

ReceiveMessageFromDevice
------------------------

### Signature:

void ReceiveMessagesFromDevice(string PartitionId, DataTime startTime,
ReceiveData CallBack)

### Description:

This function starts receiving messages from the device end points based
on the mentioned partition Id. The messages received are sent via the
specified delegate of type ReceiveData. This function once started exits
only when CleanUp function is called. So it is advisable to put this
function in a thread. Also wait for this thread to close after calling
CleanUp.

### Parameters:

PartitionId - Represents Partition Id to monitor. When a message is
received in this partition the call back is called along with the
message and the DeviceId of the sender.

startTime: This parameter specifies the time from which we are
interested in receiving the messages.

CallBack – This delegate will be called whenever a message is received.

### Return type:

None.

GetPartitions
-------------

### Signature:

String\[\] GetPartition()

### Description:

This function returns the partitions available as an array of string
with each individual string representing the partition id.

### Parameters:

None

### Return type:

Returns an array of string containing the partitions ids.

SendCloudToDeviceMessage
------------------------

### Signature:

Void SendCloudToDeviceMessage(String data, ReceiveFeedBackData callback
= null)

### Description:

This method sends the message to the device end point.

### Parameters:

Data – Represents string containing the data to send to the device end
point mentioned during initialisation in the constructor.

Callback – This is optional parameter which gets called when the feed
back data for the message sent is received. If the callback is not
called within 60 seconds then message is not received by the
destination.

### Return type:

None.

CleanUp
-------

### Signature:

Void CleanUp()

### Description:

This method stops the makes sure that the receive methods call are
terminated.

### Parameters:

None.

### Return type:

None.
