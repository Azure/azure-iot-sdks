using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using Microsoft.Azure.Devices.Common;
using Microsoft.Azure.Devices;

namespace DeviceExplorer
{
    public partial class ImportDevicesForm : Form
    {
        private string iotHubConnectionString = "";
        private RegistryManager registryManager;

        public event EventHandler DeviceImported;

        public ImportDevicesForm(string iotHubConnectionString)
        {
            InitializeComponent();

            this.iotHubConnectionString = iotHubConnectionString;
            this.registryManager = RegistryManager.CreateFromConnectionString(iotHubConnectionString);
        }

        private void filePathTextBox_TextChanged(object sender, EventArgs e)
        {
            if (File.Exists(filePathTextBox.Text))
                this.startButton.Enabled = true;
            else
                this.startButton.Enabled = false;
        }

        private void browseButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();

            ofd.InitialDirectory = "c:\\";
            ofd.Filter = "csv files (*.csv)|*.csv|All files (*.*)|*.*";
            ofd.FilterIndex = 1;
            ofd.RestoreDirectory = true;

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                this.filePathTextBox.Text = ofd.FileName;
            }
        }

        private async void startButton_Click(object sender, EventArgs e)
        {
            startButton.Enabled = false;
            this.filePathTextBox.Enabled = false;
            this.Cursor = Cursors.WaitCursor;

            this.importProgressBar.Minimum = 0;
            this.importProgressBar.Value = 0;
            this.importProgressBar.Step = 1;

            try
            {
                statusLabel.Text = "Reading file";
                var devicesToImport = GetDevicesToImport(filePathTextBox.Text);
                statusLabel.Text = string.Format("{0} devices found in file", devicesToImport.Count);

                this.importProgressBar.Maximum = devicesToImport.Count;
                var importedDeviceCount = 1;

                foreach(var deviceToImport in devicesToImport)
                {
                    statusLabel.Text = string.Format("Importing device {0} of {1}", importedDeviceCount, devicesToImport.Count);

                    var device = new Device(deviceToImport.Id);

                    await registryManager.AddDeviceAsync(device);

                    device = await registryManager.GetDeviceAsync(device.Id);

                    device.Authentication.SymmetricKey.PrimaryKey = deviceToImport.PrimaryKey;
                    device.Authentication.SymmetricKey.SecondaryKey = deviceToImport.SecondaryKey;
                    device = await registryManager.UpdateDeviceAsync(device);

                    OnDeviceImported(EventArgs.Empty);

                    importedDeviceCount += 1;
                    this.importProgressBar.PerformStep();
                }

                statusLabel.Text = string.Format("{0} devices imported", devicesToImport.Count);
            }
            catch(Exception ex)
            {
                statusLabel.Text = "Import Error";
                MessageBox.Show(string.Format("Error encountered during import: {0}", ex.Message), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            this.Cursor = Cursors.Default;
            this.filePathTextBox.Enabled = true;
            startButton.Enabled = true;
        }

        private List<ImportDeviceEntity> GetDevicesToImport(string filePath)
        {
            var devicesToImport = new List<ImportDeviceEntity>();

            using (var sr = new StreamReader(filePath))
            {
                while (sr.Peek() >= 0)
                {
                    var line = sr.ReadLine();

                    var data = line.Split(new char[] { ',' });

                    if (data.Length == 1)
                    {
                        if (!string.IsNullOrEmpty(data[0]))
                        {
                            var device = new ImportDeviceEntity();

                            device.Id = data[0];
                            device.PrimaryKey = CryptoKeyGenerator.GenerateKey(32);
                            device.SecondaryKey = CryptoKeyGenerator.GenerateKey(32);

                            devicesToImport.Add(device);
                        }
                    }
                    else if (data.Length == 3)
                    {
                        if (!string.IsNullOrEmpty(data[0]))
                        {
                            var device = new ImportDeviceEntity();

                            device.Id = data[0];

                            if (!string.IsNullOrEmpty(data[1]))
                                device.PrimaryKey = data[1];
                            else
                                device.PrimaryKey = CryptoKeyGenerator.GenerateKey(32);

                            if (!string.IsNullOrEmpty(data[2]))
                                device.SecondaryKey = data[2];
                            else
                                device.SecondaryKey = CryptoKeyGenerator.GenerateKey(32);

                            devicesToImport.Add(device);
                        }
                    }
                }

                return devicesToImport;
            }
        }   

        protected virtual void OnDeviceImported(EventArgs e)
        {
            var handler = DeviceImported;

            if (handler != null)
                handler(this, e);
        }
    }
}
