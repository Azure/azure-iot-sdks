using Microsoft.Azure.Devices;
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
        private System.Windows.Forms.PropertyGrid propertyGrid;
        private String iotHubConnectionString;
        private String deviceName;
        private List<string> deviceList;
        private DeviceTwinProperties reportedProperties;
        private DeviceTwinProperties desiredProperties;
        private String reportedPropertiesJson;
        private String desiredPropertiesJson;
        private bool initialIndexSet;

        public DeviceTwinPropertiesForm()
        {
            InitializeComponent();
        }

        public async Task<bool> GetDeviceTwinData()
        {
            bool isOK = false;
            string exStr = "";

            dynamic registryManager = RegistryManager.CreateFromConnectionString(iotHubConnectionString);
            dynamic repProps = null;
            dynamic desProps = null;
            dynamic tags = null;

            try
            {
                var deviceTwin = await registryManager.GetTwinAsync(deviceName);
                if (deviceTwin != null)
                {
                    tags = deviceTwin.Tags;
                    repProps = deviceTwin.Properties.Reported;
                    desProps = deviceTwin.Properties.Desired;

                    Console.WriteLine(repProps.ToJson());

                    if (repProps != null)
                    {
                        reportedProperties = new DeviceTwinProperties();

                        foreach (var property in repProps)
                        {
                            reportedProperties.Add(new DeviceTwinProperty(property.Key, property.Value, typeof(string), true));
                        }
                    }

                    if (desProps != null)
                    {
                        desiredProperties = new DeviceTwinProperties();
                        foreach (var property in desProps)
                        {
                            desiredProperties.Add(new DeviceTwinProperty(property.Key, property.Value, typeof(string), true));
                        }
                    }

                    reportedPropertiesJson = repProps.ToJson();
                    desiredPropertiesJson = desProps.ToJson();

                    isOK = true;
                }
            }
            catch (Exception e)
            {
                exStr = "Device Twin functionality is not found." + Environment.NewLine +
                    "Make sure you are using the latest Microsoft.Azure.Devices package.";
            }

            if (!isOK)
            {
                reportedProperties = null;
                desiredProperties = null;
                reportedPropertiesJson = null;
                desiredPropertiesJson = null;

                MessageBox.Show(exStr, "Device Twin Properties", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            return isOK;
        }

        public async Task<bool> UpdateDialogData()
        {
            bool isOK = await GetDeviceTwinData();

            reportedPropertiesGrid.SelectedObject = reportedProperties;
            reportedPropertiesGrid.Refresh();
            jsonRichTextBox1.Text = reportedPropertiesJson;

            desiredPropertiesGrid.SelectedObject = desiredProperties;
            desiredPropertiesGrid.Refresh();
            jsonRichTextBox2.Text = desiredPropertiesJson;

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
    }
}
