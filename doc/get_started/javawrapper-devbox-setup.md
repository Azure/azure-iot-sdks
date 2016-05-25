# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT device SDK for Java*.

- [Java JDK SE](#installjava)
- [Maven 3](#installmaven)

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

