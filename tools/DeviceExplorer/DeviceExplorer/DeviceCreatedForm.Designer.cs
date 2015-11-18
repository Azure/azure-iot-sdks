namespace DeviceExplorer
{
    partial class DeviceCreatedForm
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
            this.doneButton = new System.Windows.Forms.Button();
            this.richTextBox = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // doneButton
            // 
            this.doneButton.Location = new System.Drawing.Point(116, 104);
            this.doneButton.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.doneButton.Name = "doneButton";
            this.doneButton.Size = new System.Drawing.Size(141, 30);
            this.doneButton.TabIndex = 0;
            this.doneButton.Text = "Done";
            this.doneButton.UseVisualStyleBackColor = true;
            this.doneButton.Click += new System.EventHandler(this.doneButton_Click);
            // 
            // richTextBox
            // 
            this.richTextBox.Location = new System.Drawing.Point(10, 11);
            this.richTextBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.richTextBox.Name = "richTextBox";
            this.richTextBox.ReadOnly = true;
            this.richTextBox.Size = new System.Drawing.Size(390, 80);
            this.richTextBox.TabIndex = 1;
            this.richTextBox.Text = "";
            // 
            // DeviceCreatedForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(408, 144);
            this.Controls.Add(this.richTextBox);
            this.Controls.Add(this.doneButton);
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.Name = "DeviceCreatedForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Device Created";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button doneButton;
        private System.Windows.Forms.RichTextBox richTextBox;
    }
}