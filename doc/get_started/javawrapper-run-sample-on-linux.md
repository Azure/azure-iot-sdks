
# Run sample on Windows

 * execute \javawrapper\build_all\windows\build.sh
 * Start a new instance of the Device Explorer, 
   select or create a new device, obtain and note the connection string for the device, and begin moitoring under the Data tab. 
 * Navigate to the folder containing the executable JAR file for the sample that you wish to run and run the samples as follows:

   The executable JAR file can be found at:

  ```
  {IoT device SDK root}/javawrapper/device/samples/send-receive-sample/target/sendReceive-{version}-jar-with-dependencies.jar

  ```
   Navigate to the directory with the sample. Run the sample using the following command:

  ```
  java -jar sendReceive-{version}-jar-with-dependencies.jar
  ```
