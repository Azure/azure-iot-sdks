using System;
using Android.App;
using Android.Widget;
using Android.OS;
using Microsoft.Azure.Devices.Client;

namespace DeviceClientSampleAndroid
{
    using System.Text;
    using System.Threading.Tasks;
    using Message = Microsoft.Azure.Devices.Client.Message;

    [Activity(Label = "Device client sample", MainLauncher = true, Icon = "@drawable/icon")]
    public class MainActivity : Activity
    {
        int count = 1;
        private const string DeviceConnectionString = "<replace>";
        DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(DeviceConnectionString, TransportType.Http1);

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);

            // Set our view from the "main" layout resource
            SetContentView(Resource.Layout.Main);

            // Get our button from the layout resource,
            // and attach an event to it
            Button button1 = FindViewById<Button>(Resource.Id.MyButton);

            button1.Click += delegate
            {
                string dataBuffer = string.Format("Message from Android: {0}_{1}", count, Guid.NewGuid().ToString());
                var eventMessage = new Message(Encoding.UTF8.GetBytes(dataBuffer));
                deviceClient.SendEventAsync(eventMessage);
                button1.Text = string.Format("{0} messages sent to IotHub", count++);
            };

            Button button2 = FindViewById<Button>(Resource.Id.MyButton2);

            button2.Click += delegate
            {
                var asyncTask = Task.Run(() => ReceiveMessage().Result);
                var receivedMessage = asyncTask.Result;
                var receivedMessagesTextBox = FindViewById<EditText>(Resource.Id.receivedMessagesTextBox);
                if (receivedMessage != null)
                {
                    receivedMessagesTextBox.Text += Encoding.Default.GetString(receivedMessage.GetBytes()) + "\r\n";
                }
                Console.WriteLine("Message received, now completing it...");
                Task.Run(async () => { await deviceClient.ReceiveAsync(); });
                Console.WriteLine("Message completed.");
            };
        }

        private async Task<Message> ReceiveMessage()
        {
            return await this.deviceClient.ReceiveAsync();
        }

        private async Task CompleteMessage(Message message)
        {
            await this.deviceClient.CompleteAsync(message);
        }
    }
}