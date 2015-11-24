namespace DeviceExplorer
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.targetTextBox = new System.Windows.Forms.TextBox();
            this.keyValueTextBox = new System.Windows.Forms.TextBox();
            this.keyNameTextBox = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.numericUpDownTTL = new System.Windows.Forms.NumericUpDown();
            this.sasRichTextBox = new System.Windows.Forms.RichTextBox();
            this.generateSASButton = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.protocolGatewayHost = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.updateSettingsButton = new System.Windows.Forms.Button();
            this.dhConStringTextBox = new System.Windows.Forms.TextBox();
            this.tabManagement = new System.Windows.Forms.TabPage();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.createDeviceButton = new System.Windows.Forms.Button();
            this.listDevicesButton = new System.Windows.Forms.Button();
            this.updateDeviceButton = new System.Windows.Forms.Button();
            this.deleteDeviceButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.deviceCountLabel = new System.Windows.Forms.Label();
            this.totalLabel = new System.Windows.Forms.Label();
            this.devicesGridView = new System.Windows.Forms.DataGridView();
            this.devicesGridViewContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.copyAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.copySelectedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.getDeviceConnectionStringToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabData = new System.Windows.Forms.TabPage();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.consumerGroupCheckBox = new System.Windows.Forms.CheckBox();
            this.label6 = new System.Windows.Forms.Label();
            this.groupNameTextBox = new System.Windows.Forms.TextBox();
            this.cancelMonitoringButton = new System.Windows.Forms.Button();
            this.label12 = new System.Windows.Forms.Label();
            this.clearDataButton = new System.Windows.Forms.Button();
            this.dataMonitorButton = new System.Windows.Forms.Button();
            this.label15 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.eventHubNameTextBoxForDataTab = new System.Windows.Forms.TextBox();
            this.deviceIDsComboBoxForEvent = new System.Windows.Forms.ComboBox();
            this.dateTimePicker = new System.Windows.Forms.DateTimePicker();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.eventHubTextBox = new System.Windows.Forms.RichTextBox();
            this.tabMessagesToDevice = new System.Windows.Forms.TabPage();
            this.checkBoxMonitorFeedbackEndpoint = new System.Windows.Forms.CheckBox();
            this.messageClearButton = new System.Windows.Forms.Button();
            this.deviceIDsComboBoxForCloudToDeviceMessage = new System.Windows.Forms.ComboBox();
            this.iotHubNameTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.messagesTextBox = new System.Windows.Forms.RichTextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.sendMessageToDeviceButton = new System.Windows.Forms.Button();
            this.textBoxMessage = new System.Windows.Forms.TextBox();
            this.ehStringToolTip = new System.Windows.Forms.ToolTip(this.components);
            this.tabControl1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox5.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownTTL)).BeginInit();
            this.groupBox6.SuspendLayout();
            this.tabManagement.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.devicesGridView)).BeginInit();
            this.devicesGridViewContextMenu.SuspendLayout();
            this.tabData.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabMessagesToDevice.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabManagement);
            this.tabControl1.Controls.Add(this.tabData);
            this.tabControl1.Controls.Add(this.tabMessagesToDevice);
            this.tabControl1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(1191, 630);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.Selected += new System.Windows.Forms.TabControlEventHandler(this.tabControl1_Selected);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.groupBox5);
            this.tabPage2.Controls.Add(this.groupBox6);
            this.tabPage2.Location = new System.Drawing.Point(4, 25);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(1183, 601);
            this.tabPage2.TabIndex = 3;
            this.tabPage2.Text = "Configuration";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // groupBox5
            // 
            this.groupBox5.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox5.Controls.Add(this.targetTextBox);
            this.groupBox5.Controls.Add(this.keyValueTextBox);
            this.groupBox5.Controls.Add(this.keyNameTextBox);
            this.groupBox5.Controls.Add(this.label9);
            this.groupBox5.Controls.Add(this.label4);
            this.groupBox5.Controls.Add(this.label3);
            this.groupBox5.Controls.Add(this.numericUpDownTTL);
            this.groupBox5.Controls.Add(this.sasRichTextBox);
            this.groupBox5.Controls.Add(this.generateSASButton);
            this.groupBox5.Controls.Add(this.label10);
            this.groupBox5.Location = new System.Drawing.Point(5, 310);
            this.groupBox5.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox5.Size = new System.Drawing.Size(1173, 288);
            this.groupBox5.TabIndex = 13;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Shared Access Signature";
            // 
            // targetTextBox
            // 
            this.targetTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.targetTextBox.Enabled = false;
            this.targetTextBox.Location = new System.Drawing.Point(95, 80);
            this.targetTextBox.Margin = new System.Windows.Forms.Padding(2);
            this.targetTextBox.Name = "targetTextBox";
            this.targetTextBox.Size = new System.Drawing.Size(1073, 22);
            this.targetTextBox.TabIndex = 16;
            // 
            // keyValueTextBox
            // 
            this.keyValueTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.keyValueTextBox.Enabled = false;
            this.keyValueTextBox.Location = new System.Drawing.Point(95, 54);
            this.keyValueTextBox.Margin = new System.Windows.Forms.Padding(2);
            this.keyValueTextBox.Name = "keyValueTextBox";
            this.keyValueTextBox.Size = new System.Drawing.Size(1073, 22);
            this.keyValueTextBox.TabIndex = 15;
            // 
            // keyNameTextBox
            // 
            this.keyNameTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.keyNameTextBox.Enabled = false;
            this.keyNameTextBox.Location = new System.Drawing.Point(95, 28);
            this.keyNameTextBox.Margin = new System.Windows.Forms.Padding(2);
            this.keyNameTextBox.Name = "keyNameTextBox";
            this.keyNameTextBox.Size = new System.Drawing.Size(1073, 22);
            this.keyNameTextBox.TabIndex = 14;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(41, 80);
            this.label9.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(48, 16);
            this.label9.TabIndex = 13;
            this.label9.Text = "Target";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(21, 54);
            this.label4.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(69, 16);
            this.label4.TabIndex = 12;
            this.label4.Text = "Key Value";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(20, 28);
            this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 16);
            this.label3.TabIndex = 11;
            this.label3.Text = "Key Name";
            // 
            // numericUpDownTTL
            // 
            this.numericUpDownTTL.Location = new System.Drawing.Point(95, 108);
            this.numericUpDownTTL.Margin = new System.Windows.Forms.Padding(2);
            this.numericUpDownTTL.Maximum = new decimal(new int[] {
            365,
            0,
            0,
            0});
            this.numericUpDownTTL.Name = "numericUpDownTTL";
            this.numericUpDownTTL.Size = new System.Drawing.Size(144, 22);
            this.numericUpDownTTL.TabIndex = 10;
            // 
            // sasRichTextBox
            // 
            this.sasRichTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.sasRichTextBox.Location = new System.Drawing.Point(4, 147);
            this.sasRichTextBox.Margin = new System.Windows.Forms.Padding(2);
            this.sasRichTextBox.Name = "sasRichTextBox";
            this.sasRichTextBox.ReadOnly = true;
            this.sasRichTextBox.Size = new System.Drawing.Size(1164, 137);
            this.sasRichTextBox.TabIndex = 9;
            this.sasRichTextBox.Text = "";
            // 
            // generateSASButton
            // 
            this.generateSASButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.generateSASButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.generateSASButton.Location = new System.Drawing.Point(972, 108);
            this.generateSASButton.Margin = new System.Windows.Forms.Padding(2);
            this.generateSASButton.Name = "generateSASButton";
            this.generateSASButton.Size = new System.Drawing.Size(183, 28);
            this.generateSASButton.TabIndex = 8;
            this.generateSASButton.Text = "Generate SAS";
            this.generateSASButton.UseVisualStyleBackColor = true;
            this.generateSASButton.Click += new System.EventHandler(this.generateSASButton_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(13, 108);
            this.label10.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(76, 16);
            this.label10.TabIndex = 3;
            this.label10.Text = "TTL (Days)";
            // 
            // groupBox6
            // 
            this.groupBox6.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox6.Controls.Add(this.protocolGatewayHost);
            this.groupBox6.Controls.Add(this.label16);
            this.groupBox6.Controls.Add(this.label1);
            this.groupBox6.Controls.Add(this.updateSettingsButton);
            this.groupBox6.Controls.Add(this.dhConStringTextBox);
            this.groupBox6.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox6.Location = new System.Drawing.Point(5, 24);
            this.groupBox6.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox6.Size = new System.Drawing.Size(1173, 268);
            this.groupBox6.TabIndex = 14;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Connection Information";
            // 
            // protocolGatewayHost
            // 
            this.protocolGatewayHost.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.protocolGatewayHost.Location = new System.Drawing.Point(5, 199);
            this.protocolGatewayHost.Name = "protocolGatewayHost";
            this.protocolGatewayHost.Size = new System.Drawing.Size(1163, 22);
            this.protocolGatewayHost.TabIndex = 17;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(8, 180);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(185, 16);
            this.label16.TabIndex = 16;
            this.label16.Text = "Protocol Gateway HostName:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(8, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(166, 16);
            this.label1.TabIndex = 0;
            this.label1.Text = "IoT Hub Connection String:";
            // 
            // updateSettingsButton
            // 
            this.updateSettingsButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.updateSettingsButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.updateSettingsButton.Location = new System.Drawing.Point(5, 227);
            this.updateSettingsButton.Name = "updateSettingsButton";
            this.updateSettingsButton.Size = new System.Drawing.Size(131, 31);
            this.updateSettingsButton.TabIndex = 1;
            this.updateSettingsButton.Text = "Update";
            this.updateSettingsButton.UseVisualStyleBackColor = true;
            this.updateSettingsButton.Click += new System.EventHandler(this.updateSettingsButton_Click);
            // 
            // dhConStringTextBox
            // 
            this.dhConStringTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dhConStringTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.dhConStringTextBox.Location = new System.Drawing.Point(5, 39);
            this.dhConStringTextBox.Multiline = true;
            this.dhConStringTextBox.Name = "dhConStringTextBox";
            this.dhConStringTextBox.Size = new System.Drawing.Size(1163, 138);
            this.dhConStringTextBox.TabIndex = 4;
            this.dhConStringTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dhConStringTextBox_KeyDown);
            // 
            // tabManagement
            // 
            this.tabManagement.Controls.Add(this.groupBox7);
            this.tabManagement.Controls.Add(this.groupBox2);
            this.tabManagement.Location = new System.Drawing.Point(4, 25);
            this.tabManagement.Name = "tabManagement";
            this.tabManagement.Padding = new System.Windows.Forms.Padding(3);
            this.tabManagement.Size = new System.Drawing.Size(502, 601);
            this.tabManagement.TabIndex = 0;
            this.tabManagement.Text = "Management";
            this.tabManagement.UseVisualStyleBackColor = true;
            // 
            // groupBox7
            // 
            this.groupBox7.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox7.Controls.Add(this.createDeviceButton);
            this.groupBox7.Controls.Add(this.listDevicesButton);
            this.groupBox7.Controls.Add(this.updateDeviceButton);
            this.groupBox7.Controls.Add(this.deleteDeviceButton);
            this.groupBox7.Location = new System.Drawing.Point(6, 18);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(531, 59);
            this.groupBox7.TabIndex = 8;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Actions";
            // 
            // createDeviceButton
            // 
            this.createDeviceButton.Location = new System.Drawing.Point(6, 21);
            this.createDeviceButton.Name = "createDeviceButton";
            this.createDeviceButton.Size = new System.Drawing.Size(100, 30);
            this.createDeviceButton.TabIndex = 1;
            this.createDeviceButton.Text = "Create";
            this.createDeviceButton.UseVisualStyleBackColor = true;
            this.createDeviceButton.Click += new System.EventHandler(this.createButton_Click);
            // 
            // listDevicesButton
            // 
            this.listDevicesButton.Location = new System.Drawing.Point(130, 21);
            this.listDevicesButton.Name = "listDevicesButton";
            this.listDevicesButton.Size = new System.Drawing.Size(100, 30);
            this.listDevicesButton.TabIndex = 2;
            this.listDevicesButton.Text = "List";
            this.listDevicesButton.UseVisualStyleBackColor = true;
            this.listDevicesButton.Click += new System.EventHandler(this.listButton_Click);
            // 
            // updateDeviceButton
            // 
            this.updateDeviceButton.Location = new System.Drawing.Point(255, 21);
            this.updateDeviceButton.Name = "updateDeviceButton";
            this.updateDeviceButton.Size = new System.Drawing.Size(100, 30);
            this.updateDeviceButton.TabIndex = 4;
            this.updateDeviceButton.Text = "Update";
            this.updateDeviceButton.UseVisualStyleBackColor = true;
            this.updateDeviceButton.Click += new System.EventHandler(this.updateDeviceButton_Click);
            // 
            // deleteDeviceButton
            // 
            this.deleteDeviceButton.Location = new System.Drawing.Point(384, 21);
            this.deleteDeviceButton.Name = "deleteDeviceButton";
            this.deleteDeviceButton.Size = new System.Drawing.Size(100, 30);
            this.deleteDeviceButton.TabIndex = 5;
            this.deleteDeviceButton.Text = "Delete";
            this.deleteDeviceButton.UseVisualStyleBackColor = true;
            this.deleteDeviceButton.Click += new System.EventHandler(this.deleteButton_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.deviceCountLabel);
            this.groupBox2.Controls.Add(this.totalLabel);
            this.groupBox2.Controls.Add(this.devicesGridView);
            this.groupBox2.Location = new System.Drawing.Point(6, 83);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(490, 512);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Devices";
            // 
            // deviceCountLabel
            // 
            this.deviceCountLabel.AutoSize = true;
            this.deviceCountLabel.Location = new System.Drawing.Point(55, 22);
            this.deviceCountLabel.Name = "deviceCountLabel";
            this.deviceCountLabel.Size = new System.Drawing.Size(15, 16);
            this.deviceCountLabel.TabIndex = 12;
            this.deviceCountLabel.Text = "0";
            // 
            // totalLabel
            // 
            this.totalLabel.AutoSize = true;
            this.totalLabel.Location = new System.Drawing.Point(7, 22);
            this.totalLabel.Name = "totalLabel";
            this.totalLabel.Size = new System.Drawing.Size(42, 16);
            this.totalLabel.TabIndex = 10;
            this.totalLabel.Text = "Total:";
            // 
            // devicesGridView
            // 
            this.devicesGridView.AllowUserToOrderColumns = true;
            this.devicesGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.devicesGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.devicesGridView.ContextMenuStrip = this.devicesGridViewContextMenu;
            this.devicesGridView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.devicesGridView.Location = new System.Drawing.Point(5, 49);
            this.devicesGridView.Margin = new System.Windows.Forms.Padding(2);
            this.devicesGridView.MultiSelect = false;
            this.devicesGridView.Name = "devicesGridView";
            this.devicesGridView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.devicesGridView.Size = new System.Drawing.Size(480, 458);
            this.devicesGridView.TabIndex = 9;
            this.devicesGridView.DataBindingComplete += new System.Windows.Forms.DataGridViewBindingCompleteEventHandler(this.devicesGridView_DataBindingComplete);
            // 
            // devicesGridViewContextMenu
            // 
            this.devicesGridViewContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.copyAllToolStripMenuItem,
            this.copySelectedToolStripMenuItem,
            this.toolStripSeparator1,
            this.getDeviceConnectionStringToolStripMenuItem});
            this.devicesGridViewContextMenu.Name = "devicesGridViewContextMenu";
            this.devicesGridViewContextMenu.Size = new System.Drawing.Size(300, 76);
            // 
            // copyAllToolStripMenuItem
            // 
            this.copyAllToolStripMenuItem.Name = "copyAllToolStripMenuItem";
            this.copyAllToolStripMenuItem.Size = new System.Drawing.Size(299, 22);
            this.copyAllToolStripMenuItem.Text = "Copy data for all device";
            this.copyAllToolStripMenuItem.Click += new System.EventHandler(this.copyAllToolStripMenuItem_Click);
            // 
            // copySelectedToolStripMenuItem
            // 
            this.copySelectedToolStripMenuItem.Name = "copySelectedToolStripMenuItem";
            this.copySelectedToolStripMenuItem.Size = new System.Drawing.Size(299, 22);
            this.copySelectedToolStripMenuItem.Text = "Copy data for selected device";
            this.copySelectedToolStripMenuItem.Click += new System.EventHandler(this.copySelectedToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(296, 6);
            // 
            // getDeviceConnectionStringToolStripMenuItem
            // 
            this.getDeviceConnectionStringToolStripMenuItem.Name = "getDeviceConnectionStringToolStripMenuItem";
            this.getDeviceConnectionStringToolStripMenuItem.Size = new System.Drawing.Size(299, 22);
            this.getDeviceConnectionStringToolStripMenuItem.Text = "Copy connection string for selected device";
            this.getDeviceConnectionStringToolStripMenuItem.Click += new System.EventHandler(this.getDeviceConnectionStringToolStripMenuItem_Click);
            // 
            // tabData
            // 
            this.tabData.Controls.Add(this.groupBox3);
            this.tabData.Controls.Add(this.groupBox1);
            this.tabData.Location = new System.Drawing.Point(4, 25);
            this.tabData.Name = "tabData";
            this.tabData.Padding = new System.Windows.Forms.Padding(3);
            this.tabData.Size = new System.Drawing.Size(1183, 601);
            this.tabData.TabIndex = 1;
            this.tabData.Text = "Data";
            this.tabData.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.consumerGroupCheckBox);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.groupNameTextBox);
            this.groupBox3.Controls.Add(this.cancelMonitoringButton);
            this.groupBox3.Controls.Add(this.label12);
            this.groupBox3.Controls.Add(this.clearDataButton);
            this.groupBox3.Controls.Add(this.dataMonitorButton);
            this.groupBox3.Controls.Add(this.label15);
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Controls.Add(this.eventHubNameTextBoxForDataTab);
            this.groupBox3.Controls.Add(this.deviceIDsComboBoxForEvent);
            this.groupBox3.Controls.Add(this.dateTimePicker);
            this.groupBox3.Location = new System.Drawing.Point(6, 24);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(1171, 247);
            this.groupBox3.TabIndex = 26;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Monitoring";
            // 
            // consumerGroupCheckBox
            // 
            this.consumerGroupCheckBox.AutoSize = true;
            this.consumerGroupCheckBox.Location = new System.Drawing.Point(407, 157);
            this.consumerGroupCheckBox.Name = "consumerGroupCheckBox";
            this.consumerGroupCheckBox.Size = new System.Drawing.Size(70, 20);
            this.consumerGroupCheckBox.TabIndex = 29;
            this.consumerGroupCheckBox.Text = "Enable";
            this.consumerGroupCheckBox.UseVisualStyleBackColor = true;
            this.consumerGroupCheckBox.CheckedChanged += new System.EventHandler(this.consumerGroupCheckBox_CheckedChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(22, 161);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(112, 16);
            this.label6.TabIndex = 28;
            this.label6.Text = "Consumer Group:";
            // 
            // groupNameTextBox
            // 
            this.groupNameTextBox.Enabled = false;
            this.groupNameTextBox.Location = new System.Drawing.Point(140, 155);
            this.groupNameTextBox.Name = "groupNameTextBox";
            this.groupNameTextBox.Size = new System.Drawing.Size(258, 22);
            this.groupNameTextBox.TabIndex = 27;
            // 
            // cancelMonitoringButton
            // 
            this.cancelMonitoringButton.Location = new System.Drawing.Point(182, 202);
            this.cancelMonitoringButton.Name = "cancelMonitoringButton";
            this.cancelMonitoringButton.Size = new System.Drawing.Size(133, 30);
            this.cancelMonitoringButton.TabIndex = 26;
            this.cancelMonitoringButton.Text = "Cancel";
            this.cancelMonitoringButton.UseVisualStyleBackColor = true;
            this.cancelMonitoringButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(22, 40);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(73, 16);
            this.label12.TabIndex = 15;
            this.label12.Text = "Event Hub:";
            // 
            // clearDataButton
            // 
            this.clearDataButton.Location = new System.Drawing.Point(353, 202);
            this.clearDataButton.Name = "clearDataButton";
            this.clearDataButton.Size = new System.Drawing.Size(120, 30);
            this.clearDataButton.TabIndex = 21;
            this.clearDataButton.Text = "Clear";
            this.clearDataButton.UseVisualStyleBackColor = true;
            this.clearDataButton.Click += new System.EventHandler(this.clearDataButton_Click);
            // 
            // dataMonitorButton
            // 
            this.dataMonitorButton.Location = new System.Drawing.Point(21, 202);
            this.dataMonitorButton.Name = "dataMonitorButton";
            this.dataMonitorButton.Size = new System.Drawing.Size(120, 30);
            this.dataMonitorButton.TabIndex = 4;
            this.dataMonitorButton.Text = "Monitor";
            this.dataMonitorButton.UseVisualStyleBackColor = true;
            this.dataMonitorButton.Click += new System.EventHandler(this.dataMonitorButton_Click);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(22, 121);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(72, 16);
            this.label15.TabIndex = 23;
            this.label15.Text = "Start Time:";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(22, 81);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(70, 16);
            this.label11.TabIndex = 2;
            this.label11.Text = "Device ID:";
            // 
            // eventHubNameTextBoxForDataTab
            // 
            this.eventHubNameTextBoxForDataTab.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.eventHubNameTextBoxForDataTab.Location = new System.Drawing.Point(101, 34);
            this.eventHubNameTextBoxForDataTab.Name = "eventHubNameTextBoxForDataTab";
            this.eventHubNameTextBoxForDataTab.ReadOnly = true;
            this.eventHubNameTextBoxForDataTab.Size = new System.Drawing.Size(1054, 22);
            this.eventHubNameTextBoxForDataTab.TabIndex = 17;
            // 
            // deviceIDsComboBoxForEvent
            // 
            this.deviceIDsComboBoxForEvent.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.deviceIDsComboBoxForEvent.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.deviceIDsComboBoxForEvent.FormattingEnabled = true;
            this.deviceIDsComboBoxForEvent.Location = new System.Drawing.Point(101, 73);
            this.deviceIDsComboBoxForEvent.Name = "deviceIDsComboBoxForEvent";
            this.deviceIDsComboBoxForEvent.Size = new System.Drawing.Size(1054, 24);
            this.deviceIDsComboBoxForEvent.TabIndex = 18;
            this.deviceIDsComboBoxForEvent.SelectionChangeCommitted += new System.EventHandler(this.deviceIDsComboBoxForEvent_SelectionChangeCommitted);
            // 
            // dateTimePicker
            // 
            this.dateTimePicker.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dateTimePicker.Checked = false;
            this.dateTimePicker.CustomFormat = "MM/dd/yyyy HH:mm:ss";
            this.dateTimePicker.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.dateTimePicker.Location = new System.Drawing.Point(101, 115);
            this.dateTimePicker.Name = "dateTimePicker";
            this.dateTimePicker.ShowCheckBox = true;
            this.dateTimePicker.Size = new System.Drawing.Size(1054, 22);
            this.dateTimePicker.TabIndex = 22;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.eventHubTextBox);
            this.groupBox1.Location = new System.Drawing.Point(6, 292);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(1171, 303);
            this.groupBox1.TabIndex = 14;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Event Hub Data";
            // 
            // eventHubTextBox
            // 
            this.eventHubTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.eventHubTextBox.Location = new System.Drawing.Point(8, 21);
            this.eventHubTextBox.Name = "eventHubTextBox";
            this.eventHubTextBox.ReadOnly = true;
            this.eventHubTextBox.Size = new System.Drawing.Size(1157, 276);
            this.eventHubTextBox.TabIndex = 5;
            this.eventHubTextBox.Text = "";
            // 
            // tabMessagesToDevice
            // 
            this.tabMessagesToDevice.Controls.Add(this.checkBoxMonitorFeedbackEndpoint);
            this.tabMessagesToDevice.Controls.Add(this.messageClearButton);
            this.tabMessagesToDevice.Controls.Add(this.deviceIDsComboBoxForCloudToDeviceMessage);
            this.tabMessagesToDevice.Controls.Add(this.iotHubNameTextBox);
            this.tabMessagesToDevice.Controls.Add(this.label2);
            this.tabMessagesToDevice.Controls.Add(this.checkBox1);
            this.tabMessagesToDevice.Controls.Add(this.groupBox4);
            this.tabMessagesToDevice.Controls.Add(this.label8);
            this.tabMessagesToDevice.Controls.Add(this.label7);
            this.tabMessagesToDevice.Controls.Add(this.label5);
            this.tabMessagesToDevice.Controls.Add(this.sendMessageToDeviceButton);
            this.tabMessagesToDevice.Controls.Add(this.textBoxMessage);
            this.tabMessagesToDevice.Location = new System.Drawing.Point(4, 25);
            this.tabMessagesToDevice.Name = "tabMessagesToDevice";
            this.tabMessagesToDevice.Padding = new System.Windows.Forms.Padding(3);
            this.tabMessagesToDevice.Size = new System.Drawing.Size(1183, 601);
            this.tabMessagesToDevice.TabIndex = 2;
            this.tabMessagesToDevice.Text = "Messages To Device";
            this.tabMessagesToDevice.UseVisualStyleBackColor = true;
            // 
            // checkBoxMonitorFeedbackEndpoint
            // 
            this.checkBoxMonitorFeedbackEndpoint.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.checkBoxMonitorFeedbackEndpoint.AutoSize = true;
            this.checkBoxMonitorFeedbackEndpoint.Location = new System.Drawing.Point(281, 148);
            this.checkBoxMonitorFeedbackEndpoint.Name = "checkBoxMonitorFeedbackEndpoint";
            this.checkBoxMonitorFeedbackEndpoint.Size = new System.Drawing.Size(192, 20);
            this.checkBoxMonitorFeedbackEndpoint.TabIndex = 12;
            this.checkBoxMonitorFeedbackEndpoint.Text = "Monitor Feedback Endpoint";
            this.checkBoxMonitorFeedbackEndpoint.UseVisualStyleBackColor = true;
            this.checkBoxMonitorFeedbackEndpoint.CheckedChanged += new System.EventHandler(this.checkBoxMonitorFeedbackEndpoint_CheckedChanged);
            // 
            // messageClearButton
            // 
            this.messageClearButton.Location = new System.Drawing.Point(205, 189);
            this.messageClearButton.MaximumSize = new System.Drawing.Size(145, 30);
            this.messageClearButton.Name = "messageClearButton";
            this.messageClearButton.Size = new System.Drawing.Size(145, 30);
            this.messageClearButton.TabIndex = 11;
            this.messageClearButton.Text = "Clear";
            this.messageClearButton.UseVisualStyleBackColor = true;
            this.messageClearButton.Click += new System.EventHandler(this.messageClearButton_Click);
            // 
            // deviceIDsComboBoxForCloudToDeviceMessage
            // 
            this.deviceIDsComboBoxForCloudToDeviceMessage.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.deviceIDsComboBoxForCloudToDeviceMessage.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.deviceIDsComboBoxForCloudToDeviceMessage.FormattingEnabled = true;
            this.deviceIDsComboBoxForCloudToDeviceMessage.Location = new System.Drawing.Point(104, 81);
            this.deviceIDsComboBoxForCloudToDeviceMessage.Name = "deviceIDsComboBoxForCloudToDeviceMessage";
            this.deviceIDsComboBoxForCloudToDeviceMessage.Size = new System.Drawing.Size(1067, 24);
            this.deviceIDsComboBoxForCloudToDeviceMessage.TabIndex = 10;
            this.deviceIDsComboBoxForCloudToDeviceMessage.SelectionChangeCommitted += new System.EventHandler(this.deviceIDsComboBoxForMessage_SelectionChangeCommitted);
            // 
            // iotHubNameTextBox
            // 
            this.iotHubNameTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.iotHubNameTextBox.Location = new System.Drawing.Point(104, 53);
            this.iotHubNameTextBox.Name = "iotHubNameTextBox";
            this.iotHubNameTextBox.ReadOnly = true;
            this.iotHubNameTextBox.Size = new System.Drawing.Size(1067, 22);
            this.iotHubNameTextBox.TabIndex = 9;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(21, 56);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(59, 16);
            this.label2.TabIndex = 8;
            this.label2.Text = "IoT Hub:";
            // 
            // checkBox1
            // 
            this.checkBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(104, 148);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(128, 20);
            this.checkBox1.TabIndex = 5;
            this.checkBox1.Text = "Add Time Stamp";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.messagesTextBox);
            this.groupBox4.Location = new System.Drawing.Point(6, 225);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(1165, 370);
            this.groupBox4.TabIndex = 7;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Output";
            // 
            // messagesTextBox
            // 
            this.messagesTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.messagesTextBox.Location = new System.Drawing.Point(6, 21);
            this.messagesTextBox.Name = "messagesTextBox";
            this.messagesTextBox.ReadOnly = true;
            this.messagesTextBox.Size = new System.Drawing.Size(1153, 343);
            this.messagesTextBox.TabIndex = 7;
            this.messagesTextBox.Text = "";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(21, 114);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(68, 16);
            this.label8.TabIndex = 3;
            this.label8.Text = "Message:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(21, 84);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(70, 16);
            this.label7.TabIndex = 1;
            this.label7.Text = "Device ID:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 19);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(163, 16);
            this.label5.TabIndex = 2;
            this.label5.Text = "Send Message to Device:";
            // 
            // sendMessageToDeviceButton
            // 
            this.sendMessageToDeviceButton.Location = new System.Drawing.Point(24, 189);
            this.sendMessageToDeviceButton.MaximumSize = new System.Drawing.Size(145, 30);
            this.sendMessageToDeviceButton.Name = "sendMessageToDeviceButton";
            this.sendMessageToDeviceButton.Size = new System.Drawing.Size(145, 30);
            this.sendMessageToDeviceButton.TabIndex = 6;
            this.sendMessageToDeviceButton.Text = "Send";
            this.sendMessageToDeviceButton.UseVisualStyleBackColor = true;
            this.sendMessageToDeviceButton.Click += new System.EventHandler(this.sendMessageToDeviceButton_Click);
            // 
            // textBoxMessage
            // 
            this.textBoxMessage.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxMessage.Location = new System.Drawing.Point(104, 111);
            this.textBoxMessage.Name = "textBoxMessage";
            this.textBoxMessage.Size = new System.Drawing.Size(1067, 22);
            this.textBoxMessage.TabIndex = 4;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1215, 646);
            this.Controls.Add(this.tabControl1);
            this.MinimumSize = new System.Drawing.Size(550, 685);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Device Explorer";
            this.Shown += new System.EventHandler(this.MainForm_Shown);
            this.tabControl1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownTTL)).EndInit();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.tabManagement.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.devicesGridView)).EndInit();
            this.devicesGridViewContextMenu.ResumeLayout(false);
            this.tabData.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.tabMessagesToDevice.ResumeLayout(false);
            this.tabMessagesToDevice.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabManagement;
        private System.Windows.Forms.TabPage tabData;
        private System.Windows.Forms.TabPage tabMessagesToDevice;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox dhConStringTextBox;
        private System.Windows.Forms.Button createDeviceButton;
        private System.Windows.Forms.Button listDevicesButton;
        private System.Windows.Forms.Button updateDeviceButton;
        private System.Windows.Forms.Button deleteDeviceButton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RichTextBox eventHubTextBox;
        private System.Windows.Forms.Button sendMessageToDeviceButton;
        private System.Windows.Forms.TextBox textBoxMessage;
        private System.Windows.Forms.Button dataMonitorButton;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.RichTextBox messagesTextBox;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Button updateSettingsButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.RichTextBox sasRichTextBox;
        private System.Windows.Forms.Button generateSASButton;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.NumericUpDown numericUpDownTTL;
        private System.Windows.Forms.TextBox targetTextBox;
        private System.Windows.Forms.TextBox keyValueTextBox;
        private System.Windows.Forms.TextBox keyNameTextBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox eventHubNameTextBoxForDataTab;
        private System.Windows.Forms.TextBox iotHubNameTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox deviceIDsComboBoxForEvent;
        private System.Windows.Forms.ComboBox deviceIDsComboBoxForCloudToDeviceMessage;
        private System.Windows.Forms.Button clearDataButton;
        private System.Windows.Forms.DataGridView devicesGridView;
        private System.Windows.Forms.Button messageClearButton;
        private System.Windows.Forms.ToolTip ehStringToolTip;
        private System.Windows.Forms.DateTimePicker dateTimePicker;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button cancelMonitoringButton;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox groupNameTextBox;
        private System.Windows.Forms.CheckBox consumerGroupCheckBox;
        private System.Windows.Forms.ContextMenuStrip devicesGridViewContextMenu;
        private System.Windows.Forms.ToolStripMenuItem copyAllToolStripMenuItem;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem copySelectedToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem getDeviceConnectionStringToolStripMenuItem;
        private System.Windows.Forms.CheckBox checkBoxMonitorFeedbackEndpoint;
        private System.Windows.Forms.TextBox protocolGatewayHost;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label deviceCountLabel;
        private System.Windows.Forms.Label totalLabel;
    }
}

