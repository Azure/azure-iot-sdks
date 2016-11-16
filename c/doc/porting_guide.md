# How to Port the IoTHub C SDK to Other Platforms

The purpose of this document is to provide guidance on how to port the C IoTHub SDK to platforms not supported out of the box.

# Table of contents
- [Overview](#Overview)
- [PAL Porting](#PAL-porting)

<a name="Overview"/>
## Overview
</a>

The C IoT client library is written in C for the purpose of portability to most platforms (the SDK code is C99).
The IoTHub C SDK relies on the Platform Abstraction Layer implemented by the azure-c-shared-utility library.

<a name="PAL-porting"/>
## PAL porting
</a>

In order to port the C IoTHub SDK, one needs to port the PAL following [this](https://www.github.com/Azure/azure-c-shared-utility/blob/master/doc/porting_guide.md) document. 
