namespace ThingM.Blink1.ColorManager
{
    using System.ComponentModel;

    partial class PresetControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.labelDisplayMillisecoond = new System.Windows.Forms.Label();
            this.textBoxDisplayMilisecond = new System.Windows.Forms.TextBox();
            this.labelRed = new System.Windows.Forms.Label();
            this.labelGreen = new System.Windows.Forms.Label();
            this.labelBlue = new System.Windows.Forms.Label();
            this.panelSwatch = new System.Windows.Forms.Panel();
            this.buttonUseColor = new System.Windows.Forms.Button();
            this.groupBoxPreset = new System.Windows.Forms.GroupBox();
            this.numericUpDownGreen = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownBlue = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownRed = new System.Windows.Forms.NumericUpDown();
            this.groupBoxPreset.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownGreen)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownBlue)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownRed)).BeginInit();
            this.SuspendLayout();
            // 
            // labelDisplayMillisecoond
            // 
            this.labelDisplayMillisecoond.AutoSize = true;
            this.labelDisplayMillisecoond.Location = new System.Drawing.Point(4, 15);
            this.labelDisplayMillisecoond.Name = "labelDisplayMillisecoond";
            this.labelDisplayMillisecoond.Size = new System.Drawing.Size(104, 13);
            this.labelDisplayMillisecoond.TabIndex = 2;
            this.labelDisplayMillisecoond.Text = "Display Milliseconds:";
            // 
            // textBoxDisplayMilisecond
            // 
            this.textBoxDisplayMilisecond.Location = new System.Drawing.Point(114, 12);
            this.textBoxDisplayMilisecond.Name = "textBoxDisplayMilisecond";
            this.textBoxDisplayMilisecond.Size = new System.Drawing.Size(43, 20);
            this.textBoxDisplayMilisecond.TabIndex = 3;
            this.textBoxDisplayMilisecond.Text = "1000";
            this.textBoxDisplayMilisecond.Validating += new CancelEventHandler(textBoxDisplayMilisecond_Validating);
            // 
            // labelRed
            // 
            this.labelRed.AutoSize = true;
            this.labelRed.Location = new System.Drawing.Point(166, 15);
            this.labelRed.Name = "labelRed";
            this.labelRed.Size = new System.Drawing.Size(30, 13);
            this.labelRed.TabIndex = 4;
            this.labelRed.Text = "Red:";
            // 
            // labelGreen
            // 
            this.labelGreen.AutoSize = true;
            this.labelGreen.Location = new System.Drawing.Point(252, 15);
            this.labelGreen.Name = "labelGreen";
            this.labelGreen.Size = new System.Drawing.Size(39, 13);
            this.labelGreen.TabIndex = 6;
            this.labelGreen.Text = "Green:";
            // 
            // labelBlue
            // 
            this.labelBlue.AutoSize = true;
            this.labelBlue.Location = new System.Drawing.Point(347, 15);
            this.labelBlue.Name = "labelBlue";
            this.labelBlue.Size = new System.Drawing.Size(31, 13);
            this.labelBlue.TabIndex = 8;
            this.labelBlue.Text = "Blue:";
            // 
            // panelSwatch
            // 
            this.panelSwatch.BackColor = System.Drawing.Color.Black;
            this.panelSwatch.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panelSwatch.Location = new System.Drawing.Point(447, 12);
            this.panelSwatch.Name = "panelSwatch";
            this.panelSwatch.Size = new System.Drawing.Size(20, 20);
            this.panelSwatch.TabIndex = 10;
            // 
            // buttonUseColor
            // 
            this.buttonUseColor.Location = new System.Drawing.Point(483, 10);
            this.buttonUseColor.Name = "buttonUseColor";
            this.buttonUseColor.Size = new System.Drawing.Size(75, 23);
            this.buttonUseColor.TabIndex = 11;
            this.buttonUseColor.Text = "Use Color";
            this.buttonUseColor.UseVisualStyleBackColor = true;
            this.buttonUseColor.Click += new System.EventHandler(this.buttonUseColor_Click);
            // 
            // groupBoxPreset
            // 
            this.groupBoxPreset.Controls.Add(this.numericUpDownGreen);
            this.groupBoxPreset.Controls.Add(this.numericUpDownBlue);
            this.groupBoxPreset.Controls.Add(this.numericUpDownRed);
            this.groupBoxPreset.Controls.Add(this.buttonUseColor);
            this.groupBoxPreset.Controls.Add(this.labelDisplayMillisecoond);
            this.groupBoxPreset.Controls.Add(this.panelSwatch);
            this.groupBoxPreset.Controls.Add(this.textBoxDisplayMilisecond);
            this.groupBoxPreset.Controls.Add(this.labelRed);
            this.groupBoxPreset.Controls.Add(this.labelBlue);
            this.groupBoxPreset.Controls.Add(this.labelGreen);
            this.groupBoxPreset.Location = new System.Drawing.Point(0, 0);
            this.groupBoxPreset.Margin = new System.Windows.Forms.Padding(1);
            this.groupBoxPreset.Name = "groupBoxPreset";
            this.groupBoxPreset.Padding = new System.Windows.Forms.Padding(1);
            this.groupBoxPreset.Size = new System.Drawing.Size(574, 36);
            this.groupBoxPreset.TabIndex = 12;
            this.groupBoxPreset.TabStop = false;
            this.groupBoxPreset.Text = "Preset # ";
            // 
            // numericUpDownGreen
            // 
            this.numericUpDownGreen.Location = new System.Drawing.Point(299, 12);
            this.numericUpDownGreen.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numericUpDownGreen.Name = "numericUpDownGreen";
            this.numericUpDownGreen.Size = new System.Drawing.Size(44, 20);
            this.numericUpDownGreen.TabIndex = 14;
            this.numericUpDownGreen.ValueChanged += new System.EventHandler(this.numericUpDownGreen_ValueChanged);
            // 
            // numericUpDownBlue
            // 
            this.numericUpDownBlue.Location = new System.Drawing.Point(386, 12);
            this.numericUpDownBlue.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numericUpDownBlue.Name = "numericUpDownBlue";
            this.numericUpDownBlue.Size = new System.Drawing.Size(44, 20);
            this.numericUpDownBlue.TabIndex = 13;
            this.numericUpDownBlue.ValueChanged += new System.EventHandler(this.numericUpDownBlue_ValueChanged);
            // 
            // numericUpDownRed
            // 
            this.numericUpDownRed.Location = new System.Drawing.Point(204, 12);
            this.numericUpDownRed.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numericUpDownRed.Name = "numericUpDownRed";
            this.numericUpDownRed.Size = new System.Drawing.Size(44, 20);
            this.numericUpDownRed.TabIndex = 12;
            this.numericUpDownRed.ValueChanged += new System.EventHandler(this.numericUpDownRed_ValueChanged);
            // 
            // PresetControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBoxPreset);
            this.Name = "PresetControl";
            this.Size = new System.Drawing.Size(573, 41);
            this.groupBoxPreset.ResumeLayout(false);
            this.groupBoxPreset.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownGreen)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownBlue)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownRed)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label labelDisplayMillisecoond;
        private System.Windows.Forms.TextBox textBoxDisplayMilisecond;
        private System.Windows.Forms.Label labelRed;
        private System.Windows.Forms.Label labelGreen;
        private System.Windows.Forms.Label labelBlue;
        private System.Windows.Forms.Panel panelSwatch;
        private System.Windows.Forms.Button buttonUseColor;
        private System.Windows.Forms.GroupBox groupBoxPreset;
        private System.Windows.Forms.NumericUpDown numericUpDownGreen;
        private System.Windows.Forms.NumericUpDown numericUpDownBlue;
        private System.Windows.Forms.NumericUpDown numericUpDownRed;
    }
}
