# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT device SDK for Java*.

- [Java JDK SE (version 1.7 on Windows | 1.8+ on Linux)](#installjava)
- [Maven 3](#installmaven)
- [Qpid JMS](#installjsm)
- [Azure IoT device SDK for Java](#installiot)
- [Application Samples](#samplecode)

<a name="installjava"/>
## Install Java JDK SE
To run the SDK you will need **Java SE 1.7** if running on Windows or **Java SE 1.8+** if running on Linux.

### Windows
For downloads and installation instructions go here: http://www.oracle.com/technetwork/java/javase/downloads/jdk7-downloads-1880260.html

#### Set up environment variables
- Please make sure that the `PATH` environment variable includes the full path to the `jdk1.7.x\bin` directory. (Example: C:\\Program Files\\Java\\jdk1.7.0_79\\bin)
- Please make sure that the `JAVA_HOME` environment variable includes the full path to the `jdk1.7.x` directory. (Example: JAVA_HOME=C:\\Program Files\\Java\\jdk1.7.0_79)

You can test whether your `PATH` variable is set correctly by restarting your console and running `java -version`.

### Linux
On Linux, Java JDK 1.8 can be installed as follows:

```
sudo apt-get update
sudo apt-get install openjdk-8-jdk
```

**If you are running a version of Ubuntu below 14.10, you will need to run the below command to add the respository containing openjdk-8-jdk to Ubuntu's list of software sources before attempting to use apt-get to install openjdk-8-jdk:**

```
sudo add-apt-repository ppa:openjdk-r/ppa
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

- `PATH` environment variable contain the full path to the bin folder containing **_Maven 3_**.

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

<a name="installjms"/>
## Install Qpid JMS
```
git clone https://github.com/avranju/qpid-jms.git
cd qpid-jms
mvn install
cd ..
```

<a name="installiot"/>
## Install Azure IoT device SDK for Java

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

_In the future, the IoT device SDK for Java will be uploaded to the Maven central repository.
Then, it can be included as a dependency in your project by including the following block
in your pom.xml file:_

```
<dependency>
    <groupId>com.microsoft.azure.iothub-java-client</groupId>
    <artifactId>iothub-java-client</artifactId>
    <version>0.2.0</version>
</dependency>
```

<a name="samplecode">
## Sample applications

This repository contains various .Net sample applications that illustrate how to use the Microsoft Azure IoT device SDK for Java. For more information, see the [readme][readme].

To learn how to run a simple *Getting started* Java application that sends messages to an IoT hub, see [Getting started - running a Java sample][lnk-getstarted].

[readme]: ../readme.md
[lnk-getstarted]: run_sample_on_java.md
