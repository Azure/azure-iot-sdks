# How to build and run Unit Tests

## Build and produce the appx package:

    msbuild UWPTests.sln /p:Publish=true /p:Platform=x86

## Install certs

This is one-time only step. If you don't do this, you will get this error:

    Error: Installation of package 'C:\projects\git_repos\azure-iot-sdks\csharp\device\tests\UWPTests\AppPackages\UWPTests_1.0.0.0_x86_Debug_Test\UWPTests_1.0.0.0_x86_Debug.
    appx' failed with Error: (0x800B0109) A certificate chain processed, but terminated in a root certificate which is not trusted by the trust provider.

Run this from the elevated command prompt:

    Certutil -addstore root AppPackages\UWPTests_1.0.0.0_x86_Debug_Test\UWPTests_1.0.0.0_x86_Debug.cer

## Run Tests:

    VSTest.Console.exe AppPackages\UWPTests_1.0.0.0_x86_Debug_Test\UWPTests_1.0.0.0_x86_Debug.appx /InIsolation 