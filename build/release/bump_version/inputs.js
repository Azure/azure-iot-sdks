module.exports = [
    ///////////////////////////////////////////////////
    // Device Explorer
    ///////////////////////////////////////////////////
    {
        "taskType": "regexReplaceTask",
        "filePath": "tools/DeviceExplorer/DeviceExplorer/Properties/AssemblyInfo.cs",
        "search": "(AssemblyInformationalVersion\\(\").*(\"\\)\\])",
        "replaceString": function(versions) {
            return '$1' + versions['device-explorer'] + '$2';
        }
    },
    ///////////////////////////////////////////////////
    // C SDK
    ///////////////////////////////////////////////////
    {
        "taskType": "regexReplaceTask",
        "filePath": "build/docs/Doxyfile",
        "search": "(PROJECT\\_NUMBER)([ ]+\\= )(.*)",
        "replaceString": function(versions) {
            return '$1' + '$2' + versions.c.device;
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "build/release/release_mbed_project.cmd",
        "search": "(hg\\_commit\\_message\\=)(\".*\")",
        "replaceString": function(versions) {
            return '$1' + '"' + versions.c.device + '"';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "c/iothub_client/inc/iothub_client_version.h",
        "search": "(IOTHUB\\_SDK\\_VERSION)([ ]+)(\".*\")",
        "replaceString": function(versions) {
            return '$1' + '$2' + '"' + versions.c.device + '"';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "c/build_all/arduino_cc/base-libraries/AzureIoTHub/library.properties",
        "search": "(version\\=)(.*)",
        "replaceString": function(versions) {
            return '$1' + versions.c_arduino.device;
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "c/build_all/arduino_cc/base-libraries/AzureIoTHub/src/AzureIoTHub.h",
        "search": "(AzureIoTHubVersion\\=)([ ]+)(\".*\")",
        "replaceString": function(versions) {
            return '$1' + '$2' + '"' + versions.c_arduino.device + '"';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "c/iothub_client/tests/version_unittests/version_unittests.cpp",
        "search": "(\\\".*\\\")([ \t]*\\,[ \t]*IOTHUB\\_SDK\\_VERSION)",
        "replaceString": function(versions) {
            return '"' + versions.c.device + '"$2';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "c/iothub_client/tests/iothubclient_ll_u2b_unittests/iothub_client_ll_u2b_unittests.c",
        "search": "(TEST\\_IOTHUB\\_SDK\\_VERSION)([ ]+)(\".*\")",
        "replaceString": function(versions) {
            return '$1' + '$2' + '"' + versions.c.device + '"';
        }
    },
    ///////////////////////////////////////////////////
    // C SDK nugets
    ///////////////////////////////////////////////////
    {
        "taskType": "multiTask",
        "filePath": "c/build_all/packaging/windows/Microsoft.Azure.IoTHub.AmqpTransport.nuspec",
        "search": [
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.IoTHub.IoTHubClient']/@version",
                "replaceString": "c_nuget.device"
            },
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.C.SharedUtility']/@version",
                "replaceString": "c_shared_utility_nuget"
            },
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.Uamqp']/@version",
                "replaceString": "uamqp_c_nuget"
            },
            {
                "taskType": "xmlReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:version",
                "replaceString":"c_nuget.device"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "c/build_all/packaging/windows/Microsoft.Azure.IoTHub.HttpTransport.nuspec",
        "search": [
            {
                "taskType": "xmlReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:version",
                "replaceString": "c_nuget.device"
            },
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.IoTHub.IoTHubClient']/@version",
                "replaceString": "c_nuget.device"
            },
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.C.SharedUtility']/@version",
                "replaceString": "c_shared_utility_nuget"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "c/build_all/packaging/windows/Microsoft.Azure.IoTHub.IoTHubClient.nuspec",
        "search": [
            {
                "taskType": "xmlReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:version",
                "replaceString": "c_nuget.device"
            },
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.C.SharedUtility']/@version",
                "replaceString": "c_shared_utility_nuget"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "c/build_all/packaging/windows/Microsoft.Azure.IoTHub.MqttTransport.nuspec",
        "search": [
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.IoTHub.IoTHubClient']/@version",
                "replaceString": "c_nuget.device"
            },
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.C.SharedUtility']/@version",
                "replaceString": "c_shared_utility_nuget"
            },
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.umqtt']/@version",
                "replaceString": "umqtt_c_nuget"
            },
            {
                "taskType": "xmlReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:version",
                "replaceString":"c_nuget.device"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "c/build_all/packaging/windows/Microsoft.Azure.IoTHub.Serializer.nuspec",
        "search": [
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.C.SharedUtility']/@version",
                "replaceString": "c_shared_utility_nuget"
            },
            {
                "taskType": "xmlReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:version",
                "replaceString":"c_nuget.device"
            }
        ]
    },
    ///////////////////////////////////////////////////
    // C uamqp nuget
    ///////////////////////////////////////////////////
    {
        "taskType": "multiTask",
        "filePath": "c/azure-uamqp-c/build_all/Packaging/windows/Microsoft.Azure.uamqp.nuspec",
        "search": [
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.C.SharedUtility']/@version",
                "replaceString": "c_shared_utility_nuget"
            },
            {
                "taskType": "xmlReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:version",
                "replaceString": "uamqp_c_nuget"
            }
        ]
    },
    ///////////////////////////////////////////////////
    // C shared utility nuget
    ///////////////////////////////////////////////////
    {
        "taskType": "multiTask",
        "filePath": "c/azure-c-shared-utility/build_all/Packaging/windows/Microsoft.Azure.C.SharedUtility.nuspec",
        "search": [
            {
                "taskType": "xmlReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:version",
                "replaceString": "c_shared_utility_nuget"
            }
        ]
    },
    ///////////////////////////////////////////////////
    // C umqtt nuget
    ///////////////////////////////////////////////////
    {
        "taskType": "multiTask",
        "filePath": "c/azure-umqtt-c/build_all/Packaging/windows/Microsoft.Azure.umqtt.nuspec",
        "search": [
            {
                "taskType": "xmlAttributeReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:dependencies/nu:dependency[@id = 'Microsoft.Azure.C.SharedUtility']/@version",
                "replaceString": "c_shared_utility_nuget"
            },
            {
                "taskType": "xmlReplaceTask",
                "nsmap": {
                    "nu": "http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd"
                    },
                "search": "//nu:package/nu:metadata/nu:version",
               "replaceString": "umqtt_c_nuget"
            }
        ]
    },
    ///////////////////////////////////////////////////
    // C# Device SDK
    ///////////////////////////////////////////////////
    {
        "taskType": "regexReplaceTask",
        "filePath": "csharp/device/Microsoft.Azure.Devices.Client/Properties/AssemblyInfo.cs",
        "search": "(AssemblyInformationalVersion\\(\").*(\"\\)\\])",
        "replaceString": function(versions) {
            return '$1' + versions.csharp.device + '$2';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "csharp/device/Microsoft.Azure.Devices.Client.PCL/Properties/AssemblyInfo.cs",
        "search": "(AssemblyInformationalVersion\\(\").*(\"\\)\\])",
        "replaceString": function(versions) {
            return '$1' + versions.csharp.devicepcl + '$2';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "csharp/device/Microsoft.Azure.Devices.Client.WinRT/Properties/AssemblyInfo.cs",
        "search": "(AssemblyInformationalVersion\\(\").*(\"\\)\\])",
        "replaceString": function(versions) {
            return '$1' + versions.csharp.device + '$2';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "csharp/device/Microsoft.Azure.Devices.Client/IotHubConnection.cs",
        "search": "(UWPAssemblyVersion[ \t]*=[ \t]*\").*(\")",
        "replaceString": function(versions) {
            return '$1' + versions.csharp.device + '$2';
        }
    },

    ///////////////////////////////////////////////////
    // C# Service SDK
    ///////////////////////////////////////////////////
    {
        "taskType": "regexReplaceTask",
        "filePath": "csharp/service/Microsoft.Azure.Devices/Properties/AssemblyInfo.cs",
        "search": "(AssemblyInformationalVersion\\(\").*(\"\\)\\])",
        "replaceString": function(versions) {
            return '$1' + versions.csharp.service + '$2';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "csharp/service/Microsoft.Azure.Devices.Uwp/Properties/AssemblyInfo.cs",
        "search": "(AssemblyInformationalVersion\\(\").*(\"\\)\\])",
        "replaceString": function(versions) {
            return '$1' + versions.csharp.service + '$2';
        }
    },

    {
        "taskType": "regexReplaceTask",
        "filePath": "csharp/service/Microsoft.Azure.Devices/Common/Utils.cs",
        "search": "(UWPAssemblyVersion[ \t]*=[ \t]*\").*(\")",
        "replaceString": function(versions) {
            return '$1' + versions.csharp.service + '$2';
        }
    },    

    ///////////////////////////////////////////////////
    // Java Device SDK files
    ///////////////////////////////////////////////////
    {
        "taskType": "xmlReplaceTask",
        "filePath": "java/device/pom.xml",
        "search": "//project/version",
        "replaceString": "java.device"
    },
    {
        "taskType": "xmlReplaceTask",
        "filePath": "java/device/iothub-java-client/pom.xml",
        "search": "//project/parent/version",
        "replaceString": "java.device"
    },
    {
        "taskType": "multiTask",
        "filePath": "java/device/samples/pom.xml",
        "search": [
            {
                "taskType": "xmlReplaceTask",
                "search": "//project/parent/version",
                "replaceString": "java.device"
            },
            {
                "taskType": "xmlReplaceTask",
                "search": "//project/dependencies/dependency[groupId = 'com.microsoft.azure.iothub-java-client']/version",
                "replaceString": "java.device"
            }
        ]
    },
    {
        "taskType": "xmlReplaceTask",
        "filePath": "java/device/samples/handle-messages/pom.xml",
        "search": "//project/parent/version",
        "replaceString": "java.device"
    },
    {
        "taskType": "xmlReplaceTask",
        "filePath": "java/device/samples/send-event/pom.xml",
        "search": "//project/parent/version",
        "replaceString": "java.device"
    },
    {
        "taskType": "xmlReplaceTask",
        "filePath": "java/device/samples/send-serialized-event/pom.xml",
        "search": "//project/parent/version",
        "replaceString": "java.device"
    },
    {
        "taskType": "xmlReplaceTask",
        "filePath": "java/device/samples/send-receive-sample/pom.xml",
        "search": "//project/parent/version",
        "replaceString": "java.device"
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "java/device/iothub-java-client/src/main/java/com/microsoft/azure/iothub/transport/TransportUtils.java",
        "search": "(clientVersion =)([ ]+)(\".*\")",
        "replaceString": function(versions) {
            return '$1' + '$2' + '"' + versions.java.device + '"';
        }
    },
    ///////////////////////////////////////////////////
    // Java Service SDK files
    ///////////////////////////////////////////////////
    {
        "taskType": "xmlReplaceTask",
        "filePath": "java/service/pom.xml",
        "search": "//project/version",
        "replaceString": "java.service"
    },
    {
        "taskType": "xmlReplaceTask",
        "filePath": "java/service/iothub-service-sdk/pom.xml",
        "search": "//project/version",
        "replaceString": "java.service"
    },
    {
        "taskType": "multiTask",
        "filePath": "java/service/samples/pom.xml",
        "search": [
            {
                "taskType": "xmlReplaceTask",
                "search": "//project/version",
                "replaceString": "java.service"
            },
            {
                "taskType": "xmlReplaceTask",
                "search": "//project/dependencies/dependency[groupId = 'com.microsoft.azure.iothub.service.sdk']/version",
                "replaceString": "java.service"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "java/service/samples/device-manager-sample/pom.xml",
        "search": [
            {
                "taskType": "xmlReplaceTask",
                "search": "//project/version",
                "replaceString": "java.service"
            },
            {
                "taskType": "xmlReplaceTask",
                "search": "//project/dependencies/dependency[groupId = 'com.microsoft.azure.iothub.service.sdk']/version",
                "replaceString": "java.service"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "java/service/samples/service-client-sample/pom.xml",
        "search": [
            {
                "taskType": "xmlReplaceTask",
                "search": "//project/version",
                "replaceString": "java.service"
            },
            {
                "taskType": "xmlReplaceTask",
                "search": "//project/dependencies/dependency[groupId = 'com.microsoft.azure.iothub.service.sdk']/version",
                "replaceString": "java.service"
            }
        ]
    },
        {
        "taskType": "regexReplaceTask",
        "filePath": "java/service/iothub-service-sdk/src/main/java/com/microsoft/azure/iot/service/transport/TransportUtils.java",
        "search": "(serviceVersion =)([ ]+)(\".*\")",
        "replaceString": function(versions) {
            return '$1' + '$2' + '"' + versions.java.service + '"';
        }
    },
    ///////////////////////////////////////////////////
    // Node SDK package.json files
    ///////////////////////////////////////////////////
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/common/core/package.json",
        "search": "version",
        "replaceString": "node.common"
    },
    {
        "taskType": "multiTask",
        "filePath": "node/common/transport/amqp/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.amqp-base"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "node/common/transport/http/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.http-base"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "node/common/transport/mqtt/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.mqtt-base"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            }
        ]
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "node/device/core/readme.md",
        "search": "(http\\:\\/\\/azure.github.io\\/azure-iot-sdks\\/node\\/api_reference\\/azure-iot-device\\/).*(\\/index.html)",
        "replaceString": function(versions) {
            return '$1' + versions.node.device + '$2';
        }
    },
    {
        "taskType": "multiTask",
        "filePath": "node/device/core/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.device"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "node/device/samples/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device",
                "replaceString": "node.device"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device-amqp",
                "replaceString": "node.device-amqp"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device-http",
                "replaceString": "node.device-http"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device-mqtt",
                "replaceString": "node.device-mqtt"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "node/device/transport/amqp/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.device-amqp"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-amqp-base",
                "replaceString": "node.amqp-base"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device",
                "replaceString": "node.device"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "node/device/transport/amqp-ws/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.device-amqp-ws"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-amqp-base",
                "replaceString": "node.amqp-base"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device",
                "replaceString": "node.device"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device-amqp",
                "replaceString": "node.device-amqp"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "node/device/transport/http/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.device-http"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-http-base",
                "replaceString": "node.http-base"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device",
                "replaceString": "node.device"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "node/device/transport/mqtt/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.device-mqtt"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-mqtt-base",
                "replaceString": "node.mqtt-base"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device",
                "replaceString": "node.device"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "node/e2etests/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.device"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device",
                "replaceString": "node.device"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device-amqp",
                "replaceString": "node.device-amqp"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device-http",
                "replaceString": "node.device-http"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iothub",
                "replaceString": "node.service"
            }
        ]
    },
    {
        "taskType": "multiTask",
        "filePath": "node/service/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "node.service"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-amqp-base",
                "replaceString": "node.amqp-base"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-http-base",
                "replaceString": "node.http-base"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            }
        ]
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/service/samples/package.json",
        "search": "dependencies.azure-iothub",
        "replaceString": "node.service"
    },
    {
        "taskType": "multiTask",
        "filePath": "tools/iothub-explorer/package.json",
        "search": [
            {
                "taskType": "jsonReplaceTask",
                "search": "version",
                "replaceString": "iothub-explorer"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-common",
                "replaceString": "node.common"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iot-device",
                "replaceString": "node.device"
            },
            {
                "taskType": "jsonReplaceTask",
                "search": "dependencies.azure-iothub",
                "replaceString": "node.service"
            }
        ]
    }
];
