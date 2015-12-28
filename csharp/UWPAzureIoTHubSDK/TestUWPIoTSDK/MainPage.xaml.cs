using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using UWPIoTHubSDK;
using System.Diagnostics;
using System.Threading.Tasks;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace TestUWPIoTHubSDK
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    /// 
    public sealed partial class MainPage : Page
    {
        Task taskReceive;
        UWPIoTHubSDK.IoTHub iotHub;
        // this delegate will be called on receiving the message from device
        public static void DelegateMethod(byte[] data,string devid) 
        {
           Debug.WriteLine(System.Text.Encoding.UTF8.GetString(data, 0,data.Length));
           Debug.WriteLine(devid);
        }
        public static void DelegateFBMethod(string FeedBack)
        {
           
            Debug.WriteLine("FeedBack:"+ FeedBack);
        }
        public MainPage()
        {
            this.InitializeComponent();
            // Namespace refers to the string before the .azure-devices.net url
            // Eventhubcompatible name space is the name space provided in the azure IoT Hub portal
            // DeviceName refers to the name of the device in the IoTHub
            // KeyName and Key Value refers to the Access key name and Access key in the Azure portal
            iotHub = new UWPIoTHubSDK.IoTHub("remotecamerahub", "iothub-ns-remotecame-3364-ba3a28ad2c.servicebus.windows.net", "RasberryPi", "iothubowner", "xhuG9TcaieP9s+bNN51KTKy3H73Pw/0Sd8XOyrVJet8=");
            // This handler is passed to the Receive function 
            UWPIoTHubSDK.IoTHub.ReceivedData handler = DelegateMethod;
            UWPIoTHubSDK.IoTHub.ReceivedFeedBackData handlerFB = DelegateFBMethod;
            // This function is used to send data to the device. In case you need receiverfeedback you can specify call back or ignore it in case you dont need one 
            iotHub.SendCloudToDeviceMessage("This is Test Message", handlerFB);
            // Receive message is blocking and once called will be active till we call clean up. So we have to implement it as separate parallel task
            taskReceive = new Task(() =>
            {
                string[] temp = iotHub.GetPartitions();
                foreach( string s in temp)
                    Debug.WriteLine(s);
                iotHub.ReceiveMessagesFromDevice("13", DateTime.UtcNow, handler);
            });
            
            taskReceive.Start();
        }

        private void Close_App(object sender, RoutedEventArgs e) // Things to handle on application close 
        {
            // Closes the ReceiveFeedback and Receive Threads
            iotHub.CleanUp();

            // We have to wait till all the thread is closed
            taskReceive.Wait();
            Application.Current.Exit();
        }
    }
}
