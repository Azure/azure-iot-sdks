namespace DeviceExplorer
{
    partial class DeviceTwinPropertiesForm
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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.propertyTabs = new System.Windows.Forms.TabControl();
            this.tabTwin = new System.Windows.Forms.TabPage();
            this.jsonRichTextBox0 = new System.Windows.Forms.RichTextBox();
            this.tabTags = new System.Windows.Forms.TabPage();
            this.jsonRichTextBox1 = new System.Windows.Forms.RichTextBox();
            this.tabReportedProperties = new System.Windows.Forms.TabPage();
            this.jsonRichTextBox2 = new System.Windows.Forms.RichTextBox();
            this.tabDesiredProperties = new System.Windows.Forms.TabPage();
            this.jsonRichTextBox3 = new System.Windows.Forms.RichTextBox();
            this.toolbarPanel = new System.Windows.Forms.Panel();
            this.deviceListCombo = new System.Windows.Forms.ComboBox();
            this.refreshBtn = new System.Windows.Forms.Button();
            this.mainTableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.sendBtn = new System.Windows.Forms.Button();
            this.jsonEditRichTextBox = new System.Windows.Forms.RichTextBox();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.SuspendLayout();
            this.propertyTabs.SuspendLayout();
            this.tabTwin.SuspendLayout();
            this.tabTags.SuspendLayout();
            this.tabReportedProperties.SuspendLayout();
            this.tabDesiredProperties.SuspendLayout();
            this.toolbarPanel.SuspendLayout();
            this.mainTableLayoutPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
            this.splitContainer3.Panel1.SuspendLayout();
            this.splitContainer3.Panel2.SuspendLayout();
            this.splitContainer3.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.IsSplitterFixed = true;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Size = new System.Drawing.Size(600, 100);
            this.splitContainer1.SplitterDistance = 200;
            this.splitContainer1.TabIndex = 0;
            // 
            // propertyTabs
            // 
            this.propertyTabs.Controls.Add(this.tabTwin);
            this.propertyTabs.Controls.Add(this.tabTags);
            this.propertyTabs.Controls.Add(this.tabReportedProperties);
            this.propertyTabs.Controls.Add(this.tabDesiredProperties);
            this.propertyTabs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyTabs.Location = new System.Drawing.Point(3, 37);
            this.propertyTabs.Name = "propertyTabs";
            this.propertyTabs.SelectedIndex = 0;
            this.propertyTabs.Size = new System.Drawing.Size(594, 621);
            this.propertyTabs.TabIndex = 0;
            // 
            // tabTwin
            // 
            this.tabTwin.Controls.Add(this.jsonRichTextBox0);
            this.tabTwin.Location = new System.Drawing.Point(4, 22);
            this.tabTwin.Name = "tabTwin";
            this.tabTwin.Padding = new System.Windows.Forms.Padding(3);
            this.tabTwin.Size = new System.Drawing.Size(688, 565);
            this.tabTwin.TabIndex = 0;
            this.tabTwin.Text = "Entire Twin";
            this.tabTwin.UseVisualStyleBackColor = true;
            // 
            // jsonRichTextBox0
            // 
            this.jsonRichTextBox0.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonRichTextBox0.Font = new System.Drawing.Font("Courier New", 12F);
            this.jsonRichTextBox0.Location = new System.Drawing.Point(3, 3);
            this.jsonRichTextBox0.Name = "jsonRichTextBox0";
            this.jsonRichTextBox0.ReadOnly = true;
            this.jsonRichTextBox0.Size = new System.Drawing.Size(682, 559);
            this.jsonRichTextBox0.TabIndex = 0;
            this.jsonRichTextBox0.Text = "";
            // 
            // tabTags
            // 
            this.tabTags.Controls.Add(this.jsonRichTextBox1);
            this.tabTags.Location = new System.Drawing.Point(4, 22);
            this.tabTags.Name = "tabTags";
            this.tabTags.Padding = new System.Windows.Forms.Padding(3);
            this.tabTags.Size = new System.Drawing.Size(688, 565);
            this.tabTags.TabIndex = 1;
            this.tabTags.Text = "Tags";
            this.tabTags.UseVisualStyleBackColor = true;
            // 
            // jsonRichTextBox1
            // 
            this.jsonRichTextBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonRichTextBox1.Font = new System.Drawing.Font("Courier New", 12F);
            this.jsonRichTextBox1.Location = new System.Drawing.Point(3, 3);
            this.jsonRichTextBox1.Name = "jsonRichTextBox1";
            this.jsonRichTextBox1.ReadOnly = true;
            this.jsonRichTextBox1.Size = new System.Drawing.Size(682, 559);
            this.jsonRichTextBox1.TabIndex = 0;
            this.jsonRichTextBox1.Text = "";
            // 
            // tabReportedProperties
            // 
            this.tabReportedProperties.Controls.Add(this.jsonRichTextBox2);
            this.tabReportedProperties.Location = new System.Drawing.Point(4, 22);
            this.tabReportedProperties.Name = "tabReportedProperties";
            this.tabReportedProperties.Padding = new System.Windows.Forms.Padding(3);
            this.tabReportedProperties.Size = new System.Drawing.Size(688, 565);
            this.tabReportedProperties.TabIndex = 2;
            this.tabReportedProperties.Text = "Reported Properties";
            this.tabReportedProperties.UseVisualStyleBackColor = true;
            // 
            // jsonRichTextBox2
            // 
            this.jsonRichTextBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonRichTextBox2.Font = new System.Drawing.Font("Courier New", 12F);
            this.jsonRichTextBox2.Location = new System.Drawing.Point(3, 3);
            this.jsonRichTextBox2.Name = "jsonRichTextBox2";
            this.jsonRichTextBox2.ReadOnly = true;
            this.jsonRichTextBox2.Size = new System.Drawing.Size(682, 559);
            this.jsonRichTextBox2.TabIndex = 0;
            this.jsonRichTextBox2.Text = "";
            // 
            // tabDesiredProperties
            // 
            this.tabDesiredProperties.Controls.Add(this.jsonRichTextBox3);
            this.tabDesiredProperties.Location = new System.Drawing.Point(4, 22);
            this.tabDesiredProperties.Name = "tabDesiredProperties";
            this.tabDesiredProperties.Padding = new System.Windows.Forms.Padding(3);
            this.tabDesiredProperties.Size = new System.Drawing.Size(586, 595);
            this.tabDesiredProperties.TabIndex = 3;
            this.tabDesiredProperties.Text = "Desired Properties";
            this.tabDesiredProperties.UseVisualStyleBackColor = true;
            // 
            // jsonRichTextBox3
            // 
            this.jsonRichTextBox3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonRichTextBox3.Font = new System.Drawing.Font("Courier New", 12F);
            this.jsonRichTextBox3.Location = new System.Drawing.Point(3, 3);
            this.jsonRichTextBox3.Name = "jsonRichTextBox3";
            this.jsonRichTextBox3.ReadOnly = true;
            this.jsonRichTextBox3.Size = new System.Drawing.Size(580, 589);
            this.jsonRichTextBox3.TabIndex = 0;
            this.jsonRichTextBox3.Text = "";
            // 
            // toolbarPanel
            // 
            this.toolbarPanel.Controls.Add(this.deviceListCombo);
            this.toolbarPanel.Controls.Add(this.refreshBtn);
            this.toolbarPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.toolbarPanel.Location = new System.Drawing.Point(3, 3);
            this.toolbarPanel.Name = "toolbarPanel";
            this.toolbarPanel.Size = new System.Drawing.Size(594, 28);
            this.toolbarPanel.TabIndex = 1;
            // 
            // deviceListCombo
            // 
            this.deviceListCombo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.deviceListCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.deviceListCombo.FormattingEnabled = true;
            this.deviceListCombo.Location = new System.Drawing.Point(82, 1);
            this.deviceListCombo.Name = "deviceListCombo";
            this.deviceListCombo.Size = new System.Drawing.Size(508, 21);
            this.deviceListCombo.TabIndex = 1;
            this.deviceListCombo.SelectedIndexChanged += new System.EventHandler(this.deviceListCombo_SelectedIndexChanged);
            // 
            // refreshBtn
            // 
            this.refreshBtn.Location = new System.Drawing.Point(1, 1);
            this.refreshBtn.Name = "refreshBtn";
            this.refreshBtn.Size = new System.Drawing.Size(75, 23);
            this.refreshBtn.TabIndex = 0;
            this.refreshBtn.Text = "Refresh";
            this.refreshBtn.UseVisualStyleBackColor = true;
            this.refreshBtn.Click += new System.EventHandler(this.refreshBtn_Click);
            // 
            // mainTableLayoutPanel
            // 
            this.mainTableLayoutPanel.ColumnCount = 1;
            this.mainTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.mainTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.mainTableLayoutPanel.Controls.Add(this.toolbarPanel, 0, 0);
            this.mainTableLayoutPanel.Controls.Add(this.propertyTabs, 0, 1);
            this.mainTableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainTableLayoutPanel.Location = new System.Drawing.Point(0, 0);
            this.mainTableLayoutPanel.Name = "mainTableLayoutPanel";
            this.mainTableLayoutPanel.RowCount = 2;
            this.mainTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 34F));
            this.mainTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.mainTableLayoutPanel.Size = new System.Drawing.Size(600, 661);
            this.mainTableLayoutPanel.TabIndex = 2;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.sendBtn);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.jsonEditRichTextBox);
            this.splitContainer2.Size = new System.Drawing.Size(276, 661);
            this.splitContainer2.SplitterDistance = 27;
            this.splitContainer2.TabIndex = 4;
            // 
            // sendBtn
            // 
            this.sendBtn.Dock = System.Windows.Forms.DockStyle.Top;
            this.sendBtn.Location = new System.Drawing.Point(0, 0);
            this.sendBtn.Name = "sendBtn";
            this.sendBtn.Size = new System.Drawing.Size(276, 30);
            this.sendBtn.TabIndex = 0;
            this.sendBtn.Text = "Send ( use Json format )";
            this.sendBtn.UseVisualStyleBackColor = true;
            this.sendBtn.Click += new System.EventHandler(this.sendBtn_Click);
            // 
            // jsonEditRichTextBox
            // 
            this.jsonEditRichTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonEditRichTextBox.Font = new System.Drawing.Font("Courier New", 12F);
            this.jsonEditRichTextBox.Location = new System.Drawing.Point(0, 0);
            this.jsonEditRichTextBox.Name = "jsonEditRichTextBox";
            this.jsonEditRichTextBox.Size = new System.Drawing.Size(276, 630);
            this.jsonEditRichTextBox.TabIndex = 0;
            this.jsonEditRichTextBox.Text = "";
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(3, 3);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(80, 17);
            this.checkBox1.TabIndex = 0;
            this.checkBox1.Text = "checkBox1";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // splitContainer3
            // 
            this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer3.Location = new System.Drawing.Point(0, 0);
            this.splitContainer3.Name = "splitContainer3";
            // 
            // splitContainer3.Panel1
            // 
            this.splitContainer3.Panel1.Controls.Add(this.mainTableLayoutPanel);
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.splitContainer2);
            this.splitContainer3.Size = new System.Drawing.Size(884, 661);
            this.splitContainer3.SplitterDistance = 600;
            this.splitContainer3.SplitterWidth = 8;
            this.splitContainer3.TabIndex = 3;
            // 
            // DeviceTwinPropertiesForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(884, 661);
            this.Controls.Add(this.splitContainer3);
            this.Name = "DeviceTwinPropertiesForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Device Twin";
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.propertyTabs.ResumeLayout(false);
            this.tabTwin.ResumeLayout(false);
            this.tabTags.ResumeLayout(false);
            this.tabReportedProperties.ResumeLayout(false);
            this.tabDesiredProperties.ResumeLayout(false);
            this.toolbarPanel.ResumeLayout(false);
            this.mainTableLayoutPanel.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.splitContainer3.Panel1.ResumeLayout(false);
            this.splitContainer3.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
            this.splitContainer3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TabControl propertyTabs;
        private System.Windows.Forms.TabPage tabTwin;
        private System.Windows.Forms.Panel toolbarPanel;
        private System.Windows.Forms.Button refreshBtn;
        private System.Windows.Forms.TableLayoutPanel mainTableLayoutPanel;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.ComboBox deviceListCombo;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.Button sendBtn;
        private System.Windows.Forms.RichTextBox jsonEditRichTextBox;
        private System.Windows.Forms.TabPage tabTags;
        private System.Windows.Forms.TabPage tabReportedProperties;
        private System.Windows.Forms.TabPage tabDesiredProperties;
        private System.Windows.Forms.RichTextBox jsonRichTextBox0;
        private System.Windows.Forms.RichTextBox jsonRichTextBox1;
        private System.Windows.Forms.RichTextBox jsonRichTextBox2;
        private System.Windows.Forms.RichTextBox jsonRichTextBox3;
        private System.Windows.Forms.SplitContainer splitContainer3;
    }
}