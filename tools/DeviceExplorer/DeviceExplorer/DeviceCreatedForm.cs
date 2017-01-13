using Microsoft.Azure.Devices;
using System;
using System.Windows.Forms;

namespace DeviceExplorer
{
    public partial class DeviceCreatedForm : Form
    {
        public DeviceCreatedForm(Device device)
        {
            InitializeComponent();
            if (device.Authentication.SymmetricKey != null)
            {
                richTextBox.Text = $"ID={device.Id}\nPrimaryKey={device.Authentication.SymmetricKey.PrimaryKey}\nSecondaryKey={device.Authentication.SymmetricKey.SecondaryKey}";
            }
            else if (device.Authentication.X509Thumbprint != null)
            {
                richTextBox.Text = $"ID={device.Id}\nPrimaryThumbPrint={device.Authentication.X509Thumbprint.PrimaryThumbprint}\nSecondaryThumbPrint={device.Authentication.X509Thumbprint.SecondaryThumbprint}";
            }
        }

        private void doneButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
