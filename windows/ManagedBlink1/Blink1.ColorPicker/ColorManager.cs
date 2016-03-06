using System;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;

namespace ThingM.Blink1.ColorManager
{
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Xml.Linq;

    using ThingM.Blink1;
    using ThingM.Blink1.ColorProcessor;

    /// <summary>
	/// Summary description for ColorManager.
	/// </summary>
	public class ColorManager : System.Windows.Forms.Form
	{
		internal System.Windows.Forms.Label lblBlue;
		internal System.Windows.Forms.Label lblGreen;
		internal System.Windows.Forms.Label lblRed;
		internal System.Windows.Forms.Label lblBrightness;
		internal System.Windows.Forms.Label lblSaturation;
		internal System.Windows.Forms.Label lblHue;
		internal System.Windows.Forms.HScrollBar hsbBlue;
		internal System.Windows.Forms.HScrollBar hsbGreen;
		internal System.Windows.Forms.HScrollBar hsbRed;
		internal System.Windows.Forms.HScrollBar hsbBrightness;
		internal System.Windows.Forms.HScrollBar hsbSaturation;
        internal System.Windows.Forms.HScrollBar hsbHue;
		internal System.Windows.Forms.Label Label3;
		internal System.Windows.Forms.Label Label7;
		internal System.Windows.Forms.Panel pnlColor;
		internal System.Windows.Forms.Label Label6;
		internal System.Windows.Forms.Label Label1;
		internal System.Windows.Forms.Label Label5;
		internal System.Windows.Forms.Panel pnlSelectedColor;
		internal System.Windows.Forms.Panel pnlBrightness;
        internal System.Windows.Forms.Label Label2;
        private ThingM.Blink1.ColorManager.PresetControl preset0;
        private ThingM.Blink1.ColorManager.PresetControl preset1;
        private ThingM.Blink1.ColorManager.PresetControl preset2;
        private ThingM.Blink1.ColorManager.PresetControl preset3;
        private ThingM.Blink1.ColorManager.PresetControl preset4;
        private ThingM.Blink1.ColorManager.PresetControl preset5;
        private ThingM.Blink1.ColorManager.PresetControl preset6;
        private ThingM.Blink1.ColorManager.PresetControl preset7;
        private ThingM.Blink1.ColorManager.PresetControl preset8;
        private ThingM.Blink1.ColorManager.PresetControl preset9;
        private ThingM.Blink1.ColorManager.PresetControl preset10;
        private ThingM.Blink1.ColorManager.PresetControl preset11;
        private FlowLayoutPanel flowLayoutPanelPreset;
        private GroupBox groupBoxWaveLength;
        private GroupBox groupBoxColor;
        private GroupBox groupBoxDevice;
        private ComboBox comboBoxDevice;
        private MenuStrip menuStripForm;
        private ToolStripMenuItem fileToolStripMenuItem;
        private ToolStripMenuItem deviceToolStripMenuItem;
        private ToolStripMenuItem loadPresetFromFileToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem1;
        private ToolStripMenuItem savePresetToFileToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem2;
        private ToolStripMenuItem exitToolStripMenuItem;
        private ToolStripMenuItem loadPresetFromTheSelectedBlink1DeviceToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem3;
        private ToolStripMenuItem savePresetToTheSelectedBlink1DeviceToolStripMenuItem;
        private ToolStripMenuItem fileToolStripMenuItem1;
        private ToolStripMenuItem loadPresetFromFileToolStripMenuItem1;
        private ToolStripSeparator toolStripMenuItem4;
        private ToolStripMenuItem savePresetToFileToolStripMenuItem1;
        private ToolStripSeparator toolStripMenuItem5;
        private ToolStripMenuItem exitToolStripMenuItem1;
        private ToolStripMenuItem deviceToolStripMenuItem1;
        private ToolStripMenuItem loadPresetFromSelectedBlink1DeviceToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem6;
        private ToolStripMenuItem savePresetToSelectedBlink1DeviceToolStripMenuItem;
        private TextBox textBoxInactivityMillisecond;
        private CheckBox checkBoxInactivityMode;
        private Label labelInactivityMillisecond;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ColorManager()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ColorManager));
            this.lblBlue = new System.Windows.Forms.Label();
            this.lblGreen = new System.Windows.Forms.Label();
            this.lblRed = new System.Windows.Forms.Label();
            this.lblBrightness = new System.Windows.Forms.Label();
            this.lblSaturation = new System.Windows.Forms.Label();
            this.lblHue = new System.Windows.Forms.Label();
            this.hsbBlue = new System.Windows.Forms.HScrollBar();
            this.hsbGreen = new System.Windows.Forms.HScrollBar();
            this.hsbRed = new System.Windows.Forms.HScrollBar();
            this.hsbBrightness = new System.Windows.Forms.HScrollBar();
            this.hsbSaturation = new System.Windows.Forms.HScrollBar();
            this.hsbHue = new System.Windows.Forms.HScrollBar();
            this.Label3 = new System.Windows.Forms.Label();
            this.Label7 = new System.Windows.Forms.Label();
            this.pnlColor = new System.Windows.Forms.Panel();
            this.Label6 = new System.Windows.Forms.Label();
            this.Label1 = new System.Windows.Forms.Label();
            this.Label5 = new System.Windows.Forms.Label();
            this.pnlSelectedColor = new System.Windows.Forms.Panel();
            this.pnlBrightness = new System.Windows.Forms.Panel();
            this.Label2 = new System.Windows.Forms.Label();
            this.flowLayoutPanelPreset = new System.Windows.Forms.FlowLayoutPanel();
            this.groupBoxWaveLength = new System.Windows.Forms.GroupBox();
            this.groupBoxColor = new System.Windows.Forms.GroupBox();
            this.groupBoxDevice = new System.Windows.Forms.GroupBox();
            this.labelInactivityMillisecond = new System.Windows.Forms.Label();
            this.textBoxInactivityMillisecond = new System.Windows.Forms.TextBox();
            this.checkBoxInactivityMode = new System.Windows.Forms.CheckBox();
            this.comboBoxDevice = new System.Windows.Forms.ComboBox();
            this.menuStripForm = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.loadPresetFromFileToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
            this.savePresetToFileToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem5 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.deviceToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.loadPresetFromSelectedBlink1DeviceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem6 = new System.Windows.Forms.ToolStripSeparator();
            this.savePresetToSelectedBlink1DeviceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deviceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadPresetFromFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.savePresetToFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadPresetFromTheSelectedBlink1DeviceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.savePresetToTheSelectedBlink1DeviceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.preset0 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset1 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset2 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset3 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset4 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset5 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset6 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset7 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset8 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset9 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset10 = new ThingM.Blink1.ColorManager.PresetControl();
            this.preset11 = new ThingM.Blink1.ColorManager.PresetControl();
            this.flowLayoutPanelPreset.SuspendLayout();
            this.groupBoxWaveLength.SuspendLayout();
            this.groupBoxColor.SuspendLayout();
            this.groupBoxDevice.SuspendLayout();
            this.menuStripForm.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblBlue
            // 
            this.lblBlue.Location = new System.Drawing.Point(315, 64);
            this.lblBlue.Name = "lblBlue";
            this.lblBlue.Size = new System.Drawing.Size(40, 23);
            this.lblBlue.TabIndex = 54;
            this.lblBlue.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblGreen
            // 
            this.lblGreen.Location = new System.Drawing.Point(315, 40);
            this.lblGreen.Name = "lblGreen";
            this.lblGreen.Size = new System.Drawing.Size(40, 23);
            this.lblGreen.TabIndex = 53;
            this.lblGreen.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblRed
            // 
            this.lblRed.Location = new System.Drawing.Point(315, 16);
            this.lblRed.Name = "lblRed";
            this.lblRed.Size = new System.Drawing.Size(40, 23);
            this.lblRed.TabIndex = 52;
            this.lblRed.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblBrightness
            // 
            this.lblBrightness.Location = new System.Drawing.Point(315, 64);
            this.lblBrightness.Name = "lblBrightness";
            this.lblBrightness.Size = new System.Drawing.Size(40, 23);
            this.lblBrightness.TabIndex = 51;
            this.lblBrightness.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblSaturation
            // 
            this.lblSaturation.Location = new System.Drawing.Point(315, 40);
            this.lblSaturation.Name = "lblSaturation";
            this.lblSaturation.Size = new System.Drawing.Size(40, 23);
            this.lblSaturation.TabIndex = 50;
            this.lblSaturation.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblHue
            // 
            this.lblHue.Location = new System.Drawing.Point(315, 16);
            this.lblHue.Name = "lblHue";
            this.lblHue.Size = new System.Drawing.Size(40, 23);
            this.lblHue.TabIndex = 49;
            this.lblHue.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // hsbBlue
            // 
            this.hsbBlue.LargeChange = 1;
            this.hsbBlue.Location = new System.Drawing.Point(83, 64);
            this.hsbBlue.Maximum = 255;
            this.hsbBlue.Name = "hsbBlue";
            this.hsbBlue.Size = new System.Drawing.Size(224, 18);
            this.hsbBlue.TabIndex = 48;
            this.hsbBlue.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HandleRGBScroll);
            // 
            // hsbGreen
            // 
            this.hsbGreen.LargeChange = 1;
            this.hsbGreen.Location = new System.Drawing.Point(83, 40);
            this.hsbGreen.Maximum = 255;
            this.hsbGreen.Name = "hsbGreen";
            this.hsbGreen.Size = new System.Drawing.Size(224, 18);
            this.hsbGreen.TabIndex = 47;
            this.hsbGreen.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HandleRGBScroll);
            // 
            // hsbRed
            // 
            this.hsbRed.LargeChange = 1;
            this.hsbRed.Location = new System.Drawing.Point(83, 16);
            this.hsbRed.Maximum = 255;
            this.hsbRed.Name = "hsbRed";
            this.hsbRed.Size = new System.Drawing.Size(224, 18);
            this.hsbRed.TabIndex = 46;
            this.hsbRed.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HandleRGBScroll);
            // 
            // hsbBrightness
            // 
            this.hsbBrightness.LargeChange = 1;
            this.hsbBrightness.Location = new System.Drawing.Point(83, 64);
            this.hsbBrightness.Maximum = 255;
            this.hsbBrightness.Name = "hsbBrightness";
            this.hsbBrightness.Size = new System.Drawing.Size(224, 18);
            this.hsbBrightness.TabIndex = 45;
            this.hsbBrightness.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HandleHSVScroll);
            // 
            // hsbSaturation
            // 
            this.hsbSaturation.LargeChange = 1;
            this.hsbSaturation.Location = new System.Drawing.Point(83, 40);
            this.hsbSaturation.Maximum = 255;
            this.hsbSaturation.Name = "hsbSaturation";
            this.hsbSaturation.Size = new System.Drawing.Size(224, 18);
            this.hsbSaturation.TabIndex = 44;
            this.hsbSaturation.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HandleHSVScroll);
            // 
            // hsbHue
            // 
            this.hsbHue.LargeChange = 1;
            this.hsbHue.Location = new System.Drawing.Point(83, 16);
            this.hsbHue.Maximum = 255;
            this.hsbHue.Name = "hsbHue";
            this.hsbHue.Size = new System.Drawing.Size(224, 18);
            this.hsbHue.TabIndex = 43;
            this.hsbHue.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HandleHSVScroll);
            // 
            // Label3
            // 
            this.Label3.Location = new System.Drawing.Point(11, 64);
            this.Label3.Name = "Label3";
            this.Label3.Size = new System.Drawing.Size(72, 18);
            this.Label3.TabIndex = 34;
            this.Label3.Text = "Blue";
            this.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Label7
            // 
            this.Label7.Location = new System.Drawing.Point(11, 64);
            this.Label7.Name = "Label7";
            this.Label7.Size = new System.Drawing.Size(72, 18);
            this.Label7.TabIndex = 37;
            this.Label7.Text = "Brightness";
            this.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // pnlColor
            // 
            this.pnlColor.Location = new System.Drawing.Point(12, 95);
            this.pnlColor.Name = "pnlColor";
            this.pnlColor.Size = new System.Drawing.Size(224, 216);
            this.pnlColor.TabIndex = 38;
            this.pnlColor.Visible = false;
            // 
            // Label6
            // 
            this.Label6.Location = new System.Drawing.Point(11, 40);
            this.Label6.Name = "Label6";
            this.Label6.Size = new System.Drawing.Size(72, 18);
            this.Label6.TabIndex = 36;
            this.Label6.Text = "Saturation";
            this.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Label1
            // 
            this.Label1.Location = new System.Drawing.Point(11, 16);
            this.Label1.Name = "Label1";
            this.Label1.Size = new System.Drawing.Size(72, 18);
            this.Label1.TabIndex = 32;
            this.Label1.Text = "Red";
            this.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Label5
            // 
            this.Label5.Location = new System.Drawing.Point(11, 16);
            this.Label5.Name = "Label5";
            this.Label5.Size = new System.Drawing.Size(72, 18);
            this.Label5.TabIndex = 35;
            this.Label5.Text = "Hue";
            this.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // pnlSelectedColor
            // 
            this.pnlSelectedColor.Location = new System.Drawing.Point(292, 95);
            this.pnlSelectedColor.Name = "pnlSelectedColor";
            this.pnlSelectedColor.Size = new System.Drawing.Size(82, 216);
            this.pnlSelectedColor.TabIndex = 40;
            this.pnlSelectedColor.Visible = false;
            // 
            // pnlBrightness
            // 
            this.pnlBrightness.Location = new System.Drawing.Point(244, 95);
            this.pnlBrightness.Name = "pnlBrightness";
            this.pnlBrightness.Size = new System.Drawing.Size(28, 216);
            this.pnlBrightness.TabIndex = 39;
            this.pnlBrightness.Visible = false;
            // 
            // Label2
            // 
            this.Label2.Location = new System.Drawing.Point(11, 40);
            this.Label2.Name = "Label2";
            this.Label2.Size = new System.Drawing.Size(72, 18);
            this.Label2.TabIndex = 33;
            this.Label2.Text = "Green";
            this.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // flowLayoutPanelPreset
            // 
            this.flowLayoutPanelPreset.Controls.Add(this.preset0);
            this.flowLayoutPanelPreset.Controls.Add(this.preset1);
            this.flowLayoutPanelPreset.Controls.Add(this.preset2);
            this.flowLayoutPanelPreset.Controls.Add(this.preset3);
            this.flowLayoutPanelPreset.Controls.Add(this.preset4);
            this.flowLayoutPanelPreset.Controls.Add(this.preset5);
            this.flowLayoutPanelPreset.Controls.Add(this.preset6);
            this.flowLayoutPanelPreset.Controls.Add(this.preset7);
            this.flowLayoutPanelPreset.Controls.Add(this.preset8);
            this.flowLayoutPanelPreset.Controls.Add(this.preset9);
            this.flowLayoutPanelPreset.Controls.Add(this.preset10);
            this.flowLayoutPanelPreset.Controls.Add(this.preset11);
            this.flowLayoutPanelPreset.Location = new System.Drawing.Point(380, 85);
            this.flowLayoutPanelPreset.Name = "flowLayoutPanelPreset";
            this.flowLayoutPanelPreset.Size = new System.Drawing.Size(588, 550);
            this.flowLayoutPanelPreset.TabIndex = 56;
            // 
            // groupBoxWaveLength
            // 
            this.groupBoxWaveLength.Controls.Add(this.hsbHue);
            this.groupBoxWaveLength.Controls.Add(this.Label7);
            this.groupBoxWaveLength.Controls.Add(this.lblHue);
            this.groupBoxWaveLength.Controls.Add(this.lblSaturation);
            this.groupBoxWaveLength.Controls.Add(this.Label6);
            this.groupBoxWaveLength.Controls.Add(this.lblBrightness);
            this.groupBoxWaveLength.Controls.Add(this.hsbSaturation);
            this.groupBoxWaveLength.Controls.Add(this.hsbBrightness);
            this.groupBoxWaveLength.Controls.Add(this.Label5);
            this.groupBoxWaveLength.Location = new System.Drawing.Point(12, 323);
            this.groupBoxWaveLength.Name = "groupBoxWaveLength";
            this.groupBoxWaveLength.Size = new System.Drawing.Size(362, 93);
            this.groupBoxWaveLength.TabIndex = 57;
            this.groupBoxWaveLength.TabStop = false;
            this.groupBoxWaveLength.Text = "Wavelength";
            // 
            // groupBoxColor
            // 
            this.groupBoxColor.Controls.Add(this.hsbRed);
            this.groupBoxColor.Controls.Add(this.hsbBlue);
            this.groupBoxColor.Controls.Add(this.Label3);
            this.groupBoxColor.Controls.Add(this.Label2);
            this.groupBoxColor.Controls.Add(this.hsbGreen);
            this.groupBoxColor.Controls.Add(this.lblBlue);
            this.groupBoxColor.Controls.Add(this.Label1);
            this.groupBoxColor.Controls.Add(this.lblRed);
            this.groupBoxColor.Controls.Add(this.lblGreen);
            this.groupBoxColor.Location = new System.Drawing.Point(12, 426);
            this.groupBoxColor.Name = "groupBoxColor";
            this.groupBoxColor.Size = new System.Drawing.Size(362, 93);
            this.groupBoxColor.TabIndex = 58;
            this.groupBoxColor.TabStop = false;
            this.groupBoxColor.Text = "Color";
            // 
            // groupBoxDevice
            // 
            this.groupBoxDevice.Controls.Add(this.labelInactivityMillisecond);
            this.groupBoxDevice.Controls.Add(this.textBoxInactivityMillisecond);
            this.groupBoxDevice.Controls.Add(this.checkBoxInactivityMode);
            this.groupBoxDevice.Controls.Add(this.comboBoxDevice);
            this.groupBoxDevice.Location = new System.Drawing.Point(12, 27);
            this.groupBoxDevice.Name = "groupBoxDevice";
            this.groupBoxDevice.Size = new System.Drawing.Size(944, 51);
            this.groupBoxDevice.TabIndex = 59;
            this.groupBoxDevice.TabStop = false;
            this.groupBoxDevice.Text = "Device(s)";
            // 
            // labelInactivityMillisecond
            // 
            this.labelInactivityMillisecond.AutoSize = true;
            this.labelInactivityMillisecond.Location = new System.Drawing.Point(862, 23);
            this.labelInactivityMillisecond.Name = "labelInactivityMillisecond";
            this.labelInactivityMillisecond.Size = new System.Drawing.Size(64, 13);
            this.labelInactivityMillisecond.TabIndex = 4;
            this.labelInactivityMillisecond.Text = "Milliseconds";
            // 
            // textBoxInactivityMillisecond
            // 
            this.textBoxInactivityMillisecond.Location = new System.Drawing.Point(813, 20);
            this.textBoxInactivityMillisecond.Name = "textBoxInactivityMillisecond";
            this.textBoxInactivityMillisecond.Size = new System.Drawing.Size(43, 20);
            this.textBoxInactivityMillisecond.TabIndex = 3;
            this.textBoxInactivityMillisecond.Text = "60000";
            this.textBoxInactivityMillisecond.Validating += new System.ComponentModel.CancelEventHandler(this.textBoxInactivityMillisecond_Validating);
            // 
            // checkBoxInactivityMode
            // 
            this.checkBoxInactivityMode.AutoSize = true;
            this.checkBoxInactivityMode.Location = new System.Drawing.Point(598, 21);
            this.checkBoxInactivityMode.Name = "checkBoxInactivityMode";
            this.checkBoxInactivityMode.Size = new System.Drawing.Size(212, 17);
            this.checkBoxInactivityMode.TabIndex = 1;
            this.checkBoxInactivityMode.Text = "Inactivity Mode After Being Inactive For";
            this.checkBoxInactivityMode.UseVisualStyleBackColor = true;
            // 
            // comboBoxDevice
            // 
            this.comboBoxDevice.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxDevice.FormattingEnabled = true;
            this.comboBoxDevice.Location = new System.Drawing.Point(6, 19);
            this.comboBoxDevice.Name = "comboBoxDevice";
            this.comboBoxDevice.Size = new System.Drawing.Size(577, 21);
            this.comboBoxDevice.TabIndex = 0;
            this.comboBoxDevice.SelectedIndexChanged += new System.EventHandler(this.comboBoxDevice_SelectedIndexChanged);
            // 
            // menuStripForm
            // 
            this.menuStripForm.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem1,
            this.deviceToolStripMenuItem1});
            this.menuStripForm.Location = new System.Drawing.Point(0, 0);
            this.menuStripForm.Name = "menuStripForm";
            this.menuStripForm.Size = new System.Drawing.Size(968, 24);
            this.menuStripForm.TabIndex = 60;
            // 
            // fileToolStripMenuItem1
            // 
            this.fileToolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadPresetFromFileToolStripMenuItem1,
            this.toolStripMenuItem4,
            this.savePresetToFileToolStripMenuItem1,
            this.toolStripMenuItem5,
            this.exitToolStripMenuItem1});
            this.fileToolStripMenuItem1.Name = "fileToolStripMenuItem1";
            this.fileToolStripMenuItem1.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem1.Text = "File";
            // 
            // loadPresetFromFileToolStripMenuItem1
            // 
            this.loadPresetFromFileToolStripMenuItem1.Image = global::ThingM.Blink1.ColorManager.Properties.Resources.LoadFromFile;
            this.loadPresetFromFileToolStripMenuItem1.Name = "loadPresetFromFileToolStripMenuItem1";
            this.loadPresetFromFileToolStripMenuItem1.Size = new System.Drawing.Size(196, 22);
            this.loadPresetFromFileToolStripMenuItem1.Text = "Load Preset From File...";
            this.loadPresetFromFileToolStripMenuItem1.Click += new System.EventHandler(this.loadPresetFromFileToolStripMenuItem1_Click);
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(193, 6);
            // 
            // savePresetToFileToolStripMenuItem1
            // 
            this.savePresetToFileToolStripMenuItem1.Image = global::ThingM.Blink1.ColorManager.Properties.Resources.SaveToFile;
            this.savePresetToFileToolStripMenuItem1.Name = "savePresetToFileToolStripMenuItem1";
            this.savePresetToFileToolStripMenuItem1.Size = new System.Drawing.Size(196, 22);
            this.savePresetToFileToolStripMenuItem1.Text = "Save Preset To File...";
            this.savePresetToFileToolStripMenuItem1.Click += new System.EventHandler(this.savePresetToFileToolStripMenuItem1_Click);
            // 
            // toolStripMenuItem5
            // 
            this.toolStripMenuItem5.Name = "toolStripMenuItem5";
            this.toolStripMenuItem5.Size = new System.Drawing.Size(193, 6);
            // 
            // exitToolStripMenuItem1
            // 
            this.exitToolStripMenuItem1.Image = global::ThingM.Blink1.ColorManager.Properties.Resources.ExitApplication;
            this.exitToolStripMenuItem1.Name = "exitToolStripMenuItem1";
            this.exitToolStripMenuItem1.Size = new System.Drawing.Size(196, 22);
            this.exitToolStripMenuItem1.Text = "Exit";
            this.exitToolStripMenuItem1.Click += new System.EventHandler(this.exitToolStripMenuItem1_Click);
            // 
            // deviceToolStripMenuItem1
            // 
            this.deviceToolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadPresetFromSelectedBlink1DeviceToolStripMenuItem,
            this.toolStripMenuItem6,
            this.savePresetToSelectedBlink1DeviceToolStripMenuItem});
            this.deviceToolStripMenuItem1.Name = "deviceToolStripMenuItem1";
            this.deviceToolStripMenuItem1.Size = new System.Drawing.Size(54, 20);
            this.deviceToolStripMenuItem1.Text = "Device";
            // 
            // loadPresetFromSelectedBlink1DeviceToolStripMenuItem
            // 
            this.loadPresetFromSelectedBlink1DeviceToolStripMenuItem.Image = global::ThingM.Blink1.ColorManager.Properties.Resources.LoadFromDevice;
            this.loadPresetFromSelectedBlink1DeviceToolStripMenuItem.Name = "loadPresetFromSelectedBlink1DeviceToolStripMenuItem";
            this.loadPresetFromSelectedBlink1DeviceToolStripMenuItem.Size = new System.Drawing.Size(294, 22);
            this.loadPresetFromSelectedBlink1DeviceToolStripMenuItem.Text = "Load Preset From Selected Blink(1) Device";
            this.loadPresetFromSelectedBlink1DeviceToolStripMenuItem.Click += new System.EventHandler(this.loadPresetFromSelectedBlink1DeviceToolStripMenuItem_Click);
            // 
            // toolStripMenuItem6
            // 
            this.toolStripMenuItem6.Name = "toolStripMenuItem6";
            this.toolStripMenuItem6.Size = new System.Drawing.Size(291, 6);
            // 
            // savePresetToSelectedBlink1DeviceToolStripMenuItem
            // 
            this.savePresetToSelectedBlink1DeviceToolStripMenuItem.Image = global::ThingM.Blink1.ColorManager.Properties.Resources.SaveToDevice;
            this.savePresetToSelectedBlink1DeviceToolStripMenuItem.Name = "savePresetToSelectedBlink1DeviceToolStripMenuItem";
            this.savePresetToSelectedBlink1DeviceToolStripMenuItem.Size = new System.Drawing.Size(294, 22);
            this.savePresetToSelectedBlink1DeviceToolStripMenuItem.Text = "Save Preset To Selected Blink(1) Device";
            this.savePresetToSelectedBlink1DeviceToolStripMenuItem.Click += new System.EventHandler(this.savePresetToSelectedBlink1DeviceToolStripMenuItem_Click);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // deviceToolStripMenuItem
            // 
            this.deviceToolStripMenuItem.Name = "deviceToolStripMenuItem";
            this.deviceToolStripMenuItem.Size = new System.Drawing.Size(54, 20);
            this.deviceToolStripMenuItem.Text = "Device";
            // 
            // loadPresetFromFileToolStripMenuItem
            // 
            this.loadPresetFromFileToolStripMenuItem.Name = "loadPresetFromFileToolStripMenuItem";
            this.loadPresetFromFileToolStripMenuItem.Size = new System.Drawing.Size(192, 22);
            this.loadPresetFromFileToolStripMenuItem.Text = "Load preset from file...";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(189, 6);
            // 
            // savePresetToFileToolStripMenuItem
            // 
            this.savePresetToFileToolStripMenuItem.Name = "savePresetToFileToolStripMenuItem";
            this.savePresetToFileToolStripMenuItem.Size = new System.Drawing.Size(192, 22);
            this.savePresetToFileToolStripMenuItem.Text = "Save preset to file...";
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(189, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(192, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            // 
            // loadPresetFromTheSelectedBlink1DeviceToolStripMenuItem
            // 
            this.loadPresetFromTheSelectedBlink1DeviceToolStripMenuItem.Name = "loadPresetFromTheSelectedBlink1DeviceToolStripMenuItem";
            this.loadPresetFromTheSelectedBlink1DeviceToolStripMenuItem.Size = new System.Drawing.Size(310, 22);
            this.loadPresetFromTheSelectedBlink1DeviceToolStripMenuItem.Text = "Load preset from the selected Blink(1) device";
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(307, 6);
            // 
            // savePresetToTheSelectedBlink1DeviceToolStripMenuItem
            // 
            this.savePresetToTheSelectedBlink1DeviceToolStripMenuItem.Name = "savePresetToTheSelectedBlink1DeviceToolStripMenuItem";
            this.savePresetToTheSelectedBlink1DeviceToolStripMenuItem.Size = new System.Drawing.Size(310, 22);
            this.savePresetToTheSelectedBlink1DeviceToolStripMenuItem.Text = "Save preset to the selected Blink(1) device";
            // 
            // preset0
            // 
            this.preset0.Location = new System.Drawing.Point(3, 3);
            this.preset0.Millisecond = ((ushort)(1000));
            this.preset0.Name = "preset0";
            this.preset0.Size = new System.Drawing.Size(580, 40);
            this.preset0.TabIndex = 0;
            this.preset0.Title = "Preset # 1";
            // 
            // preset1
            // 
            this.preset1.Location = new System.Drawing.Point(3, 49);
            this.preset1.Millisecond = ((ushort)(1000));
            this.preset1.Name = "preset1";
            this.preset1.Size = new System.Drawing.Size(580, 40);
            this.preset1.TabIndex = 1;
            this.preset1.Title = "Preset # 2";
            // 
            // preset2
            // 
            this.preset2.Location = new System.Drawing.Point(3, 95);
            this.preset2.Millisecond = ((ushort)(1000));
            this.preset2.Name = "preset2";
            this.preset2.Size = new System.Drawing.Size(580, 40);
            this.preset2.TabIndex = 2;
            this.preset2.Title = "Preset # 3";
            // 
            // preset3
            // 
            this.preset3.Location = new System.Drawing.Point(3, 141);
            this.preset3.Millisecond = ((ushort)(1000));
            this.preset3.Name = "preset3";
            this.preset3.Size = new System.Drawing.Size(580, 40);
            this.preset3.TabIndex = 3;
            this.preset3.Title = "Preset # 4";
            // 
            // preset4
            // 
            this.preset4.Location = new System.Drawing.Point(3, 187);
            this.preset4.Millisecond = ((ushort)(1000));
            this.preset4.Name = "preset4";
            this.preset4.Size = new System.Drawing.Size(580, 40);
            this.preset4.TabIndex = 4;
            this.preset4.Title = "Preset # 5";
            // 
            // preset5
            // 
            this.preset5.Location = new System.Drawing.Point(3, 233);
            this.preset5.Millisecond = ((ushort)(1000));
            this.preset5.Name = "preset5";
            this.preset5.Size = new System.Drawing.Size(580, 40);
            this.preset5.TabIndex = 5;
            this.preset5.Title = "Preset # 6";
            // 
            // preset6
            // 
            this.preset6.Location = new System.Drawing.Point(3, 279);
            this.preset6.Millisecond = ((ushort)(1000));
            this.preset6.Name = "preset6";
            this.preset6.Size = new System.Drawing.Size(580, 40);
            this.preset6.TabIndex = 6;
            this.preset6.Title = "Preset # 7";
            // 
            // preset7
            // 
            this.preset7.Location = new System.Drawing.Point(3, 325);
            this.preset7.Millisecond = ((ushort)(1000));
            this.preset7.Name = "preset7";
            this.preset7.Size = new System.Drawing.Size(580, 40);
            this.preset7.TabIndex = 7;
            this.preset7.Title = "Preset # 8";
            // 
            // preset8
            // 
            this.preset8.Location = new System.Drawing.Point(3, 371);
            this.preset8.Millisecond = ((ushort)(1000));
            this.preset8.Name = "preset8";
            this.preset8.Size = new System.Drawing.Size(580, 40);
            this.preset8.TabIndex = 8;
            this.preset8.Title = "Preset # 9";
            // 
            // preset9
            // 
            this.preset9.Location = new System.Drawing.Point(3, 417);
            this.preset9.Millisecond = ((ushort)(1000));
            this.preset9.Name = "preset9";
            this.preset9.Size = new System.Drawing.Size(580, 40);
            this.preset9.TabIndex = 9;
            this.preset9.Title = "Preset # 10";
            // 
            // preset10
            // 
            this.preset10.Location = new System.Drawing.Point(3, 463);
            this.preset10.Millisecond = ((ushort)(1000));
            this.preset10.Name = "preset10";
            this.preset10.Size = new System.Drawing.Size(580, 40);
            this.preset10.TabIndex = 10;
            this.preset10.Title = "Preset # 11";
            // 
            // preset11
            // 
            this.preset11.Location = new System.Drawing.Point(3, 509);
            this.preset11.Millisecond = ((ushort)(1000));
            this.preset11.Name = "preset11";
            this.preset11.Size = new System.Drawing.Size(580, 40);
            this.preset11.TabIndex = 11;
            this.preset11.Title = "Preset # 12";
            // 
            // ColorManager
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(968, 645);
            this.Controls.Add(this.groupBoxDevice);
            this.Controls.Add(this.groupBoxColor);
            this.Controls.Add(this.groupBoxWaveLength);
            this.Controls.Add(this.pnlSelectedColor);
            this.Controls.Add(this.flowLayoutPanelPreset);
            this.Controls.Add(this.pnlBrightness);
            this.Controls.Add(this.pnlColor);
            this.Controls.Add(this.menuStripForm);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStripForm;
            this.MaximizeBox = false;
            this.Name = "ColorManager";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Blink(1) Color Manager";
            this.Load += new System.EventHandler(this.ColorManager_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.ColorManager_Paint);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.HandleMouse);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.HandleMouse);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.frmMain_MouseUp);
            this.flowLayoutPanelPreset.ResumeLayout(false);
            this.groupBoxWaveLength.ResumeLayout(false);
            this.groupBoxColor.ResumeLayout(false);
            this.groupBoxDevice.ResumeLayout(false);
            this.groupBoxDevice.PerformLayout();
            this.menuStripForm.ResumeLayout(false);
            this.menuStripForm.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

		}
		#endregion

		private enum ChangeStyle
		{
			MouseMove,
			RGB,
			HSV,
			None
		}

		private ChangeStyle changeType = ChangeStyle.None;
		private Point selectedPoint;

		private ColorWheel myColorWheel;
		private ColorHandler.RGB RGB;
		private ColorHandler.HSV HSV;

        private Blink1 blink1 = new Blink1();

		private void ColorManager_Load(object sender, System.EventArgs e)
		{
			// Turn on double-buffering, so the form looks better. 
			this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			this.SetStyle(ControlStyles.UserPaint, true);
			this.SetStyle(ControlStyles.DoubleBuffer, true);

			// These properties are set in design view, as well, but they
			// have to be set to false in order for the Paint
			// event to be able to display their contents.
			// Never hurts to make sure they're invisible.
			pnlSelectedColor.Visible = false;
			pnlBrightness.Visible = false;
			pnlColor.Visible = false;

			// Calculate the coordinates of the three
			// required regions on the form.
            Rectangle SelectedColorRectangle = new Rectangle(pnlSelectedColor.Location, pnlSelectedColor.Size);
            Rectangle BrightnessRectangle = new Rectangle(pnlBrightness.Location, pnlBrightness.Size);
            Rectangle ColorRectangle = new Rectangle(pnlColor.Location, pnlColor.Size);

			// Create the new ColorWheel class, indicating
			// the locations of the color wheel itself, the
			// brightness area, and the position of the selected color.
			myColorWheel = new ColorWheel(ColorRectangle, BrightnessRectangle, SelectedColorRectangle);
			myColorWheel.ColorChanged += 
				new ColorWheel.ColorChangedEventHandler(this.myColorWheel_ColorChanged);

			// Set the RGB and HSV values 
			// of the NumericUpDown controls.
			SetRGB(RGB);
			SetHSV(HSV);

		    hsbBrightness.Value = 255;
		    HandleHSVScroll(null, null);

		    this.LoadBlinkDevice();
		}

        private void LoadBlinkDevice()
        {
            List<string> devicePaths = Blink1Info.GetDevicePath();

            if (devicePaths.Count == 0)
            {
                MessageBox.Show("No Blink(1) devices found.", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            Dictionary<string, string> deviceSources = new Dictionary<string, string>();

            for (int counter = 0; counter < devicePaths.Count; counter++)
            {
                deviceSources.Add(devicePaths[counter], string.Format("Blink(1) # {0}: HID: {1}", counter + 1, devicePaths[counter]));
            }

            this.comboBoxDevice.Enabled = false;

            this.comboBoxDevice.DataSource = new BindingSource(deviceSources, null);
            this.comboBoxDevice.DisplayMember = "Value";
            this.comboBoxDevice.ValueMember = "Key";

            this.comboBoxDevice.Enabled = true;

            if (deviceSources.Count > 0)
            {
                this.OpenSelectedBlinkDevice();
            }
        }

        private void OpenSelectedBlinkDevice()
        {
            if (this.comboBoxDevice.Enabled == false)
            {
                return;
            }

            if (this.comboBoxDevice.SelectedIndex < 0)
            {
                return;
            }

            if (this.blink1.IsConnected)
            {
                this.blink1.Close();
            }

            KeyValuePair<string, string> selectedItem = (KeyValuePair<string, string>)this.comboBoxDevice.SelectedItem;

            try
            {
                this.blink1.Open(selectedItem.Key);

                this.blink1.Blink(3, 250, 100, 255, 0, 0);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

		private void HandleMouse(object sender,  MouseEventArgs e)
		{
			// If you have the left mouse button down, 
			// then update the selectedPoint value and 
			// force a repaint of the color wheel.
			if ( e.Button == MouseButtons.Left ) 
			{
				changeType = ChangeStyle.MouseMove;
				selectedPoint = new Point(e.X, e.Y);
				this.Invalidate();
			}
		}

		private void frmMain_MouseUp(object sender,  MouseEventArgs e)
		{
			myColorWheel.SetMouseUp();
			changeType = ChangeStyle.None;
		}

		private void SetRGBLabels(ColorHandler.RGB RGB) 
		{
			RefreshText(lblRed, RGB.Red);
			RefreshText(lblBlue, RGB.Blue);
			RefreshText(lblGreen, RGB.Green);
		}

		private void SetHSVLabels(ColorHandler.HSV HSV) 
		{
			RefreshText(lblHue, HSV.Hue);
			RefreshText(lblSaturation, HSV.Saturation);
			RefreshText(lblBrightness, HSV.value);
		}

		private void SetRGB(ColorHandler.RGB RGB) 
		{
			// Update the RGB values on the form.
			RefreshValue(hsbRed, RGB.Red);
			RefreshValue(hsbBlue, RGB.Blue);
			RefreshValue(hsbGreen, RGB.Green);
			SetRGBLabels(RGB);
		}

		private void SetHSV( ColorHandler.HSV HSV) 
		{
			// Update the HSV values on the form.
			RefreshValue(hsbHue, HSV.Hue);
			RefreshValue(hsbSaturation, HSV.Saturation);
			RefreshValue(hsbBrightness, HSV.value);
			SetHSVLabels(HSV);
			}

		private void RefreshValue(HScrollBar hsb, int value) 
		{
			hsb.Value = value;
		}

		private void RefreshText(Label lbl, int value) 
		{
			lbl.Text = value.ToString();
		}

		public Color Color  
		{
			// Get or set the color to be
			// displayed in the color wheel.
			get 
			{
				return myColorWheel.Color;
			}

			set 
			{
				// Indicate the color change type. Either RGB or HSV
				// will cause the color wheel to update the position
				// of the pointer.
				changeType = ChangeStyle.RGB;
				RGB = new ColorHandler.RGB(value.R, value.G, value.B);
				HSV = ColorHandler.RGBtoHSV(RGB);
			}
		}

		private void myColorWheel_ColorChanged(object sender,  ColorChangedEventArgs e)  
		{
			SetRGB(e.RGB);
			SetHSV(e.HSV);

            if (this.blink1.IsConnected)
            {
                this.blink1.SetColor((ushort)e.RGB.Red, (ushort)e.RGB.Green, (ushort)e.RGB.Blue);
            }
		}

		private void HandleHSVScroll(object sender,  ScrollEventArgs e)  
			// If the H, S, or V values change, use this 
			// code to update the RGB values and invalidate
			// the color wheel (so it updates the pointers).
			// Check the isInUpdate flag to avoid recursive events
			// when you update the NumericUpdownControls.
		{
			changeType = ChangeStyle.HSV;
			HSV = new ColorHandler.HSV(hsbHue.Value, hsbSaturation.Value, hsbBrightness.Value);
			SetRGB(ColorHandler.HSVtoRGB(HSV));
			SetHSVLabels(HSV);
			this.Invalidate();
		}

		private void HandleRGBScroll(object sender, ScrollEventArgs e)
		{
			// If the R, G, or B values change, use this 
			// code to update the HSV values and invalidate
			// the color wheel (so it updates the pointers).
			// Check the isInUpdate flag to avoid recursive events
			// when you update the NumericUpdownControls.
			changeType = ChangeStyle.RGB;
			RGB = new ColorHandler.RGB(hsbRed.Value, hsbGreen.Value, hsbBlue.Value);
			SetHSV(ColorHandler.RGBtoHSV(RGB));
			SetRGBLabels(RGB);
			this.Invalidate();
		}

		private void ColorManager_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			// Depending on the circumstances, force a repaint
			// of the color wheel passing different information.
			switch (changeType)
			{
				case ChangeStyle.HSV:
					myColorWheel.Draw(e.Graphics, HSV);
					break;
				case ChangeStyle.MouseMove:
				case ChangeStyle.None:
					myColorWheel.Draw(e.Graphics, selectedPoint);
					break;
				case ChangeStyle.RGB:
					myColorWheel.Draw(e.Graphics, RGB);
					break;
			}
		}

        private void btnOK_Click(object sender, EventArgs e)
        {

        }

        private void importPresetFromTheSelectedBlink1DeviceToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void comboBoxDevice_SelectedIndexChanged(object sender, EventArgs e)
        {
            this.OpenSelectedBlinkDevice();
        }

        private void loadPresetFromSelectedBlink1DeviceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.blink1.IsConnected == false)
            {
                MessageBox.Show("No Blink(1) devices found.", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            for (int counter = 0; counter < Blink1Constant.NumberOfPreset; counter++)
            {
                Blink1Preset blink1Preset = this.blink1.ReadPreset((uint)counter);

                PresetControl control = (PresetControl)this.flowLayoutPanelPreset.Controls[counter];

                control.Color = blink1Preset.Rgb;

                control.Millisecond = blink1Preset.Millisecond;
            }
        }

        private void savePresetToSelectedBlink1DeviceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.blink1.IsConnected == false)
            {
                MessageBox.Show("No Blink(1) devices found.", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            for (int counter = 0; counter < Blink1Constant.NumberOfPreset; counter++)
            {
                PresetControl control = (PresetControl)this.flowLayoutPanelPreset.Controls[counter];

                Blink1Preset preset = new Blink1Preset(control.Millisecond, control.Color);

                this.blink1.SavePreset(preset, (ushort)counter);
            }

            if (this.checkBoxInactivityMode.CheckState == CheckState.Checked)
            {
                this.blink1.ActivateInactivityMode(Convert.ToUInt16(this.textBoxInactivityMillisecond.Text));
            }
            else
            {
                this.blink1.DeactivateInactivityMode();
            }
        }

        void textBoxInactivityMillisecond_Validating(object sender, CancelEventArgs e)
        {
            ushort numberEntered;

            if (ushort.TryParse(this.textBoxInactivityMillisecond.Text, out numberEntered) == false)
            {
                MessageBox.Show("You need to enter an positive number");
                this.textBoxInactivityMillisecond.Text = 60000.ToString();
            }
        }

        private void loadPresetFromFileToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog { Filter = "Blink(1) Preset Xml File|*.bxf", Title = "Load Blink(1) Preset Xml File" };
            openFileDialog.ShowDialog();

            if (string.IsNullOrEmpty(openFileDialog.FileName))
            {
                return;
            }

            XDocument document = XDocument.Load(openFileDialog.FileName);

            var holders = from x in document.Descendants("Preset") select new
                {
                    Id = x.Attribute("Id"),
                    Millisecond = x.Attribute("Millisecond"),
                    Red = x.Attribute("Red"),
                    Green = x.Attribute("Green"),
                    Blue = x.Attribute("Blue")
                };

            foreach (var holder in holders)
            {
                Control[] controls = this.flowLayoutPanelPreset.Controls.Find("preset" + holder.Id.Value, true);

                if (controls.Count() > 0)
                {
                    PresetControl presetControl = controls[0] as PresetControl;

                    presetControl.Millisecond = Convert.ToUInt16(holder.Millisecond.Value);
                    presetControl.Color = new Rgb(Convert.ToUInt16(holder.Red.Value), Convert.ToUInt16(holder.Green.Value), Convert.ToUInt16(holder.Blue.Value));
                }
            }

        }

        private void savePresetToFileToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            Dictionary<int, Blink1Preset> holders = new Dictionary<int, Blink1Preset>();

            for (int counter = 0; counter < Blink1Constant.NumberOfPreset; counter++)
            {
                PresetControl control = (PresetControl)this.flowLayoutPanelPreset.Controls[counter];

                holders.Add(counter, new Blink1Preset(control.Millisecond, control.Color));
            }

            XElement holderXml = new XElement(
                "Presets",
                from h in holders
                select
                    new XElement(
                    "Preset",
                    new XAttribute("Id", h.Key),
                    new XAttribute("Millisecond", h.Value.Millisecond),
                    new XAttribute("Red", h.Value.Rgb.Red),
                    new XAttribute("Green", h.Value.Rgb.Green),
                    new XAttribute("Blue", h.Value.Rgb.Blue)));

            SaveFileDialog saveFileDialog = new SaveFileDialog { Filter = "Blink(1) Preset Xml File|*.bxf", Title = "Save Blink(1) Preset Xml File" };
            saveFileDialog.ShowDialog();

            if (string.IsNullOrEmpty(saveFileDialog.FileName))
            {
                return;
            }

            File.WriteAllText(saveFileDialog.FileName, holderXml.ToString());
        }

        private void exitToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            this.Close();
            this.Dispose();
        }
    }
}

