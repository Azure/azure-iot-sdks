# How to write an Azure IoT Hub client application from scratch with Java on Windows

This document describes how to create a client application that uses the *Microsoft Azure IoT device SDK for Java* to communicate with the Azure IoT Hub service. This walkthrough shows you how to create and build a project using the [Maven][apache-maven] tool. You can follow these instructions on either a Windows or Linux machine.

## Table of contents

- [Installation](#installation)
- [Create the project](#createproject)
- [Create the application](#createapp)
- [Change logging granularity](#logging)
- [API documentation](#apidoc)

<a name="installation"/>
## Installation

See [Prepare your development environment][devbox-setup] for information about prerequisites and setting up your development environment on Windows.

> Note: It's  important that you complete the steps in [Prepare your development environment][devbox-setup] before you start this walkthrough to ensure that all the prerequisites are installed and that the necessary JAR files are in your local Maven repository.

<a name="createproject"/>
## Create the project

On your command line, execute the following command to create a new, empty maven project in a suitable location on your development machine:

```
mvn archetype:generate -DgroupId=com.mycompany.app -DartifactId=iot-device -DarchetypeArtifactId=maven-archetype-quickstart -DinteractiveMode=false
```

>Note: This is a single, long command. If you are viewing this in GitHub, you must scroll to the right to view the complete command. Your browser may enable you to select the complete comand using a triple-click.

This creates a folder named **iot-device** that contains a project with the standard maven project structure. For more information, see [Maven in 5 Minutes][maven-five-minutes] on the Apache website.

Open the **pom.xml** file in the **iot-device** folder in a text editor.

Add the following new **dependency** section after the existing one to include the client libraries:

```
<dependency>
  <groupId>com.microsoft.azure.iothub-java-client</groupId>
  <artifactId>iothub-java-client</artifactId>
  <version>1.0.0-preview.4</version>
</dependency>
```

Add the following **build** section after the **dependencies** section to ensure that the final JAR file includes all the dependencies and manifest identifies the **main** class.

```
<build>
  <plugins>
    <plugin>
      <groupId>org.apache.maven.plugins</groupId>
      <artifactId>maven-shade-plugin</artifactId>
      <executions>
        <execution>
          <phase>package</phase>
          <goals>
            <goal>shade</goal>
          </goals>
        </execution>
      </executions>
      <configuration>
        <finalName>${artifactId}-${version}-with-deps</finalName>
      </configuration>
    </plugin>
    <plugin>
      <groupId>org.apache.maven.plugins</groupId>
      <artifactId>maven-jar-plugin</artifactId>
      <version>2.6</version>
      <configuration>
        <archive>
          <manifest>
            <addClasspath>true</addClasspath>
            <mainClass>com.mycompany.app.App</mainClass>
          </manifest>
        </archive>
      </configuration>
    </plugin>
  </plugins>
</build>
```

Save the **pom.xml** file.


<a name="createapp"/>
## Create the application

Open the **App.java** file in the iot-device/src/main/java/com/mycompany/app folder in a text editor.

Add the following **import** statements, which include the IoT device libraries, after the **package** statement:

```
import com.microsoft.azure.iothub.DeviceClient;
import com.microsoft.azure.iothub.IotHubClientProtocol;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.IotHubEventCallback;
import com.microsoft.azure.iothub.IotHubMessageResult;

import java.io.IOException;
import java.net.URISyntaxException;
import java.security.InvalidKeyException;
import java.util.Scanner;

import javax.naming.SizeLimitExceededException;
```

Add the following **EventCallback** class as a nested class inside the **App** class. The **execute** method in the **EventCallback** class is invoked when the device receives an acknowledgment from your IoT hub in response to a device-to-cloud message.

```
protected static class EventCallback
        implements IotHubEventCallback
{
    public void execute(IotHubStatusCode status, Object context)
    {
        Integer i = (Integer) context;
        System.out.println("IoT Hub responded to message " + i.toString()
                + " with status " + status.name());
    }
}
```


Add the following **MessageCallback** class as a nested class inside the **App** class. The **execute** method in the **MessageCallback** class enables you to send a feedback message to your IoT hub in response to a cloud-to-device message received by the device.

```
protected static class MessageCallback
        implements com.microsoft.azure.iothub.MessageCallback
{
    public IotHubMessageResult execute(Message msg,
            Object context)
    {
        System.out.println(
                "Received message with content: " + new String(msg.getBytes(), Message.DEFAULT_IOTHUB_MESSAGE_CHARSET));

        return IotHubMessageResult.COMPLETE;
    }
}
```

Replace the  existing **main** method with the following code. This code:

- Creates a **DeviceClient** instance.
- Initializes the message callback for cloud-to-device messages.
- Opens the **DeviceClient** to enable it to send device-to-cloud messages and receive cloud-to-device messages.
- Sends ten sample messages to your IoT hub.

> Note: Replace `<your device connection string>` with a valid device connection string from the [DeviceExplorer][device-explorer] tool.

```
public static void main( String[] args ) throws IOException, URISyntaxException
{
    String connString = "<your device connection string>";
    IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

    DeviceClient client = new DeviceClient(connString, protocol);

    MessageCallback messageCallback = new MessageCallback();
    client.setMessageCallback(messageCallback, null);

    client.open();

    for (int i = 0; i < 10; ++i)
    {
        String msgStr = "Event Message " + Integer.toString(i);
        try
        {
            Message msg = new Message(msgStr);
            msg.setProperty("messageCount", Integer.toString(i));
            System.out.println(msgStr);

            EventCallback eventCallback = new EventCallback();
            client.sendEventAsync(msg, eventCallback, i);
        }
        catch (Exception e)
        {
        }
    }

    System.out.println("Press any key to exit...");
    Scanner scanner = new Scanner(System.in);
    scanner.nextLine();

    client.close();
}
```

To compile the code and package it into a JAR file, execute the following command at a command prompt in the **iot-device** project folder:

```
mvn package
```

To run the application, execute the following command at a command prompt in the **iot-device** project folder:

```
java -jar target/iot-device-1.0-SNAPSHOT-with-deps.jar
```

You can use the [DeviceExplorer][device-explorer] tool to monitor the device-to-cloud messages that your IoT hub receives and to send cloud-to-device messages from your IoT hub.

<a name="logging"/>
## Change logging granularity
The logging granularity can be specified using a dedicated `config.properties` file.
Include the following line in your `config.properties` file:

```
.level = {LOGGING_LEVEL}
```

An explanation of the different logging levels can be found
[here](http://docs.oracle.com/javase/7/docs/api/java/util/logging/Level.html).

Then, set the JVM property `java.util.logging.config.file={Path to your config.properties file}`.

To log AMQP frames, set the environment variable PN_TRACE_FRM=1.

<a name="apidoc"/>
## API Documentation

The documentation can be found at:

```
http://azure.github.io/azure-iot-sdks/java/api_reference/index.html
```


[apache-maven]: https://maven.apache.org/index.html
[maven-five-minutes]: https://maven.apache.org/guides/getting-started/maven-in-five-minutes.html
[devbox-setup]: devbox_setup.md
[device-explorer]: ../../../tools/DeviceExplorer/devdoc/how_to_use_device_explorer.md
