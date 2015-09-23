using System;
using System.Windows.Forms;

namespace DeviceExplorer
{
    public partial class DeviceCreatedForm : Form
    {
        public DeviceCreatedForm(string deviceID, string primaryKey, string secondaryKey)
        {
            InitializeComponent();
            richTextBox.Text = String.Format("ID={0}\nPrimaryKey={1}\nSecondaryKey={2}", deviceID, primaryKey, secondaryKey);
        }

        private void doneButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
