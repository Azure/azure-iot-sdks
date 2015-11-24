using Microsoft.Azure.Devices;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace DeviceExplorer
{
    public partial class DeviceUpdateForm : Form
    {
        private RegistryManager registryManager;
        private string selectedDeviceID;
        private int devicesMaxCount;

        private string lastValidPrimaryKey;
        private string lastValidSecondaryKey;

        public DeviceUpdateForm(RegistryManager manager, int maxDevices, string deviceID)
        {
            InitializeComponent();
            this.registryManager = manager;
            this.devicesMaxCount = maxDevices;
            this.selectedDeviceID = deviceID;
            updateControls(deviceID);
        }

        private async void updateControls(string selectedDevice)
        {
            List<string> deviceIds = new List<string>();
            try
            {
                var devices = await registryManager.GetDevicesAsync(devicesMaxCount);
                foreach (var device in devices)
                {
                    deviceIds.Add(device.Id);
                }
                this.deviceIDComboBox.DataSource = deviceIds.OrderBy(c => c).ToList();

                // To capture the deviceId highlighted in the DataGridView
                foreach(var item in this.deviceIDComboBox.Items)
                {
                    if (item.ToString() == selectedDevice)
                    {
                        deviceIDComboBox.SelectedItem = item;
                        break;
                    }
                }
                foreach(var device in devices)
                {
                    if(device.Id == selectedDevice)
                    {
                        primaryKeyTextBox.Text = device.Authentication.SymmetricKey.PrimaryKey;
                        secondaryKeyTextBox.Text = device.Authentication.SymmetricKey.SecondaryKey;
                        lastValidPrimaryKey = primaryKeyTextBox.Text;
                        lastValidSecondaryKey = secondaryKeyTextBox.Text;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                using (new CenterDialog(this))
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        private async void updateButton_Click(object sender, EventArgs e)
        {
            try
            {
                var devices = await registryManager.GetDevicesAsync(devicesMaxCount);
                Device updatedDevice = null;
                foreach (var device in devices)
                {
                    if (device.Id == selectedDeviceID)
                    {
                        updatedDevice = device;
                        break;
                    }
                }

                if (updatedDevice != null)
                {
                    updatedDevice.Authentication.SymmetricKey.PrimaryKey = primaryKeyTextBox.Text;
                    updatedDevice.Authentication.SymmetricKey.SecondaryKey = secondaryKeyTextBox.Text;
                    await registryManager.UpdateDeviceAsync(updatedDevice, true);
                    using (new CenterDialog(this))
                    {
                        MessageBox.Show("Device updated successfully", "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    this.Close();
                }
                else
                {
                    using (new CenterDialog(this))
                    {
                        MessageBox.Show("Device not found!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    this.Close();
                }
            }
            catch (Exception ex)
            {
                using (new CenterDialog(this))
                {
                    MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void deviceIDComboBox_DropDownClosed(object sender, EventArgs e)
        {
            updateControls(this.deviceIDComboBox.SelectedItem.ToString());
        }

        private void restoreButton_Click(object sender, EventArgs e)
        {
            primaryKeyTextBox.Text = lastValidPrimaryKey;
            secondaryKeyTextBox.Text = lastValidSecondaryKey;
        }

        private async void DeviceUpdateForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            await registryManager.CloseAsync();
        }
    }
}
