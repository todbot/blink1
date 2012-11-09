using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Text;
using System.Windows.Forms;

namespace MyDownloader.App.Controls
{
    [ToolboxBitmap(typeof(BlockedProgressBar), "BlockedProgressBar.bmp")] //without namespace
    public partial class BlockedProgressBar : Control
    {
        BlockList _blockList;

        /// <summary>
        /// MyProgressBar Constructor
        /// </summary>
        public BlockedProgressBar()
        {
            InitializeComponent();
            _blockList = new BlockList();
            _direction = DirectionMode.Horizontal;
            SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.ResizeRedraw | ControlStyles.DoubleBuffer, true);
        }
        /// <summary>
        /// Update mode of segments
        /// </summary>
        [Description("The mode of update of progress bar")]
        [Category("MyProgressBar")]
        [RefreshProperties(RefreshProperties.All)]
        public BlockList.UpdateMode UpdateMode
        {
            get { return _blockList.Update; }
            set { _blockList.Update = value; }
        }

        /// <summary>
        /// Change quantity of segments
        /// </summary>
        [Description("The length of segments of progress bar")]
        [Category("MyProgressBar")]
        [RefreshProperties(RefreshProperties.All)]
        public int Length
        {
            get { return _blockList.Length; }
            set { _blockList.Length = value; this.Refresh(); }
        }
        /// <summary>
        /// Get or set filled segments 
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public int[] FilledSegments
        {
            get { return _blockList.FilledSegments; }
            set { _blockList.FilledSegments = value; this.Refresh(); }
        }
        /// <summary>
        /// Get or sets the full list of segments
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public bool[] FullListSegment
        {
            get { return _blockList.FullListSegment; }
            set { _blockList.FullListSegment = value; this.Refresh(); }
        }
        /// <summary>
        /// Get or set the block list of segments
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public List<Block> BlockList
        {
            get { return _blockList.List; }
            set { _blockList.List = value; this.Refresh(); }
        }
        /// <summary>
        /// DirectionMode of bar
        /// </summary>
        public enum DirectionMode : int
        {
            Horizontal = 0,
            Vertical = 1
        }
        private DirectionMode _direction = DirectionMode.Horizontal;
        /// <summary>
        /// Horizontal or Vertical
        /// </summary>
        [Description("The filling direction of progress bar")]
        [Category("MyProgressBar")]
        [RefreshProperties(RefreshProperties.All)]
        public DirectionMode Direction
        {
            get { return _direction; }
            set { _direction = value; this.Refresh(); }
        }
        /// <summary>
        /// OnPaint event
        /// </summary>
        /// <param name="pe"></param>
        protected override void OnPaint(PaintEventArgs pe)
        {
            Color Color1 = ControlPaint.Dark(this.ForeColor);
            Color Color2 = ControlPaint.Light(this.ForeColor);

            if (_direction == DirectionMode.Horizontal)
            {
                int top = ClientRectangle.Top + ClientRectangle.Height / 2 - 1;
                int height = ClientRectangle.Height - top;
                DrawRectangleH(pe, top, height, Color2, Color1);

                top = ClientRectangle.Top;
                height = ClientRectangle.Height / 2;
                DrawRectangleH(pe, top, height, Color1, Color2);
            }
            else
            {
                int left = ClientRectangle.Left;
                int width = ClientRectangle.Width / 2;
                DrawRectangleV(pe, left, width, Color1, Color2);

                left = ClientRectangle.Left + ClientRectangle.Width / 2;
                width = ClientRectangle.Width / 2;
                DrawRectangleV(pe, left, width, Color2, Color1);
            }

            pe.Graphics.DrawRectangle(new Pen(Color.Black), ClientRectangle);
            base.OnPaint(pe);
        }
        private void DrawRectangleH(PaintEventArgs pe, int top, int height, Color fromColor, Color toColor)
        {
            Rectangle rect = new Rectangle(ClientRectangle.Left, top, ClientRectangle.Width, height);
            LinearGradientBrush brush = new LinearGradientBrush(rect, fromColor, toColor, LinearGradientMode.Vertical);
            if (_blockList.Length > 0)
            {
                Rectangle[] rects = GetRectanglesH(top, height);
                if (rects.Length > 0) pe.Graphics.FillRectangles(brush, rects); //SystemBrushes.Control
            }
        }
        private void DrawRectangleV(PaintEventArgs pe, int left, int width, Color fromColor, Color toColor)
        {
            Rectangle rect = new Rectangle(left, ClientRectangle.Top, width, ClientRectangle.Height);
            LinearGradientBrush brush = new LinearGradientBrush(rect, fromColor, toColor, LinearGradientMode.Horizontal);
            if (_blockList.Length > 0)
            {
                Rectangle[] rects = GetRectanglesV(left, width);
                if (rects.Length > 0) pe.Graphics.FillRectangles(brush, rects); //SystemBrushes.Control
            }
        }
        private Rectangle[] GetRectanglesH(int top, int height)
        {
            List<Rectangle> rects = new List<Rectangle>();
            float xf = 0, wf = 0, pf = 1;
            int x = 0, y = top, w = 0, h = height;

            pf = (float)this.Width / (float)_blockList.Length;
            //h = this.Height;

            foreach (Block block in _blockList.List)
            {
                if (block.PercentProgress > 0)
                {
                    x = Convert.ToInt32(xf);
                    wf = (pf * (block.BlockSize * block.PercentProgress / 100)) + xf - x;
                    w = Convert.ToInt32(wf);

                    rects.Add(new Rectangle(x, y, w, h));
                }

                xf += pf * block.BlockSize;
            }
            return rects.ToArray();
        }

        private Rectangle[] GetRectanglesV(int left, int width)
        {
            List<Rectangle> rects = new List<Rectangle>();
            float yf = 0, hf = 0, pf = 1;
            int x = left, y = 0, w = width, h = 0;

            pf = (float)this.Height / (float)_blockList.Length;
            //w = this.Width;

            foreach (Block block in _blockList.List)
            {
                if (block.PercentProgress > 0)
                {
                    y = Convert.ToInt32(yf);
                    hf = (pf * (block.BlockSize * block.PercentProgress / 100)) + yf - y;
                    h = Convert.ToInt32(hf);

                    rects.Add(new Rectangle(x, y, w, h));
                }

                yf += pf * block.BlockSize;
            }
            return rects.ToArray();
        }
    }
}
