using System;
using System.Collections.Generic;
using System.Text;

namespace MyDownloader.App.Controls
{
    [Serializable()]
    public class BlockList
    {
        private int _length = 0;
        private List<Block> _blockList;

        public BlockList()
        {
            _blockList = new List<Block>();
        }

        public enum UpdateMode
        {
            All,
            FilledSegments,
            FullListSegment,
            BlockList
        }

        /// <summary>
        /// Update mode of segments
        /// </summary>
        public UpdateMode Update = UpdateMode.All;

        /// <summary>
        /// Change quantity of segments
        /// </summary>
        public int Length
        {
            get { return _length; }
            set
            {
                if (_length != value && value > 0)
                {
                    bool[] bools = FullListSegment;
                    bool[] bools2 = new bool[value];
                    for (int i = 0; i < bools.Length; i++)
                        bools2[i] = bools[i];
                    FullListSegment = bools2;
                }
            }
        }
        /// <summary>
        /// Get or set filled segments 
        /// </summary>
        public int[] FilledSegments
        {
            get
            {
                bool[] bools = FullListSegment;
                List<int> filled = new List<int>(); ;
                for (int i = 0; i < bools.Length; i++)
                    if (bools[i]) filled.Add(i);
                return filled.ToArray();
            }
            set
            {
                if (Update != UpdateMode.All && Update != UpdateMode.FilledSegments)
                    throw new InvalidOperationException();
                if (value != null)
                    if (value.Length > 0)
                    {
                        bool[] bools = FullListSegment;
                        for (int i = 0; i < value.Length; i++)
                            bools.SetValue(true, value[i]);
                        FullListSegment = bools;
                    }
            }
        }
        /// <summary>
        /// Get or sets the full list of segments
        /// </summary>
        public bool[] FullListSegment
        {
            get
            {
                int size_anterior = 0;
                bool[] bools = new bool[_length];
                if (bools.Length > 0)
                {
                    foreach (Block block in _blockList)
                    {
                        for (int i = 0; i < Convert.ToInt32(block.BlockSize * block.PercentProgress / 100); i++)
                            bools.SetValue(true, i + size_anterior);
                        size_anterior += Convert.ToInt32(block.BlockSize);
                    }
                }
                return bools;
            }
            set
            {
                if (Update != UpdateMode.All && Update != UpdateMode.FullListSegment)
                    throw new InvalidOperationException();

                bool b_old = false;
                int qtd = 0;
                int filled = 0;
                float percent = 0;
                if (value != null)
                {
                    if (value.Length > 0)
                    {
                        _blockList.Clear();
                        foreach (bool b in value)
                        {
                            if (b == b_old)
                                qtd++;
                            else
                            {
                                if (b_old)
                                    filled = qtd;
                                else if (filled + qtd > 0)
                                {
                                    percent = (float)filled / (float)(filled + qtd) * (float)100;
                                    _blockList.Add(new Block(filled + qtd, percent));
                                }
                                qtd = 1;
                                b_old = b;
                            }
                        }
                        if (filled + qtd > 0)
                        {
                            percent = (float)filled / (float)(filled + qtd) * (float)100;
                            _blockList.Add(new Block(filled + qtd, percent));
                        }
                        _length = value.Length;
                    }
                }
                else
                {
                    _length = 0;
                }
            }
        }
        /// <summary>
        /// Get or set the block list of segments
        /// </summary>
        public List<Block> List
        {
            get
            {
                return _blockList;
            }
            set
            {
                if (Update != UpdateMode.All && Update != UpdateMode.BlockList)
                    throw new InvalidOperationException();
                float size = 0;
                _blockList = value;
                if (_blockList != null)
                    foreach (Block block in _blockList)
                        size += block.BlockSize;
                _length = Convert.ToInt32(size);
            }
        }
    }
}