using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace WebKit
{
    public partial class SourceCodeForm : Form
    {
        public SourceCodeForm()
        {
            InitializeComponent();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("OpenWebKitSharp Source Viewer \r\n \r\n Made by Philippe Asmar \r\n \r\n Credits to: CodeProject for the FastColoredTextBox", "About Source Viewer", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void openFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog opn = new OpenFileDialog())
            {
                opn.Filter = "HTML Files (*.html; *.htm)|*.html; *.htm| All Files (*.*) | *.*";
                if (opn.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    fastColoredTextBox1.Text = System.IO.File.ReadAllText(opn.FileName);
                }
            }
        }

        private void savePageAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (SaveFileDialog sv = new SaveFileDialog())
            {
                sv.Filter = "HTML Files (*.html; *.htm)|*.html; *.htm| All Files (*.*) | *.*";
                if (sv.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    System.IO.File.WriteAllText(sv.FileName, fastColoredTextBox1.Text);
                }
            }
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void SourceCodeForm_Load(object sender, EventArgs e)
        {
            this.Text = LanguageLoader.ViewSource + " - " + GlobalPreferences.ApplicationName;
        }
    }
}
