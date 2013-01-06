using System;
using System.Collections.Generic;
using System.Text;

namespace MyDownloader.App.Controls
{
    [Serializable()]
    public class Block
    {
        private float _blockSize = 0;
        private float _percentProgress = 0;

        public Block(float BlockSize, float PercentProgress)
        {
            this.BlockSize = BlockSize;
            this.PercentProgress = PercentProgress;
        }

        public float BlockSize
        {
            get { return _blockSize; }
            set { _blockSize = value; }
        }

        public float PercentProgress
        {
            get { return _percentProgress; }
            set { _percentProgress = value; }
        }
    }
}
