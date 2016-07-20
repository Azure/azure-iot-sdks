namespace DeviceExplorer
{
    partial class CreateDeviceForm
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
            this.createButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.deviceIDTextBox = new System.Windows.Forms.TextBox();
            this.generateIDCheckBox = new System.Windows.Forms.CheckBox();
            this.primaryKeyTextBox = new System.Windows.Forms.TextBox();
            this.generateKeysCheckBox = new System.Windows.Forms.CheckBox();
            this.secondaryKeyTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.primaryLabel = new System.Windows.Forms.Label();
            this.secondaryLabel = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.x509RadioButton = new System.Windows.Forms.RadioButton();
            this.keysRadioButton = new System.Windows.Forms.RadioButton();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // createButton
            // 
            this.createButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.createButton.Location = new System.Drawing.Point(101, 278);
            this.createButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.createButton.Name = "createButton";
            this.createButton.Size = new System.Drawing.Size(189, 33);
            this.createButton.TabIndex = 2;
            this.createButton.Text = "Create";
            this.createButton.UseVisualStyleBackColor = true;
            this.createButton.Click += new System.EventHandler(this.createButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cancelButton.Location = new System.Drawing.Point(356, 278);
            this.cancelButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(189, 33);
            this.cancelButton.TabIndex = 3;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // deviceIDTextBox
            // 
            this.deviceIDTextBox.Location = new System.Drawing.Point(183, 93);
            this.deviceIDTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.deviceIDTextBox.Name = "deviceIDTextBox";
            this.deviceIDTextBox.Size = new System.Drawing.Size(457, 22);
            this.deviceIDTextBox.TabIndex = 4;
            // 
            // generateIDCheckBox
            // 
            this.generateIDCheckBox.AutoSize = true;
            this.generateIDCheckBox.Location = new System.Drawing.Point(183, 217);
            this.generateIDCheckBox.Margin = new System.Windows.Forms.Padding(4);
            this.generateIDCheckBox.Name = "generateIDCheckBox";
            this.generateIDCheckBox.Size = new System.Drawing.Size(140, 21);
            this.generateIDCheckBox.TabIndex = 5;
            this.generateIDCheckBox.Text = "Auto Generate ID";
            this.generateIDCheckBox.UseVisualStyleBackColor = true;
            this.generateIDCheckBox.CheckedChanged += new System.EventHandler(this.generateIDCheckBox_CheckedChanged);
            // 
            // primaryKeyTextBox
            // 
            this.primaryKeyTextBox.Location = new System.Drawing.Point(183, 130);
            this.primaryKeyTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.primaryKeyTextBox.Name = "primaryKeyTextBox";
            this.primaryKeyTextBox.Size = new System.Drawing.Size(457, 22);
            this.primaryKeyTextBox.TabIndex = 6;
            // 
            // generateKeysCheckBox
            // 
            this.generateKeysCheckBox.AutoSize = true;
            this.generateKeysCheckBox.Location = new System.Drawing.Point(482, 217);
            this.generateKeysCheckBox.Margin = new System.Windows.Forms.Padding(4);
            this.generateKeysCheckBox.Name = "generateKeysCheckBox";
            this.generateKeysCheckBox.Size = new System.Drawing.Size(158, 21);
            this.generateKeysCheckBox.TabIndex = 7;
            this.generateKeysCheckBox.Text = "Auto Generate Keys";
            this.generateKeysCheckBox.UseVisualStyleBackColor = true;
            this.generateKeysCheckBox.CheckedChanged += new System.EventHandler(this.generateKeysCheckBox_CheckedChanged);
            // 
            // secondaryKeyTextBox
            // 
            this.secondaryKeyTextBox.Location = new System.Drawing.Point(183, 164);
            this.secondaryKeyTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.secondaryKeyTextBox.Name = "secondaryKeyTextBox";
            this.secondaryKeyTextBox.Size = new System.Drawing.Size(457, 22);
            this.secondaryKeyTextBox.TabIndex = 8;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 98);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 17);
            this.label1.TabIndex = 9;
            this.label1.Text = "Device ID:";
            // 
            // primaryLabel
            // 
            this.primaryLabel.AutoSize = true;
            this.primaryLabel.Location = new System.Drawing.Point(13, 135);
            this.primaryLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.primaryLabel.Name = "primaryLabel";
            this.primaryLabel.Size = new System.Drawing.Size(88, 17);
            this.primaryLabel.TabIndex = 10;
            this.primaryLabel.Text = "Primary Key:";
            // 
            // secondaryLabel
            // 
            this.secondaryLabel.AutoSize = true;
            this.secondaryLabel.Location = new System.Drawing.Point(13, 169);
            this.secondaryLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.secondaryLabel.Name = "secondaryLabel";
            this.secondaryLabel.Size = new System.Drawing.Size(108, 17);
            this.secondaryLabel.TabIndex = 11;
            this.secondaryLabel.Text = "Secondary Key:";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.x509RadioButton);
            this.groupBox1.Controls.Add(this.keysRadioButton);
            this.groupBox1.Location = new System.Drawing.Point(164, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(341, 58);
            this.groupBox1.TabIndex = 12;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Device Authentication";
            // 
            // x509RadioButton
            // 
            this.x509RadioButton.AutoSize = true;
            this.x509RadioButton.Location = new System.Drawing.Point(219, 26);
            this.x509RadioButton.Name = "x509RadioButton";
            this.x509RadioButton.Size = new System.Drawing.Size(62, 21);
            this.x509RadioButton.TabIndex = 1;
            this.x509RadioButton.TabStop = true;
            this.x509RadioButton.Text = "X509";
            this.x509RadioButton.UseVisualStyleBackColor = true;
            // 
            // keysRadioButton
            // 
            this.keysRadioButton.AutoSize = true;
            this.keysRadioButton.Location = new System.Drawing.Point(74, 26);
            this.keysRadioButton.Name = "keysRadioButton";
            this.keysRadioButton.Size = new System.Drawing.Size(115, 21);
            this.keysRadioButton.TabIndex = 0;
            this.keysRadioButton.TabStop = true;
            this.keysRadioButton.Text = "Security Keys";
            this.keysRadioButton.UseVisualStyleBackColor = true;
            this.keysRadioButton.CheckedChanged += new System.EventHandler(this.keysRadioButton_CheckedChanged);
            // 
            // CreateDeviceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoValidate = System.Windows.Forms.AutoValidate.EnablePreventFocusChange;
            this.ClientSize = new System.Drawing.Size(705, 336);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.secondaryLabel);
            this.Controls.Add(this.primaryLabel);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.secondaryKeyTextBox);
            this.Controls.Add(this.generateKeysCheckBox);
            this.Controls.Add(this.primaryKeyTextBox);
            this.Controls.Add(this.generateIDCheckBox);
            this.Controls.Add(this.deviceIDTextBox);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.createButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "CreateDeviceForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Create Device";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button createButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.TextBox deviceIDTextBox;
        private System.Windows.Forms.CheckBox generateIDCheckBox;
        private System.Windows.Forms.TextBox primaryKeyTextBox;
        private System.Windows.Forms.CheckBox generateKeysCheckBox;
        private System.Windows.Forms.TextBox secondaryKeyTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label primaryLabel;
        private System.Windows.Forms.Label secondaryLabel;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton x509RadioButton;
        private System.Windows.Forms.RadioButton keysRadioButton;
    }
}