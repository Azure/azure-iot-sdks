using System;
using System.Windows.Forms;

namespace DeviceExplorer
{
    public partial class DeviceCreatedForm : Form
    {
        public DeviceCreatedForm(string deviceID, string primaryKey, string secondaryKey)
        {
            InitializeComponent();
            richTextBox.Text = $"ID={deviceID}\nPrimaryKey={primaryKey}\nSecondaryKey={secondaryKey}";
        }

        private void doneButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
