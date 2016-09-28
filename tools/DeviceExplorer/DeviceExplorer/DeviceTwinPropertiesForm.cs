using Microsoft.Azure.Devices;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DeviceExplorer
{
    public partial class DeviceTwinPropertiesForm : Form
    {
        private String iotHubConnectionString;
        private String deviceName;
        private List<string> deviceList;
        private String deviceJson;
        private String tagsJson;
        private String reportedPropertiesJson;
        private String desiredPropertiesJson;
        private bool initialIndexSet;
        private dynamic registryManager;
        private bool runOnce = true;

        public DeviceTwinPropertiesForm()
        {
            InitializeComponent();
        }

        public async Task<bool> GetDeviceTwinData()
        {
            DeviceTwinAndMethod deviceMethod = new DeviceTwinAndMethod(iotHubConnectionString, deviceName);
            DeviceTwinData deviceTwinData = await deviceMethod.GetDeviceTwinData();

            deviceJson = deviceTwinData.deviceJson;
            tagsJson = deviceTwinData.tagsJson;
            reportedPropertiesJson = deviceTwinData.reportedPropertiesJson;
            desiredPropertiesJson = deviceTwinData.desiredPropertiesJson;

            if (deviceJson == null)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        public async Task<bool> UpdateDialogData()
        {
            bool isOK = await GetDeviceTwinData();

            jsonRichTextBox0.Text = deviceJson;
            jsonRichTextBox1.Text = tagsJson;
            jsonRichTextBox2.Text = reportedPropertiesJson;
            jsonRichTextBox3.Text = desiredPropertiesJson;

            if (runOnce)
            {
                runOnce = false;
                jsonEditRichTextBox.Text = "{ \"properties\": { \"desired\": { " + Environment.NewLine + Environment.NewLine + Environment.NewLine + Environment.NewLine + "}}}";
            }

            return isOK;
        }

        public async void Execute(String activeIotHubConnectionString, String selectedDeviceName, List<string> currentDeviceList)
        {
            iotHubConnectionString = activeIotHubConnectionString;
            deviceName = selectedDeviceName;
            deviceList = currentDeviceList;

            initialIndexSet = true;

            deviceListCombo.Items.Clear();
            if (deviceList != null)
            {
                for (int i = 0; i < deviceList.Count; i++)
                {
                    deviceListCombo.Items.Add(deviceList[i]);
                }
            }
            deviceListCombo.SelectedItem = deviceName;

            if (await UpdateDialogData())
            {
                ShowDialog();
            }
        }

        private void deviceListCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!initialIndexSet)
            {
                deviceName = deviceListCombo.SelectedItem.ToString();
                refreshBtn_Click(this, null);
            }
            initialIndexSet = false;
        }

        private async void refreshBtn_Click(object sender, EventArgs e)
        {
            await UpdateDialogData();
        }

        private async void sendBtn_Click(object sender, EventArgs e)
        {
            jsonRichTextBox0.Text = "";
            jsonRichTextBox1.Text = "";
            jsonRichTextBox2.Text = "";
            jsonRichTextBox3.Text = "";

            DeviceTwinAndMethod deviceMethod = new DeviceTwinAndMethod(iotHubConnectionString, deviceName);
            await deviceMethod.UpdateTwinData(jsonEditRichTextBox.Text);

            refreshBtn_Click(this, null);
        }
    }
}
