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
            this.jsonView = new System.Windows.Forms.TabPage();
            this.jsonTableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.jsonLabel1 = new System.Windows.Forms.Label();
            this.jsonLabel2 = new System.Windows.Forms.Label();
            this.jsonRichTextBox1 = new System.Windows.Forms.RichTextBox();
            this.jsonRichTextBox2 = new System.Windows.Forms.RichTextBox();
            this.jsonLabel3 = new System.Windows.Forms.Label();
            this.sendBtn = new System.Windows.Forms.Button();
            this.jsonRichTextBox3 = new System.Windows.Forms.RichTextBox();
            this.propertyGridView = new System.Windows.Forms.TabPage();
            this.tabTableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.propLabel1 = new System.Windows.Forms.Label();
            this.propLabel2 = new System.Windows.Forms.Label();
            this.reportedPropertiesGrid = new System.Windows.Forms.PropertyGrid();
            this.desiredPropertiesGrid = new System.Windows.Forms.PropertyGrid();
            this.toolbarPanel = new System.Windows.Forms.Panel();
            this.deviceListCombo = new System.Windows.Forms.ComboBox();
            this.refreshBtn = new System.Windows.Forms.Button();
            this.mainTableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.SuspendLayout();
            this.propertyTabs.SuspendLayout();
            this.jsonView.SuspendLayout();
            this.jsonTableLayoutPanel.SuspendLayout();
            this.propertyGridView.SuspendLayout();
            this.tabTableLayoutPanel.SuspendLayout();
            this.toolbarPanel.SuspendLayout();
            this.mainTableLayoutPanel.SuspendLayout();
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
            this.propertyTabs.Controls.Add(this.jsonView);
            this.propertyTabs.Controls.Add(this.propertyGridView);
            this.propertyTabs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyTabs.Location = new System.Drawing.Point(3, 37);
            this.propertyTabs.Name = "propertyTabs";
            this.propertyTabs.SelectedIndex = 0;
            this.propertyTabs.Size = new System.Drawing.Size(863, 591);
            this.propertyTabs.TabIndex = 0;
            // 
            // jsonView
            // 
            this.jsonView.Controls.Add(this.jsonTableLayoutPanel);
            this.jsonView.Location = new System.Drawing.Point(4, 22);
            this.jsonView.Name = "jsonView";
            this.jsonView.Padding = new System.Windows.Forms.Padding(3);
            this.jsonView.Size = new System.Drawing.Size(855, 565);
            this.jsonView.TabIndex = 0;
            this.jsonView.Text = "JSon";
            this.jsonView.UseVisualStyleBackColor = true;
            // 
            // jsonTableLayoutPanel
            // 
            this.jsonTableLayoutPanel.ColumnCount = 3;
            this.jsonTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 34F));
            this.jsonTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33F));
            this.jsonTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33F));
            this.jsonTableLayoutPanel.Controls.Add(this.jsonLabel1, 0, 0);
            this.jsonTableLayoutPanel.Controls.Add(this.jsonLabel2, 1, 0);
            this.jsonTableLayoutPanel.Controls.Add(this.jsonRichTextBox1, 0, 2);
            this.jsonTableLayoutPanel.Controls.Add(this.jsonRichTextBox2, 1, 2);
            this.jsonTableLayoutPanel.Controls.Add(this.jsonLabel3, 2, 0);
            this.jsonTableLayoutPanel.Controls.Add(this.sendBtn, 2, 1);
            this.jsonTableLayoutPanel.Controls.Add(this.jsonRichTextBox3, 2, 2);
            this.jsonTableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonTableLayoutPanel.Location = new System.Drawing.Point(3, 3);
            this.jsonTableLayoutPanel.Name = "jsonTableLayoutPanel";
            this.jsonTableLayoutPanel.RowCount = 3;
            this.jsonTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.jsonTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 28F));
            this.jsonTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.jsonTableLayoutPanel.Size = new System.Drawing.Size(849, 559);
            this.jsonTableLayoutPanel.TabIndex = 0;
            // 
            // jsonLabel1
            // 
            this.jsonLabel1.AutoSize = true;
            this.jsonLabel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonLabel1.Location = new System.Drawing.Point(3, 0);
            this.jsonLabel1.Name = "jsonLabel1";
            this.jsonLabel1.Size = new System.Drawing.Size(282, 20);
            this.jsonLabel1.TabIndex = 0;
            this.jsonLabel1.Text = "Reported Properties";
            this.jsonLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // jsonLabel2
            // 
            this.jsonLabel2.AutoSize = true;
            this.jsonLabel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonLabel2.Location = new System.Drawing.Point(291, 0);
            this.jsonLabel2.Name = "jsonLabel2";
            this.jsonLabel2.Size = new System.Drawing.Size(274, 20);
            this.jsonLabel2.TabIndex = 1;
            this.jsonLabel2.Text = "DesiredProperties";
            this.jsonLabel2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // jsonRichTextBox1
            // 
            this.jsonRichTextBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonRichTextBox1.Font = new System.Drawing.Font("Courier New", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.jsonRichTextBox1.Location = new System.Drawing.Point(3, 51);
            this.jsonRichTextBox1.Name = "jsonRichTextBox1";
            this.jsonRichTextBox1.ReadOnly = true;
            this.jsonRichTextBox1.Size = new System.Drawing.Size(282, 505);
            this.jsonRichTextBox1.TabIndex = 2;
            this.jsonRichTextBox1.Text = "";
            // 
            // jsonRichTextBox2
            // 
            this.jsonRichTextBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonRichTextBox2.Font = new System.Drawing.Font("Courier New", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.jsonRichTextBox2.Location = new System.Drawing.Point(291, 51);
            this.jsonRichTextBox2.Name = "jsonRichTextBox2";
            this.jsonRichTextBox2.ReadOnly = true;
            this.jsonRichTextBox2.Size = new System.Drawing.Size(274, 505);
            this.jsonRichTextBox2.TabIndex = 3;
            this.jsonRichTextBox2.Text = "";
            // 
            // jsonLabel3
            // 
            this.jsonLabel3.AutoSize = true;
            this.jsonLabel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonLabel3.Location = new System.Drawing.Point(571, 0);
            this.jsonLabel3.Name = "jsonLabel3";
            this.jsonLabel3.Size = new System.Drawing.Size(275, 20);
            this.jsonLabel3.TabIndex = 4;
            this.jsonLabel3.Text = "Twin Editor ( use JSON format )";
            this.jsonLabel3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // sendBtn
            // 
            this.sendBtn.Dock = System.Windows.Forms.DockStyle.Top;
            this.sendBtn.Location = new System.Drawing.Point(571, 23);
            this.sendBtn.Name = "sendBtn";
            this.sendBtn.Size = new System.Drawing.Size(275, 22);
            this.sendBtn.TabIndex = 5;
            this.sendBtn.Text = "Send";
            this.sendBtn.UseVisualStyleBackColor = true;
            this.sendBtn.Click += new System.EventHandler(this.sendBtn_Click);
            // 
            // jsonRichTextBox3
            // 
            this.jsonRichTextBox3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.jsonRichTextBox3.Font = new System.Drawing.Font("Courier New", 12F);
            this.jsonRichTextBox3.Location = new System.Drawing.Point(571, 51);
            this.jsonRichTextBox3.Name = "jsonRichTextBox3";
            this.jsonRichTextBox3.Size = new System.Drawing.Size(275, 505);
            this.jsonRichTextBox3.TabIndex = 6;
            this.jsonRichTextBox3.Text = "";
            // 
            // propertyGridView
            // 
            this.propertyGridView.Controls.Add(this.tabTableLayoutPanel);
            this.propertyGridView.Location = new System.Drawing.Point(4, 22);
            this.propertyGridView.Name = "propertyGridView";
            this.propertyGridView.Padding = new System.Windows.Forms.Padding(3);
            this.propertyGridView.Size = new System.Drawing.Size(855, 565);
            this.propertyGridView.TabIndex = 1;
            this.propertyGridView.Text = "Properties";
            this.propertyGridView.UseVisualStyleBackColor = true;
            // 
            // tabTableLayoutPanel
            // 
            this.tabTableLayoutPanel.ColumnCount = 2;
            this.tabTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tabTableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tabTableLayoutPanel.Controls.Add(this.propLabel1, 0, 0);
            this.tabTableLayoutPanel.Controls.Add(this.propLabel2, 1, 0);
            this.tabTableLayoutPanel.Controls.Add(this.reportedPropertiesGrid, 0, 1);
            this.tabTableLayoutPanel.Controls.Add(this.desiredPropertiesGrid, 1, 1);
            this.tabTableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabTableLayoutPanel.Location = new System.Drawing.Point(3, 3);
            this.tabTableLayoutPanel.Name = "tabTableLayoutPanel";
            this.tabTableLayoutPanel.RowCount = 2;
            this.tabTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tabTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tabTableLayoutPanel.Size = new System.Drawing.Size(849, 559);
            this.tabTableLayoutPanel.TabIndex = 0;
            // 
            // propLabel1
            // 
            this.propLabel1.AutoSize = true;
            this.propLabel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propLabel1.Location = new System.Drawing.Point(3, 0);
            this.propLabel1.Name = "propLabel1";
            this.propLabel1.Size = new System.Drawing.Size(418, 20);
            this.propLabel1.TabIndex = 1;
            this.propLabel1.Text = "Reported Properties";
            this.propLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // propLabel2
            // 
            this.propLabel2.AutoSize = true;
            this.propLabel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propLabel2.Location = new System.Drawing.Point(427, 0);
            this.propLabel2.Name = "propLabel2";
            this.propLabel2.Size = new System.Drawing.Size(419, 20);
            this.propLabel2.TabIndex = 2;
            this.propLabel2.Text = "Desired Properties";
            this.propLabel2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // reportedPropertiesGrid
            // 
            this.reportedPropertiesGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.reportedPropertiesGrid.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.reportedPropertiesGrid.Location = new System.Drawing.Point(3, 23);
            this.reportedPropertiesGrid.Name = "reportedPropertiesGrid";
            this.reportedPropertiesGrid.Size = new System.Drawing.Size(418, 573);
            this.reportedPropertiesGrid.TabIndex = 3;
            // 
            // desiredPropertiesGrid
            // 
            this.desiredPropertiesGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.desiredPropertiesGrid.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.desiredPropertiesGrid.Location = new System.Drawing.Point(427, 23);
            this.desiredPropertiesGrid.Name = "desiredPropertiesGrid";
            this.desiredPropertiesGrid.Size = new System.Drawing.Size(419, 573);
            this.desiredPropertiesGrid.TabIndex = 4;
            // 
            // toolbarPanel
            // 
            this.toolbarPanel.Controls.Add(this.deviceListCombo);
            this.toolbarPanel.Controls.Add(this.refreshBtn);
            this.toolbarPanel.Location = new System.Drawing.Point(3, 3);
            this.toolbarPanel.Name = "toolbarPanel";
            this.toolbarPanel.Size = new System.Drawing.Size(863, 28);
            this.toolbarPanel.TabIndex = 1;
            // 
            // deviceListCombo
            // 
            this.deviceListCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.deviceListCombo.FormattingEnabled = true;
            this.deviceListCombo.Location = new System.Drawing.Point(82, 1);
            this.deviceListCombo.Name = "deviceListCombo";
            this.deviceListCombo.Size = new System.Drawing.Size(308, 21);
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
            this.mainTableLayoutPanel.Controls.Add(this.toolbarPanel, 0, 0);
            this.mainTableLayoutPanel.Controls.Add(this.propertyTabs, 0, 1);
            this.mainTableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainTableLayoutPanel.Location = new System.Drawing.Point(0, 0);
            this.mainTableLayoutPanel.Name = "mainTableLayoutPanel";
            this.mainTableLayoutPanel.RowCount = 2;
            this.mainTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 34F));
            this.mainTableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.mainTableLayoutPanel.Size = new System.Drawing.Size(869, 631);
            this.mainTableLayoutPanel.TabIndex = 2;
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
            // DeviceTwinPropertiesForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(869, 631);
            this.Controls.Add(this.mainTableLayoutPanel);
            this.Name = "DeviceTwinPropertiesForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Device Twin Properties";
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.propertyTabs.ResumeLayout(false);
            this.jsonView.ResumeLayout(false);
            this.jsonTableLayoutPanel.ResumeLayout(false);
            this.jsonTableLayoutPanel.PerformLayout();
            this.propertyGridView.ResumeLayout(false);
            this.tabTableLayoutPanel.ResumeLayout(false);
            this.tabTableLayoutPanel.PerformLayout();
            this.toolbarPanel.ResumeLayout(false);
            this.mainTableLayoutPanel.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TabControl propertyTabs;
        private System.Windows.Forms.TabPage jsonView;
        private System.Windows.Forms.TableLayoutPanel jsonTableLayoutPanel;
        private System.Windows.Forms.Label jsonLabel1;
        private System.Windows.Forms.Label jsonLabel2;
        private System.Windows.Forms.TabPage propertyGridView;
        private System.Windows.Forms.RichTextBox jsonRichTextBox1;
        private System.Windows.Forms.RichTextBox jsonRichTextBox2;
        private System.Windows.Forms.TableLayoutPanel tabTableLayoutPanel;
        private System.Windows.Forms.Label propLabel1;
        private System.Windows.Forms.Label propLabel2;
        private System.Windows.Forms.PropertyGrid reportedPropertiesGrid;
        private System.Windows.Forms.PropertyGrid desiredPropertiesGrid;
        private System.Windows.Forms.Panel toolbarPanel;
        private System.Windows.Forms.Button refreshBtn;
        private System.Windows.Forms.TableLayoutPanel mainTableLayoutPanel;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.ComboBox deviceListCombo;
        private System.Windows.Forms.Label jsonLabel3;
        private System.Windows.Forms.Button sendBtn;
        private System.Windows.Forms.RichTextBox jsonRichTextBox3;
    }
}