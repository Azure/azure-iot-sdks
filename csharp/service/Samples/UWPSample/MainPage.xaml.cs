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
using Microsoft.Azure.Devices;

namespace UWPSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        const string connectionString = "...";

        public MainPage()
        {
            this.InitializeComponent();
            //SendMessage();
            AddDevice();
        }

        void SendMessage()
        {
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString, TransportType.Amqp);
            var str = "Hello, Cloud, from a UWP app that uses Microsoft.Azure.Devices";
            var message = new Message(System.Text.Encoding.ASCII.GetBytes(str));
            serviceClient.SendAsync("GarageDoorSensor", message);
        }

        void AddDevice()
        {
            RegistryManager manager = RegistryManager.CreateFromConnectionString(connectionString);
            manager.AddDeviceAsync(new Device("new_device"));
        }
    }
}
