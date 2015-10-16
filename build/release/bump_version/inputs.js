module.exports = [
    ///////////////////////////////////////////////////
    // Device Explorer how-to doc
    ///////////////////////////////////////////////////
    {
        "taskType": "regexReplaceTask",
        "filePath": "tools/DeviceExplorer/doc/how_to_use_device_explorer.md",
        "search": "(https\\:\\/\\/github.com\\/Azure\\/azure-iot-sdks\\/releases\\/download\\/).*(\\/SetupDeviceExplorer.msi)",
        "replaceString": function(versions) {
            return '$1' + versions['github-release'] + '$2';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "tools/DeviceExplorer/DeviceExplorer/Properties/AssemblyInfo.cs",
        "search": "(AssemblyInformationalVersion\(\").*(\"\)\])",
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
        "filePath": "c/iothub_client/inc/version.h",
        "search": "(IOTHUB\\_SDK\\_VERSION)([ ]+)(\".*\")",
        "replaceString": function(versions) {
            return '$1' + '$2' + '"' + versions.c.device + '"';
        }
    },
    ///////////////////////////////////////////////////
    // C# Device SDK
    ///////////////////////////////////////////////////
    {
        "taskType": "regexReplaceTask",
        "filePath": "csharp/Microsoft.Azure.Devices.Client/Properties/AssemblyInfo.cs",
        "search": "(AssemblyInformationalVersion\(\").*(\"\)\])",
        "replaceString": function(versions) {
            return '$1' + versions.csharp.device + '$2';
        }
    },
    {
        "taskType": "regexReplaceTask",
        "filePath": "csharp/Microsoft.Azure.Devices.Client.WinRT/Properties/AssemblyInfo.cs",
        "search": "(AssemblyInformationalVersion\(\").*(\"\)\])",
        "replaceString": function(versions) {
            return '$1' + versions.csharp.device + '$2';
        }
    },
    ///////////////////////////////////////////////////
    // Java Device SDK POM files
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
    ///////////////////////////////////////////////////
    // Java Service SDK POM files
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
    ///////////////////////////////////////////////////
    // Node SDK package.json files
    ///////////////////////////////////////////////////
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/build/package.json",
        "search": "version",
        "replaceString": "node.build"
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/common/package.json",
        "search": "version",
        "replaceString": "node.common"
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/common/package.json",
        "search": "devDependencies.azure-iot-build",
        "replaceString": function(versions) {
            return '^' + versions.node.common;
        }
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/device/package.json",
        "search": "version",
        "replaceString": "node.device"
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/device/package.json",
        "search": "dependencies.azure-iot-common",
        "replaceString": function(versions) {
            return '^' + versions.node.device;
        }
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/device/package.json",
        "search": "devDependencies.azure-iot-build",
        "replaceString": function(versions) {
            return '^' + versions.node.device;
        }
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/device/samples/package.json",
        "search": "dependencies.azure-iot-device",
        "replaceString": function(versions) {
            return '^' + versions.node.device;
        }
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/service/package.json",
        "search": "version",
        "replaceString": "node.service"
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/service/package.json",
        "search": "dependencies.azure-iot-common",
        "replaceString": function(versions) {
            return '^' + versions.node.service;
        }
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "node/service/package.json",
        "search": "devDependencies.azure-iot-build",
        "replaceString": function(versions) {
            return '^' + versions.node.service;
        }
    },
    {
        "taskType": "jsonReplaceTask",
        "filePath": "tools/iothub-explorer/package.json",
        "search": "version",
        "replaceString": "iothub-explorer"
    }
];
