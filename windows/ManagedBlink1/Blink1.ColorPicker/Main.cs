using System.Windows.Forms;

namespace ColorChooserCSharp
{
    using ThingM.Blink1.ColorManager;

    /// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class frmMain : System.Windows.Forms.Form
	{
		internal System.Windows.Forms.Button btnStandard;
		internal System.Windows.Forms.Button Button2;
		internal System.Windows.Forms.Label Label1;
		internal System.Windows.Forms.Button Button1;
		internal System.Windows.Forms.ColorDialog ColorDialog1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public frmMain()
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
				if (components != null) 
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
			this.btnStandard = new System.Windows.Forms.Button();
			this.Button2 = new System.Windows.Forms.Button();
			this.Label1 = new System.Windows.Forms.Label();
			this.Button1 = new System.Windows.Forms.Button();
			this.ColorDialog1 = new System.Windows.Forms.ColorDialog();
			this.SuspendLayout();
			// 
			// btnStandard
			// 
			this.btnStandard.Location = new System.Drawing.Point(8, 8);
			this.btnStandard.Name = "btnStandard";
			this.btnStandard.Size = new System.Drawing.Size(128, 32);
			this.btnStandard.TabIndex = 7;
			this.btnStandard.Text = "Standard Color Dialog";
			this.btnStandard.Click += new System.EventHandler(this.btnStandard_Click);
			// 
			// Button2
			// 
			this.Button2.Location = new System.Drawing.Point(8, 88);
			this.Button2.Name = "Button2";
			this.Button2.Size = new System.Drawing.Size(128, 32);
			this.Button2.TabIndex = 6;
			this.Button2.Text = "Test Form 2";
			this.Button2.Click += new System.EventHandler(this.Button2_Click);
			// 
			// Label1
			// 
			this.Label1.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(0)), ((System.Byte)(192)), ((System.Byte)(0)));
			this.Label1.Location = new System.Drawing.Point(160, 48);
			this.Label1.Name = "Label1";
			this.Label1.Size = new System.Drawing.Size(72, 32);
			this.Label1.TabIndex = 5;
			this.Label1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// Button1
			// 
			this.Button1.Location = new System.Drawing.Point(8, 48);
			this.Button1.Name = "Button1";
			this.Button1.Size = new System.Drawing.Size(128, 32);
			this.Button1.TabIndex = 4;
			this.Button1.Text = "Test Form 1";
			this.Button1.Click += new System.EventHandler(this.Button1_Click);
			// 
			// frmMain
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(240, 133);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																																	this.btnStandard,
																																	this.Button2,
																																	this.Label1,
																																	this.Button1});
			this.Name = "frmMain";
			this.Text = "ColorChooser Demonstration";
			this.ResumeLayout(false);

		}
		#endregion

        ///// <summary>
        ///// The main entry point for the application.
        ///// </summary>
        //[STAThread]
        //static void Main() 
        //{
        //    Application.Run(new frmMain());
        //}

		private void btnStandard_Click(object sender, System.EventArgs e)
		{
			ColorDialog1.Color = Label1.BackColor;
			ColorDialog1.FullOpen = true;
			if (ColorDialog1.ShowDialog() == DialogResult.OK)
			{
				Label1.BackColor = ColorDialog1.Color;
			}
		}

		private void Button1_Click(object sender, System.EventArgs e)
		{
		}

		private void Button2_Click(object sender, System.EventArgs e)
		{
            ColorManager frm = new ColorManager();
			frm.Color = Label1.BackColor;
			if (frm.ShowDialog(this) == DialogResult.OK)
			{
				Label1.BackColor = frm.Color;
			}
			frm.Dispose();
		}
	}
}
