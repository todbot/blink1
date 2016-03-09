using System;
using System.ComponentModel;
using System.Windows.Forms;

namespace ThingM.Blink1.ColorManager
{
    using ThingM.Blink1.ColorProcessor;

    public partial class PresetControl : UserControl
    {
        public string Title
        {
            get
            {
                return this.groupBoxPreset.Text;
            }

            set
            {
                this.groupBoxPreset.Text = string.IsNullOrEmpty(value) ? string.Empty : value;
            }
        }

        public IColorProcessor Color
        {
            get
            {
                return new Rgb((ushort)this.numericUpDownRed.Value, (ushort)this.numericUpDownGreen.Value, (ushort)this.numericUpDownBlue.Value);
            }
            set
            {
                this.numericUpDownRed.Value = value.ToRgb().Red;
                this.numericUpDownGreen.Value = value.ToRgb().Green;
                this.numericUpDownBlue.Value = value.ToRgb().Blue;
            }
        }

        public ushort Millisecond
        {
            get
            {
                return Convert.ToUInt16(this.textBoxDisplayMilisecond.Text);
            }

            set
            {
                this.textBoxDisplayMilisecond.Text = value.ToString();
            }
        }

        public PresetControl()
        {
            InitializeComponent();
        }

        private void buttonUseColor_Click(object sender, EventArgs e)
        {
            ColorManager parentForm = FindForm() as ColorManager;

            this.numericUpDownRed.Value = parentForm.hsbRed.Value;
            this.numericUpDownGreen.Value = parentForm.hsbGreen.Value;
            this.numericUpDownBlue.Value = parentForm.hsbBlue.Value;

            this.UpdateSwatch((int)this.numericUpDownRed.Value, (int)this.numericUpDownGreen.Value, (int)this.numericUpDownBlue.Value);
        }

        private void numericUpDownRed_ValueChanged(object sender, EventArgs e)
        {
            this.UpdateSwatch((int)this.numericUpDownRed.Value, (int)this.numericUpDownGreen.Value, (int)this.numericUpDownBlue.Value);
        }

        private void numericUpDownGreen_ValueChanged(object sender, EventArgs e)
        {
            this.UpdateSwatch((int)this.numericUpDownRed.Value, (int)this.numericUpDownGreen.Value, (int)this.numericUpDownBlue.Value);
        }

        private void numericUpDownBlue_ValueChanged(object sender, EventArgs e)
        {
            this.UpdateSwatch((int)this.numericUpDownRed.Value, (int)this.numericUpDownGreen.Value, (int)this.numericUpDownBlue.Value);
        }

        private void UpdateSwatch(int red, int green, int blue)
        {
            this.panelSwatch.BackColor = System.Drawing.Color.FromArgb(red, green, blue);
        }




        void textBoxDisplayMilisecond_Validating(object sender, CancelEventArgs e)
        {
            ushort numberEntered;

            if (ushort.TryParse(textBoxDisplayMilisecond.Text, out numberEntered) == false)
            {
                MessageBox.Show("You need to enter an positive number");
                this.textBoxDisplayMilisecond.Text = 1000.ToString();
            }
        }
    }
}
