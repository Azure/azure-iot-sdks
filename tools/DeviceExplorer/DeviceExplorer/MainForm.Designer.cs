﻿namespace DeviceExplorer
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
            this.sasTokenButton = new System.Windows.Forms.Button();
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
            this.tabControl1.Location = new System.Drawing.Point(16, 15);
            this.tabControl1.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(853, 775);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.Selected += new System.Windows.Forms.TabControlEventHandler(this.tabControl1_Selected);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.groupBox5);
            this.tabPage2.Controls.Add(this.groupBox6);
            this.tabPage2.Location = new System.Drawing.Point(4, 29);
            this.tabPage2.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tabPage2.Size = new System.Drawing.Size(845, 742);
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
            this.groupBox5.Location = new System.Drawing.Point(7, 382);
            this.groupBox5.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox5.Size = new System.Drawing.Size(829, 354);
            this.groupBox5.TabIndex = 13;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Shared Access Signature";
            // 
            // targetTextBox
            // 
            this.targetTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.targetTextBox.Enabled = false;
            this.targetTextBox.Location = new System.Drawing.Point(127, 98);
            this.targetTextBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.targetTextBox.Name = "targetTextBox";
            this.targetTextBox.Size = new System.Drawing.Size(695, 26);
            this.targetTextBox.TabIndex = 16;
            // 
            // keyValueTextBox
            // 
            this.keyValueTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.keyValueTextBox.Enabled = false;
            this.keyValueTextBox.Location = new System.Drawing.Point(127, 66);
            this.keyValueTextBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.keyValueTextBox.Name = "keyValueTextBox";
            this.keyValueTextBox.Size = new System.Drawing.Size(695, 26);
            this.keyValueTextBox.TabIndex = 15;
            // 
            // keyNameTextBox
            // 
            this.keyNameTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.keyNameTextBox.Enabled = false;
            this.keyNameTextBox.Location = new System.Drawing.Point(127, 34);
            this.keyNameTextBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.keyNameTextBox.Name = "keyNameTextBox";
            this.keyNameTextBox.Size = new System.Drawing.Size(695, 26);
            this.keyNameTextBox.TabIndex = 14;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(55, 98);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(57, 20);
            this.label9.TabIndex = 13;
            this.label9.Text = "Target";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(28, 66);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(84, 20);
            this.label4.TabIndex = 12;
            this.label4.Text = "Key Value";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(27, 34);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(86, 20);
            this.label3.TabIndex = 11;
            this.label3.Text = "Key Name";
            // 
            // numericUpDownTTL
            // 
            this.numericUpDownTTL.Location = new System.Drawing.Point(127, 133);
            this.numericUpDownTTL.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.numericUpDownTTL.Maximum = new decimal(new int[] {
            365,
            0,
            0,
            0});
            this.numericUpDownTTL.Name = "numericUpDownTTL";
            this.numericUpDownTTL.Size = new System.Drawing.Size(192, 26);
            this.numericUpDownTTL.TabIndex = 10;
            // 
            // sasRichTextBox
            // 
            this.sasRichTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.sasRichTextBox.Location = new System.Drawing.Point(5, 181);
            this.sasRichTextBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.sasRichTextBox.Name = "sasRichTextBox";
            this.sasRichTextBox.ReadOnly = true;
            this.sasRichTextBox.Size = new System.Drawing.Size(816, 168);
            this.sasRichTextBox.TabIndex = 9;
            this.sasRichTextBox.Text = "";
            // 
            // generateSASButton
            // 
            this.generateSASButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.generateSASButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.generateSASButton.Location = new System.Drawing.Point(561, 133);
            this.generateSASButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.generateSASButton.Name = "generateSASButton";
            this.generateSASButton.Size = new System.Drawing.Size(244, 34);
            this.generateSASButton.TabIndex = 8;
            this.generateSASButton.Text = "Generate SAS";
            this.generateSASButton.UseVisualStyleBackColor = true;
            this.generateSASButton.Click += new System.EventHandler(this.generateSASButton_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(17, 133);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(95, 20);
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
            this.groupBox6.Location = new System.Drawing.Point(7, 30);
            this.groupBox6.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox6.Size = new System.Drawing.Size(829, 330);
            this.groupBox6.TabIndex = 14;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Connection Information";
            // 
            // protocolGatewayHost
            // 
            this.protocolGatewayHost.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.protocolGatewayHost.Location = new System.Drawing.Point(7, 245);
            this.protocolGatewayHost.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.protocolGatewayHost.Name = "protocolGatewayHost";
            this.protocolGatewayHost.Size = new System.Drawing.Size(815, 26);
            this.protocolGatewayHost.TabIndex = 17;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(11, 222);
            this.label16.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(231, 20);
            this.label16.TabIndex = 16;
            this.label16.Text = "Protocol Gateway HostName:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(11, 25);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(211, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "IoT Hub Connection String:";
            // 
            // updateSettingsButton
            // 
            this.updateSettingsButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.updateSettingsButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.updateSettingsButton.Location = new System.Drawing.Point(7, 279);
            this.updateSettingsButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.updateSettingsButton.Name = "updateSettingsButton";
            this.updateSettingsButton.Size = new System.Drawing.Size(175, 38);
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
            this.dhConStringTextBox.Location = new System.Drawing.Point(7, 48);
            this.dhConStringTextBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.dhConStringTextBox.Multiline = true;
            this.dhConStringTextBox.Name = "dhConStringTextBox";
            this.dhConStringTextBox.Size = new System.Drawing.Size(815, 169);
            this.dhConStringTextBox.TabIndex = 4;
            this.dhConStringTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dhConStringTextBox_KeyDown);
            // 
            // tabManagement
            // 
            this.tabManagement.Controls.Add(this.groupBox7);
            this.tabManagement.Controls.Add(this.groupBox2);
            this.tabManagement.Location = new System.Drawing.Point(4, 29);
            this.tabManagement.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tabManagement.Name = "tabManagement";
            this.tabManagement.Padding = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tabManagement.Size = new System.Drawing.Size(845, 742);
            this.tabManagement.TabIndex = 0;
            this.tabManagement.Text = "Management";
            this.tabManagement.UseVisualStyleBackColor = true;
            // 
            // groupBox7
            // 
            this.groupBox7.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox7.Controls.Add(this.sasTokenButton);
            this.groupBox7.Controls.Add(this.createDeviceButton);
            this.groupBox7.Controls.Add(this.listDevicesButton);
            this.groupBox7.Controls.Add(this.updateDeviceButton);
            this.groupBox7.Controls.Add(this.deleteDeviceButton);
            this.groupBox7.Location = new System.Drawing.Point(8, 22);
            this.groupBox7.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Padding = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox7.Size = new System.Drawing.Size(820, 73);
            this.groupBox7.TabIndex = 8;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Actions";
            // 
            // sasTokenButton
            // 
            this.sasTokenButton.Location = new System.Drawing.Point(672, 26);
            this.sasTokenButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.sasTokenButton.Name = "sasTokenButton";
            this.sasTokenButton.Size = new System.Drawing.Size(133, 37);
            this.sasTokenButton.TabIndex = 6;
            this.sasTokenButton.Text = "SAS Token...";
            this.sasTokenButton.UseVisualStyleBackColor = true;
            this.sasTokenButton.Click += new System.EventHandler(this.sasTokenButton_Click);
            // 
            // createDeviceButton
            // 
            this.createDeviceButton.Location = new System.Drawing.Point(8, 26);
            this.createDeviceButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.createDeviceButton.Name = "createDeviceButton";
            this.createDeviceButton.Size = new System.Drawing.Size(133, 37);
            this.createDeviceButton.TabIndex = 1;
            this.createDeviceButton.Text = "Create";
            this.createDeviceButton.UseVisualStyleBackColor = true;
            this.createDeviceButton.Click += new System.EventHandler(this.createButton_Click);
            // 
            // listDevicesButton
            // 
            this.listDevicesButton.Location = new System.Drawing.Point(173, 26);
            this.listDevicesButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.listDevicesButton.Name = "listDevicesButton";
            this.listDevicesButton.Size = new System.Drawing.Size(133, 37);
            this.listDevicesButton.TabIndex = 2;
            this.listDevicesButton.Text = "List";
            this.listDevicesButton.UseVisualStyleBackColor = true;
            this.listDevicesButton.Click += new System.EventHandler(this.listButton_Click);
            // 
            // updateDeviceButton
            // 
            this.updateDeviceButton.Location = new System.Drawing.Point(340, 26);
            this.updateDeviceButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.updateDeviceButton.Name = "updateDeviceButton";
            this.updateDeviceButton.Size = new System.Drawing.Size(133, 37);
            this.updateDeviceButton.TabIndex = 4;
            this.updateDeviceButton.Text = "Update";
            this.updateDeviceButton.UseVisualStyleBackColor = true;
            this.updateDeviceButton.Click += new System.EventHandler(this.updateDeviceButton_Click);
            // 
            // deleteDeviceButton
            // 
            this.deleteDeviceButton.Location = new System.Drawing.Point(512, 26);
            this.deleteDeviceButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.deleteDeviceButton.Name = "deleteDeviceButton";
            this.deleteDeviceButton.Size = new System.Drawing.Size(133, 37);
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
            this.groupBox2.Location = new System.Drawing.Point(8, 102);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox2.Size = new System.Drawing.Size(827, 630);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Devices";
            // 
            // deviceCountLabel
            // 
            this.deviceCountLabel.AutoSize = true;
            this.deviceCountLabel.Location = new System.Drawing.Point(73, 27);
            this.deviceCountLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.deviceCountLabel.Name = "deviceCountLabel";
            this.deviceCountLabel.Size = new System.Drawing.Size(18, 20);
            this.deviceCountLabel.TabIndex = 12;
            this.deviceCountLabel.Text = "0";
            // 
            // totalLabel
            // 
            this.totalLabel.AutoSize = true;
            this.totalLabel.Location = new System.Drawing.Point(9, 27);
            this.totalLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.totalLabel.Name = "totalLabel";
            this.totalLabel.Size = new System.Drawing.Size(51, 20);
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
            this.devicesGridView.Location = new System.Drawing.Point(7, 60);
            this.devicesGridView.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.devicesGridView.MultiSelect = false;
            this.devicesGridView.Name = "devicesGridView";
            this.devicesGridView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.devicesGridView.Size = new System.Drawing.Size(813, 564);
            this.devicesGridView.TabIndex = 9;
            this.devicesGridView.DataBindingComplete += new System.Windows.Forms.DataGridViewBindingCompleteEventHandler(this.devicesGridView_DataBindingComplete);
            // 
            // devicesGridViewContextMenu
            // 
            this.devicesGridViewContextMenu.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.devicesGridViewContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.copyAllToolStripMenuItem,
            this.copySelectedToolStripMenuItem,
            this.toolStripSeparator1,
            this.getDeviceConnectionStringToolStripMenuItem});
            this.devicesGridViewContextMenu.Name = "devicesGridViewContextMenu";
            this.devicesGridViewContextMenu.Size = new System.Drawing.Size(360, 82);
            // 
            // copyAllToolStripMenuItem
            // 
            this.copyAllToolStripMenuItem.Name = "copyAllToolStripMenuItem";
            this.copyAllToolStripMenuItem.Size = new System.Drawing.Size(359, 24);
            this.copyAllToolStripMenuItem.Text = "Copy data for all device";
            this.copyAllToolStripMenuItem.Click += new System.EventHandler(this.copyAllToolStripMenuItem_Click);
            // 
            // copySelectedToolStripMenuItem
            // 
            this.copySelectedToolStripMenuItem.Name = "copySelectedToolStripMenuItem";
            this.copySelectedToolStripMenuItem.Size = new System.Drawing.Size(359, 24);
            this.copySelectedToolStripMenuItem.Text = "Copy data for selected device";
            this.copySelectedToolStripMenuItem.Click += new System.EventHandler(this.copySelectedToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(356, 6);
            // 
            // getDeviceConnectionStringToolStripMenuItem
            // 
            this.getDeviceConnectionStringToolStripMenuItem.Name = "getDeviceConnectionStringToolStripMenuItem";
            this.getDeviceConnectionStringToolStripMenuItem.Size = new System.Drawing.Size(359, 24);
            this.getDeviceConnectionStringToolStripMenuItem.Text = "Copy connection string for selected device";
            this.getDeviceConnectionStringToolStripMenuItem.Click += new System.EventHandler(this.getDeviceConnectionStringToolStripMenuItem_Click);
            // 
            // tabData
            // 
            this.tabData.Controls.Add(this.groupBox3);
            this.tabData.Controls.Add(this.groupBox1);
            this.tabData.Location = new System.Drawing.Point(4, 29);
            this.tabData.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tabData.Name = "tabData";
            this.tabData.Padding = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tabData.Size = new System.Drawing.Size(845, 742);
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
            this.groupBox3.Location = new System.Drawing.Point(8, 30);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox3.Size = new System.Drawing.Size(827, 304);
            this.groupBox3.TabIndex = 26;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Monitoring";
            // 
            // consumerGroupCheckBox
            // 
            this.consumerGroupCheckBox.AutoSize = true;
            this.consumerGroupCheckBox.Location = new System.Drawing.Point(543, 193);
            this.consumerGroupCheckBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.consumerGroupCheckBox.Name = "consumerGroupCheckBox";
            this.consumerGroupCheckBox.Size = new System.Drawing.Size(82, 24);
            this.consumerGroupCheckBox.TabIndex = 29;
            this.consumerGroupCheckBox.Text = "Enable";
            this.consumerGroupCheckBox.UseVisualStyleBackColor = true;
            this.consumerGroupCheckBox.CheckedChanged += new System.EventHandler(this.consumerGroupCheckBox_CheckedChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(29, 198);
            this.label6.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(142, 20);
            this.label6.TabIndex = 28;
            this.label6.Text = "Consumer Group:";
            // 
            // groupNameTextBox
            // 
            this.groupNameTextBox.Enabled = false;
            this.groupNameTextBox.Location = new System.Drawing.Point(187, 191);
            this.groupNameTextBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupNameTextBox.Name = "groupNameTextBox";
            this.groupNameTextBox.Size = new System.Drawing.Size(343, 26);
            this.groupNameTextBox.TabIndex = 27;
            // 
            // cancelMonitoringButton
            // 
            this.cancelMonitoringButton.Location = new System.Drawing.Point(243, 249);
            this.cancelMonitoringButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.cancelMonitoringButton.Name = "cancelMonitoringButton";
            this.cancelMonitoringButton.Size = new System.Drawing.Size(177, 37);
            this.cancelMonitoringButton.TabIndex = 26;
            this.cancelMonitoringButton.Text = "Cancel";
            this.cancelMonitoringButton.UseVisualStyleBackColor = true;
            this.cancelMonitoringButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(29, 49);
            this.label12.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(92, 20);
            this.label12.TabIndex = 15;
            this.label12.Text = "Event Hub:";
            // 
            // clearDataButton
            // 
            this.clearDataButton.Location = new System.Drawing.Point(471, 249);
            this.clearDataButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.clearDataButton.Name = "clearDataButton";
            this.clearDataButton.Size = new System.Drawing.Size(160, 37);
            this.clearDataButton.TabIndex = 21;
            this.clearDataButton.Text = "Clear";
            this.clearDataButton.UseVisualStyleBackColor = true;
            this.clearDataButton.Click += new System.EventHandler(this.clearDataButton_Click);
            // 
            // dataMonitorButton
            // 
            this.dataMonitorButton.Location = new System.Drawing.Point(28, 249);
            this.dataMonitorButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.dataMonitorButton.Name = "dataMonitorButton";
            this.dataMonitorButton.Size = new System.Drawing.Size(160, 37);
            this.dataMonitorButton.TabIndex = 4;
            this.dataMonitorButton.Text = "Monitor";
            this.dataMonitorButton.UseVisualStyleBackColor = true;
            this.dataMonitorButton.Click += new System.EventHandler(this.dataMonitorButton_Click);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(29, 149);
            this.label15.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(92, 20);
            this.label15.TabIndex = 23;
            this.label15.Text = "Start Time:";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(29, 100);
            this.label11.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(88, 20);
            this.label11.TabIndex = 2;
            this.label11.Text = "Device ID:";
            // 
            // eventHubNameTextBoxForDataTab
            // 
            this.eventHubNameTextBoxForDataTab.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.eventHubNameTextBoxForDataTab.Location = new System.Drawing.Point(135, 42);
            this.eventHubNameTextBoxForDataTab.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.eventHubNameTextBoxForDataTab.Name = "eventHubNameTextBoxForDataTab";
            this.eventHubNameTextBoxForDataTab.ReadOnly = true;
            this.eventHubNameTextBoxForDataTab.Size = new System.Drawing.Size(669, 26);
            this.eventHubNameTextBoxForDataTab.TabIndex = 17;
            // 
            // deviceIDsComboBoxForEvent
            // 
            this.deviceIDsComboBoxForEvent.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.deviceIDsComboBoxForEvent.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.deviceIDsComboBoxForEvent.FormattingEnabled = true;
            this.deviceIDsComboBoxForEvent.Location = new System.Drawing.Point(135, 90);
            this.deviceIDsComboBoxForEvent.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.deviceIDsComboBoxForEvent.Name = "deviceIDsComboBoxForEvent";
            this.deviceIDsComboBoxForEvent.Size = new System.Drawing.Size(669, 28);
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
            this.dateTimePicker.Location = new System.Drawing.Point(135, 142);
            this.dateTimePicker.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.dateTimePicker.Name = "dateTimePicker";
            this.dateTimePicker.ShowCheckBox = true;
            this.dateTimePicker.Size = new System.Drawing.Size(669, 26);
            this.dateTimePicker.TabIndex = 22;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.eventHubTextBox);
            this.groupBox1.Location = new System.Drawing.Point(8, 359);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox1.Size = new System.Drawing.Size(827, 373);
            this.groupBox1.TabIndex = 14;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Event Hub Data";
            // 
            // eventHubTextBox
            // 
            this.eventHubTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.eventHubTextBox.Location = new System.Drawing.Point(11, 26);
            this.eventHubTextBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.eventHubTextBox.Name = "eventHubTextBox";
            this.eventHubTextBox.ReadOnly = true;
            this.eventHubTextBox.Size = new System.Drawing.Size(807, 339);
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
            this.tabMessagesToDevice.Location = new System.Drawing.Point(4, 29);
            this.tabMessagesToDevice.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tabMessagesToDevice.Name = "tabMessagesToDevice";
            this.tabMessagesToDevice.Padding = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.tabMessagesToDevice.Size = new System.Drawing.Size(845, 742);
            this.tabMessagesToDevice.TabIndex = 2;
            this.tabMessagesToDevice.Text = "Messages To Device";
            this.tabMessagesToDevice.UseVisualStyleBackColor = true;
            // 
            // checkBoxMonitorFeedbackEndpoint
            // 
            this.checkBoxMonitorFeedbackEndpoint.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.checkBoxMonitorFeedbackEndpoint.AutoSize = true;
            this.checkBoxMonitorFeedbackEndpoint.Location = new System.Drawing.Point(375, 182);
            this.checkBoxMonitorFeedbackEndpoint.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.checkBoxMonitorFeedbackEndpoint.Name = "checkBoxMonitorFeedbackEndpoint";
            this.checkBoxMonitorFeedbackEndpoint.Size = new System.Drawing.Size(234, 24);
            this.checkBoxMonitorFeedbackEndpoint.TabIndex = 12;
            this.checkBoxMonitorFeedbackEndpoint.Text = "Monitor Feedback Endpoint";
            this.checkBoxMonitorFeedbackEndpoint.UseVisualStyleBackColor = true;
            this.checkBoxMonitorFeedbackEndpoint.CheckedChanged += new System.EventHandler(this.checkBoxMonitorFeedbackEndpoint_CheckedChanged);
            // 
            // messageClearButton
            // 
            this.messageClearButton.Location = new System.Drawing.Point(273, 233);
            this.messageClearButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.messageClearButton.MaximumSize = new System.Drawing.Size(193, 37);
            this.messageClearButton.Name = "messageClearButton";
            this.messageClearButton.Size = new System.Drawing.Size(193, 37);
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
            this.deviceIDsComboBoxForCloudToDeviceMessage.Location = new System.Drawing.Point(139, 100);
            this.deviceIDsComboBoxForCloudToDeviceMessage.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.deviceIDsComboBoxForCloudToDeviceMessage.Name = "deviceIDsComboBoxForCloudToDeviceMessage";
            this.deviceIDsComboBoxForCloudToDeviceMessage.Size = new System.Drawing.Size(687, 28);
            this.deviceIDsComboBoxForCloudToDeviceMessage.TabIndex = 10;
            this.deviceIDsComboBoxForCloudToDeviceMessage.SelectionChangeCommitted += new System.EventHandler(this.deviceIDsComboBoxForMessage_SelectionChangeCommitted);
            // 
            // iotHubNameTextBox
            // 
            this.iotHubNameTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.iotHubNameTextBox.Location = new System.Drawing.Point(139, 65);
            this.iotHubNameTextBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.iotHubNameTextBox.Name = "iotHubNameTextBox";
            this.iotHubNameTextBox.ReadOnly = true;
            this.iotHubNameTextBox.Size = new System.Drawing.Size(687, 26);
            this.iotHubNameTextBox.TabIndex = 9;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(28, 69);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(73, 20);
            this.label2.TabIndex = 8;
            this.label2.Text = "IoT Hub:";
            // 
            // checkBox1
            // 
            this.checkBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(139, 182);
            this.checkBox1.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(155, 24);
            this.checkBox1.TabIndex = 5;
            this.checkBox1.Text = "Add Time Stamp";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // groupBox4
            // 
            this.groupBox4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox4.Controls.Add(this.messagesTextBox);
            this.groupBox4.Location = new System.Drawing.Point(8, 277);
            this.groupBox4.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Padding = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.groupBox4.Size = new System.Drawing.Size(829, 457);
            this.groupBox4.TabIndex = 7;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Output";
            // 
            // messagesTextBox
            // 
            this.messagesTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.messagesTextBox.Location = new System.Drawing.Point(8, 26);
            this.messagesTextBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.messagesTextBox.Name = "messagesTextBox";
            this.messagesTextBox.ReadOnly = true;
            this.messagesTextBox.Size = new System.Drawing.Size(812, 423);
            this.messagesTextBox.TabIndex = 7;
            this.messagesTextBox.Text = "";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(28, 140);
            this.label8.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(82, 20);
            this.label8.TabIndex = 3;
            this.label8.Text = "Message:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(28, 103);
            this.label7.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(88, 20);
            this.label7.TabIndex = 1;
            this.label7.Text = "Device ID:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(17, 23);
            this.label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(201, 20);
            this.label5.TabIndex = 2;
            this.label5.Text = "Send Message to Device:";
            // 
            // sendMessageToDeviceButton
            // 
            this.sendMessageToDeviceButton.Location = new System.Drawing.Point(32, 233);
            this.sendMessageToDeviceButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.sendMessageToDeviceButton.MaximumSize = new System.Drawing.Size(193, 37);
            this.sendMessageToDeviceButton.Name = "sendMessageToDeviceButton";
            this.sendMessageToDeviceButton.Size = new System.Drawing.Size(193, 37);
            this.sendMessageToDeviceButton.TabIndex = 6;
            this.sendMessageToDeviceButton.Text = "Send";
            this.sendMessageToDeviceButton.UseVisualStyleBackColor = true;
            this.sendMessageToDeviceButton.Click += new System.EventHandler(this.sendMessageToDeviceButton_Click);
            // 
            // textBoxMessage
            // 
            this.textBoxMessage.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxMessage.Location = new System.Drawing.Point(139, 137);
            this.textBoxMessage.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.textBoxMessage.Name = "textBoxMessage";
            this.textBoxMessage.Size = new System.Drawing.Size(687, 26);
            this.textBoxMessage.TabIndex = 4;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(885, 795);
            this.Controls.Add(this.tabControl1);
            this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.MinimumSize = new System.Drawing.Size(901, 832);
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
        private System.Windows.Forms.Button sasTokenButton;
    }
}

