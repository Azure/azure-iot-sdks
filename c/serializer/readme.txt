Azure IoTHub Schema Client
==============================

The IoTHub Schema client provides extra functionality compared to the IoTHub client:
- Defining a schema for the data that lives on the device (strong typing)
- Uploading the device data
- Process action/messages hooked up to C functions

For more details about the architecture of this Starter Kit Client, please check the following documents:
serializer\doc\c_agent_design.vsdx

Here you will find a sample of an implementation for some platforms:
serializer\samples ==> Currently we have a sample with 2 different protocols (AMQP and HTTP) and each sample is implementing for 
         some platforms. For example, the SimpleSampleHTTP is implemented for Linux, mbed and Windows. 
The folder structure for the samples is:
serializer\samples\SimpleSample_<Protocol>\<Platform>

Each folder will have its own Readme file with guideline on how to build and test each sample. 