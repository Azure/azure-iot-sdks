# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT device SDK for Java*.

- [Java JDK SE](#installjava)
- [Maven 3](#installmaven)
- [Azure IoT device SDK for Java](#installiot)
	- [Build from source](#installiotsource)
	- [Include using Maven](#installiotmaven)
 -  [Build for Android device](#installiotandroid)
- [Application Samples](#samplecode)

<a name="installjava"/>
## Install Java JDK SE
To use the SDK and run the samples you will need **Java SE 8**.

### Windows
For downloads and installation instructions go here: http://www.oracle.com/technetwork/java/javase/downloads/index.html

#### Set up environment variables
- Please make sure that the `PATH` environment variable includes the full path to the `jdk1.8.x\bin` directory. (Example: C:\\Program Files\\Java\\jdk1.8.0_60\\bin)
- Please make sure that the `JAVA_HOME` environment variable includes the full path to the `jdk1.8.x` directory. (Example: JAVA_HOME=C:\\Program Files\\Java\\jdk1.8.0_60)

You can test whether your `PATH` variable is set correctly by restarting your console and running `java -version`.

### Linux

**Note:** If you are running a version of Ubuntu below 14.10, you must run the command shown below to add the repository that contains the **openjdk-8-jdk** package to Ubuntu's list of software sources before you attempt to use the **apt-get** command to install openjdk-8-jdk:

```
sudo add-apt-repository ppa:openjdk-r/ppa
```


On Linux, the Java OpenJDK 8 can be installed as follows:

```
sudo apt-get update
sudo apt-get install openjdk-8-jdk
```

#### Set up environment variables
- Please make sure that the `PATH` environment variable includes the full path to the bin folder containing java.

	```
	which java
	echo $PATH
	```
	Ensure that the bin directory shown by the ```which java``` command matches one of the directories shown in your $PATH variable.
	If it does not:
	```
	export PATH=/path/to/java/bin:$PATH
	```

- Please make sure that the `JAVA_HOME` environment variable includes the full path to the jdk.

	```
	update-alternatives --config java
	```
	Take note of the jdk location. ```update-alternatives``` will show something similar to ***/usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java***. The jdk directory would then be ***/usr/lib/jvm/java-8-openjdk-amd64/***.

	```
	export JAVA_HOME=/path/to/jdk
	```


<a name="installmaven"/>
## Install Maven
Using **_Maven 3_** is the recommended way to install **Azure IoT device SDK for Java**.

### Windows
For downloads and installation instructions go here: https://maven.apache.org/download.cgi

#### Set up environment variables
- Please make sure that the `PATH` environment variable includes the full path to the `apache-maven-3.x.x\bin` directory. (Example: F:\\Setups\\apache-maven-3.3.3\\bin). The `apache-maven-3.x.x` directory is where Maven 3 is installed.

You can verify that the environment variables necessary to run **_Maven 3_** have been set correctly by restarting your console and running `mvn --version`.

### Linux
On Linux, Maven 3 can be installed as follows:

```
sudo apt-get update
sudo apt-get install maven
```

#### Set up environment variables

Please verify the following:

Ensure the `PATH` environment variable contains the full path to the bin folder containing **_Maven 3_**.

	```
	which mvn
	echo $PATH
	```
Ensure that the bin directory shown by the ```which mvn``` command matches one of the directories shown in your $PATH variable.
	If it does not:
	```
	export PATH=/path/to/mvn/bin:$PATH
	```

You can verify that the environment variables necessary to run **_Maven 3_** have been set correctly by running `mvn --version`.

<a name="installiot"/>
## Install Azure IoT device SDK for Java

- There are two ways to get the .jar library for the Azure IoT device SDK. You may either download the source code and build on your machine, or include the project as a dependency in your project if your project is a Maven project. Both methods are described below.

<a name="installiotsource">
### Build Azure IoT device SDK for Java from Sources
- Get a copy of the **Azure IoT device SDK** from GitHub. You should fetch a copy of the source from the **master** branch of the GitHub repository: <https://github.com/Azure/azure-iot-sdks>
- When you have obtained a copy of the source, you can build the SDK for Java.

Open a command prompt and use the following commands for the steps above:

```
	git clone https://github.com/Azure/azure-iot-sdks.git
	cd azure-iot-sdks/java/device
	mvn install
```

The compiled JAR file with all dependencies bundled in can then be found at:

```
{IoT device SDK root}/java/device/iothub-java-client/target/iothub-java-client-{version}-with-deps.jar
```

When you're ready to use the Java SDK in your own project, include this JAR file in your project to use the IoT device SDK.

<a name="installiotmaven">
### Get Azure IoT device SDK for Java from Maven (as a dependency)
_This is the recommended method of including the Azure IoT Device SDK in your project, however this method will only work if your project is a Maven project_

- Navigate to http://search.maven.org, search for **com.microsoft.azure.iothub-java-client** and take not of the latest version number (or the version number of whichever version of the sdk you desire to use).

In your main pom.xml file, add the Azure IoT Device SDK as a dependency using your desired version as follows:
```
<dependency>
    <groupId>com.microsoft.azure.iothub-java-client</groupId>
    <artifactId>iothub-java-client</artifactId>
    <version>1.0.0</version>
	<!--This is the current version number as of the writing of this document. Yours may be different.-->
</dependency>
```
<a name="installiotandroid"> 
## Building for Android Device
- Download and install [Android Studio][android-studio]
- Load and build **sample** located at java\device\samples\android-sample
- Sample has dependence on remote library `iothub-java-device-client`. It is currently set to use `iothub-java-device-client:1.0.7`. If you want to point it to the later version, please update `java\device\samples\android-sample\app\build.gradle` file to point to the version you want to use. For list of available versions search [Maven Repository] [maven-repository]


<a name="samplecode">
## Sample applications

This repository contains various .Net sample applications that illustrate how to use the Microsoft Azure IoT device SDK for Java. For more information, see the [readme][readme].

To learn how to run a simple *Getting started* Java application that sends messages to an IoT hub, see [Getting started - running a Java sample][lnk-getstarted].

For testing the **Android sample** on Android device follow [certify IoT device running Android] [certify-iot-device-android] documentation

[readme]: ../../java/device/readme.md
[lnk-getstarted]: java-run-sample.md
[android-studio]: https://developer.android.com/studio/index.html
[certify-iot-device-android]:https://github.com/Azure/azure-iot-sdks/blob/master/doc/iotcertification/iot_certification_android_java/iot_certification_android_java.md
[maven-repository]:http://search.maven.org/#search%7Cga%7C1%7Ciothub-java-device-client


