namespace DeviceExplorer
{
    partial class DeviceUpdateForm
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
            this.deviceIDLabel = new System.Windows.Forms.Label();
            this.primaryLabel = new System.Windows.Forms.Label();
            this.secondaryLabel = new System.Windows.Forms.Label();
            this.primaryKeyTextBox = new System.Windows.Forms.TextBox();
            this.secondaryKeyTextBox = new System.Windows.Forms.TextBox();
            this.updateButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.deviceIDComboBox = new System.Windows.Forms.ComboBox();
            this.restoreButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // deviceIDLabel
            // 
            this.deviceIDLabel.AutoSize = true;
            this.deviceIDLabel.Location = new System.Drawing.Point(12, 35);
            this.deviceIDLabel.Name = "deviceIDLabel";
            this.deviceIDLabel.Size = new System.Drawing.Size(64, 17);
            this.deviceIDLabel.TabIndex = 0;
            this.deviceIDLabel.Text = "DeviceID";
            // 
            // primaryLabel
            // 
            this.primaryLabel.AutoSize = true;
            this.primaryLabel.Location = new System.Drawing.Point(12, 75);
            this.primaryLabel.Name = "primaryLabel";
            this.primaryLabel.Size = new System.Drawing.Size(84, 17);
            this.primaryLabel.TabIndex = 2;
            this.primaryLabel.Text = "Primary Key";
            // 
            // secondaryLabel
            // 
            this.secondaryLabel.AutoSize = true;
            this.secondaryLabel.Location = new System.Drawing.Point(12, 119);
            this.secondaryLabel.Name = "secondaryLabel";
            this.secondaryLabel.Size = new System.Drawing.Size(104, 17);
            this.secondaryLabel.TabIndex = 3;
            this.secondaryLabel.Text = "Secondary Key";
            // 
            // primaryKeyTextBox
            // 
            this.primaryKeyTextBox.Location = new System.Drawing.Point(186, 70);
            this.primaryKeyTextBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.primaryKeyTextBox.Name = "primaryKeyTextBox";
            this.primaryKeyTextBox.Size = new System.Drawing.Size(404, 22);
            this.primaryKeyTextBox.TabIndex = 5;
            // 
            // secondaryKeyTextBox
            // 
            this.secondaryKeyTextBox.Location = new System.Drawing.Point(186, 114);
            this.secondaryKeyTextBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.secondaryKeyTextBox.Name = "secondaryKeyTextBox";
            this.secondaryKeyTextBox.Size = new System.Drawing.Size(404, 22);
            this.secondaryKeyTextBox.TabIndex = 6;
            // 
            // updateButton
            // 
            this.updateButton.Location = new System.Drawing.Point(86, 175);
            this.updateButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.updateButton.Name = "updateButton";
            this.updateButton.Size = new System.Drawing.Size(104, 32);
            this.updateButton.TabIndex = 7;
            this.updateButton.Text = "Update";
            this.updateButton.UseVisualStyleBackColor = true;
            this.updateButton.Click += new System.EventHandler(this.updateButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.Location = new System.Drawing.Point(410, 175);
            this.cancelButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(104, 32);
            this.cancelButton.TabIndex = 8;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // deviceIDComboBox
            // 
            this.deviceIDComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.deviceIDComboBox.FormattingEnabled = true;
            this.deviceIDComboBox.Location = new System.Drawing.Point(189, 28);
            this.deviceIDComboBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.deviceIDComboBox.Name = "deviceIDComboBox";
            this.deviceIDComboBox.Size = new System.Drawing.Size(401, 24);
            this.deviceIDComboBox.TabIndex = 9;
            this.deviceIDComboBox.DropDownClosed += new System.EventHandler(this.deviceIDComboBox_DropDownClosed);
            // 
            // restoreButton
            // 
            this.restoreButton.Location = new System.Drawing.Point(239, 175);
            this.restoreButton.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.restoreButton.Name = "restoreButton";
            this.restoreButton.Size = new System.Drawing.Size(116, 32);
            this.restoreButton.TabIndex = 10;
            this.restoreButton.Text = "Restore";
            this.restoreButton.UseVisualStyleBackColor = true;
            this.restoreButton.Click += new System.EventHandler(this.restoreButton_Click);
            // 
            // DeviceUpdateForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(603, 242);
            this.Controls.Add(this.restoreButton);
            this.Controls.Add(this.deviceIDComboBox);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.updateButton);
            this.Controls.Add(this.secondaryKeyTextBox);
            this.Controls.Add(this.primaryKeyTextBox);
            this.Controls.Add(this.secondaryLabel);
            this.Controls.Add(this.primaryLabel);
            this.Controls.Add(this.deviceIDLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "DeviceUpdateForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "DeviceUpdateForm";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.DeviceUpdateForm_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label deviceIDLabel;
        private System.Windows.Forms.Label primaryLabel;
        private System.Windows.Forms.Label secondaryLabel;
        private System.Windows.Forms.TextBox primaryKeyTextBox;
        private System.Windows.Forms.TextBox secondaryKeyTextBox;
        private System.Windows.Forms.Button updateButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.ComboBox deviceIDComboBox;
        private System.Windows.Forms.Button restoreButton;
    }
}