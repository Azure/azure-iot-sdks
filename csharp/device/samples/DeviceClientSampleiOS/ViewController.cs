using System;

using UIKit;

namespace DeviceClientSampleiOS
{
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client;

    public partial class ViewController : UIViewController
    {
        int count = 1;

        // It is NOT a good practice to put device credentials in the code as shown below.
        // This is done in this sample for simplicity purposes.
        // Good practices such as the use of TPM or other secure storage on the device are recommended to store device credentials.
        private const string DeviceConnectionString = "<replace>";
        DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(DeviceConnectionString, TransportType.Http1);

        public ViewController(IntPtr handle) : base(handle)
        {
        }

        public override void ViewDidLoad()
        {
            base.ViewDidLoad();
            // Perform any additional setup after loading the view, typically from a nib.
        }

        public override void DidReceiveMemoryWarning()
        {
            base.DidReceiveMemoryWarning();
            // Release any cached data, images, etc that aren't in use.
        }

        partial void SendMessagesButton_TouchUpInside(UIButton sender)
        {
            string dataBuffer = string.Format("Message from iOS: {0}_{1}", this.count, Guid.NewGuid().ToString());
            var eventMessage = new Message(Encoding.UTF8.GetBytes(dataBuffer));
            this.deviceClient.SendEventAsync(eventMessage);
            string message = string.Format("{0} messages sent to IotHub", count++);
            sender.SetTitle(message, UIControlState.Normal);
        }

        partial void UIButton15_TouchUpInside(UIButton sender)
        {
            var asyncTask = Task.Run(() => this.deviceClient.ReceiveAsync());
            var receivedMessage = asyncTask.Result;
            if (receivedMessage != null)
            {
                this.receivedMessagesTextBox.Text += Encoding.Default.GetString(receivedMessage.GetBytes()) + "\r\n";
            }
            Console.WriteLine("Message received, now completing it...");
            Task.Run(() => this.deviceClient.CompleteAsync(receivedMessage));
            Console.WriteLine("Message completed.");
        }
    }
}