using System;
using System.Text;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Collections.Generic;

namespace FastColoredTextBoxNS
{
    /// <summary>
    /// Diapason of text chars
    /// </summary>
    public class Range : IEnumerable<Place>
    {
        Place start;
        Place end;
        public readonly FastColoredTextBox tb;
        int preferedPos = -1;
        int updating = 0;

        string cachedText;
        List<Place> cachedCharIndexToPlace;
        int cachedTextVersion = -1;

        /// <summary>
        /// Constructor
        /// </summary>
        public Range(FastColoredTextBox tb)
        {
            this.tb = tb;
        }

        /// <summary>
        /// Constructor
        /// </summary>
        public Range(FastColoredTextBox tb, int iStartChar, int iStartLine, int iEndChar, int iEndLine)
            : this(tb)
        {
            start = new Place(iStartChar, iStartLine);
            end = new Place(iEndChar, iEndLine);
        }

        /// <summary>
        /// Constructor
        /// </summary>
        public Range(FastColoredTextBox tb, Place start, Place end)
            : this(tb)
        {
            this.start = start;
            this.end = end;
        }

        public bool Contains(Place place)
        {
            if (place.iLine < Math.Min(start.iLine, end.iLine)) return false;
            if (place.iLine > Math.Max(start.iLine, end.iLine)) return false;

            Place s = start;
            Place e = end;

            if (s.iLine > e.iLine || (s.iLine == e.iLine && s.iChar > e.iChar))
            {
                var temp = s;
                s = e;
                e = temp;
            }

            if (place.iLine == s.iLine && place.iChar < s.iChar) return false;
            if (place.iLine == e.iLine && place.iChar > e.iChar) return false;

            return true;
        }

        /// <summary>
        /// Returns intersection with other range,
        /// empty range returned otherwise
        /// </summary>
        /// <param name="range"></param>
        /// <returns></returns>
        public Range GetIntersectionWith(Range range)
        {
            Range r1 = this.Clone();
            Range r2 = range.Clone();
            r1.Normalize();
            r2.Normalize();
            Place newStart = r1.Start > r2.Start ? r1.Start : r2.Start;
            Place newEnd = r1.End < r2.End ? r1.End : r2.End;
            if (newEnd < newStart) 
                return new Range(tb, start, start);
            return tb.GetRange(newStart, newEnd);
        }

        /// <summary>
        /// Returns union with other range.
        /// </summary>
        /// <param name="range"></param>
        /// <returns></returns>
        public Range GetUnionWith(Range range)
        {
            Range r1 = this.Clone();
            Range r2 = range.Clone();
            r1.Normalize();
            r2.Normalize();
            Place newStart = r1.Start < r2.Start ? r1.Start : r2.Start;
            Place newEnd = r1.End > r2.End ? r1.End : r2.End;

            return tb.GetRange(newStart, newEnd);
        }

        /// <summary>
        /// Select all chars of control
        /// </summary>
        public void SelectAll()
        {
            Start = new Place(0, 0);
            if (tb.LinesCount == 0)
                Start = new Place(0, 0);
            else
            {
                end = new Place(0, 0);
                start = new Place(tb[tb.LinesCount - 1].Count, tb.LinesCount - 1);
            }
            if (this == tb.Selection)
                tb.Invalidate();
        }

        /// <summary>
        /// Start line and char position
        /// </summary>
        public Place Start
        {
            get { return start; }
            set
            {
                end = start = value;
                preferedPos = -1;

                OnSelectionChanged();
            }
        }

        /// <summary>
        /// Finish line and char position
        /// </summary>
        public Place End
        {
            get
            {
                return end;
            }
            set
            {
                end = value;
                OnSelectionChanged();
            }
        }

        /// <summary>
        /// Text of range
        /// </summary>
        /// <remarks>This property has not 'set' accessor because undo/redo stack works only with 
        /// FastColoredTextBox.Selection range. So, if you want to set text, you need to use FastColoredTextBox.Selection
        /// and FastColoredTextBox.InsertText() mehtod.
        /// </remarks>
        public string Text
        {
            get
            {
                int fromLine = Math.Min(end.iLine, start.iLine);
                int toLine = Math.Max(end.iLine, start.iLine);
                int fromChar = FromX;
                int toChar = ToX;
                if (fromLine < 0) return null;
                //
                StringBuilder sb = new StringBuilder();
                for (int y = fromLine; y <= toLine; y++)
                {
                    int fromX = y == fromLine ? fromChar : 0;
                    int toX = y == toLine ? toChar - 1 : tb[y].Count - 1;
                    for (int x = fromX; x <= toX; x++)
                        sb.Append(tb[y][x].c);
                    if (y != toLine && fromLine != toLine)
                        sb.AppendLine();
                }
                return sb.ToString();
            }
        }

        internal void GetText(out string text, out List<Place> charIndexToPlace)
        {
            //try get cached text
            if (tb.TextVersion == cachedTextVersion)
            {
                text = cachedText;
                charIndexToPlace = cachedCharIndexToPlace;
                return;
            }
            //
            int fromLine = Math.Min(end.iLine, start.iLine);
            int toLine = Math.Max(end.iLine, start.iLine);
            int fromChar = FromX;
            int toChar = ToX;

            StringBuilder sb = new StringBuilder((toLine - fromLine)*100);
            charIndexToPlace = new List<Place>(sb.Capacity);
            if (fromLine >= 0)
            {
                for (int y = fromLine; y <= toLine; y++)
                {
                    int fromX = y == fromLine ? fromChar : 0;
                    int toX = y == toLine ? toChar - 1 : tb[y].Count - 1;
                    for (int x = fromX; x <= toX; x++)
                    {
                        sb.Append(tb[y][x].c);
                        charIndexToPlace.Add(new Place(x, y));
                    }
                    if (y != toLine && fromLine != toLine)
                    foreach (char c in Environment.NewLine)
                    {
                        sb.Append(c);
                        charIndexToPlace.Add(new Place(tb[y].Count/*???*/, y));
                    }
                }
            }
            text = sb.ToString();
            charIndexToPlace.Add(End > Start ? End : Start);
            //caching
            cachedText = text;
            cachedCharIndexToPlace = charIndexToPlace;
            cachedTextVersion = tb.TextVersion;
        }

        /// <summary>
        /// Returns first char after Start place
        /// </summary>
        public char CharAfterStart
        {
            get
            {
                if (Start.iChar >= tb[Start.iLine].Count)
                    return '\n';
                else
                    return tb[Start.iLine][Start.iChar].c;
            }
        }

        /// <summary>
        /// Returns first char before Start place
        /// </summary>
        public char CharBeforeStart
        {
            get
            {
                if (Start.iChar <= 0)
                    return '\n';
                else
                    return tb[Start.iLine][Start.iChar - 1].c;
            }
        }

        /// <summary>
        /// Clone range
        /// </summary>
        /// <returns></returns>
        public Range Clone()
        {
            return (Range)MemberwiseClone();
        }

        /// <summary>
        /// Return minimum of end.X and start.X
        /// </summary>
        internal int FromX
        {
            get
            {
                if (end.iLine < start.iLine) return end.iChar;
                if (end.iLine > start.iLine) return start.iChar;
                return Math.Min(end.iChar, start.iChar);
            }
        }

        /// <summary>
        /// Return maximum of end.X and start.X
        /// </summary>
        internal int ToX
        {
            get
            {
                if (end.iLine < start.iLine) return start.iChar;
                if (end.iLine > start.iLine) return end.iChar;
                return Math.Max(end.iChar, start.iChar);
            }
        }

        /// <summary>
        /// Move range right
        /// </summary>
        /// <remarks>This method jump over folded blocks</remarks>
        public bool GoRight()
        {
            Place prevStart = start;
            GoRight(false);
            return prevStart != start;
        }

        /// <summary>
        /// Move range left
        /// </summary>
        /// <remarks>This method can to go inside folded blocks</remarks>
        public bool GoRightThroughFolded()
        {
            if (start.iLine >= tb.LinesCount - 1 && start.iChar >= tb[tb.LinesCount - 1].Count)
                return false;

            if (start.iChar < tb[start.iLine].Count)
                start.Offset(1, 0);
            else
                start = new Place(0, start.iLine + 1);

            preferedPos = -1;
            end = start;
            OnSelectionChanged();
            return true;
        }

        /// <summary>
        /// Move range left
        /// </summary>
        /// <remarks>This method jump over folded blocks</remarks>
        public bool GoLeft()
        {
            Place prevStart = start;
            GoLeft(false);
            return prevStart != start;
        }

        /// <summary>
        /// Move range left
        /// </summary>
        /// <remarks>This method can to go inside folded blocks</remarks>
        public bool GoLeftThroughFolded()
        {
            if (start.iChar == 0 && start.iLine == 0)
                return false;

            if (start.iChar > 0)
                start.Offset(-1, 0);
            else
                start = new Place(tb[start.iLine - 1].Count, start.iLine - 1);

            preferedPos = -1;
            end = start;
            OnSelectionChanged();
            return true;
        }

        public void GoLeft(bool shift)
        {
            if (start.iChar != 0 || start.iLine != 0)
            {
                if (start.iChar > 0 && tb[start.iLine].VisibleState == VisibleState.Visible)
                    start.Offset(-1, 0);
                else
                {
                    int i = tb.FindPrevVisibleLine(start.iLine);
                    if (i == start.iLine) return;
                    start = new Place(tb[i].Count, i);
                }
            }

            if (!shift)
                end = start;

            OnSelectionChanged();

            preferedPos = -1;
        }

        public void GoRight(bool shift)
        {
            if (start.iLine < tb.LinesCount - 1 || start.iChar < tb[tb.LinesCount - 1].Count)
            {
                if (start.iChar < tb[start.iLine].Count && tb[start.iLine].VisibleState == VisibleState.Visible)
                    start.Offset(1, 0);
                else
                {
                    int i = tb.FindNextVisibleLine(start.iLine);
                    if (i == start.iLine) return;
                    start = new Place(0, i);
                }
            }

            if (!shift)
                end = start;

            OnSelectionChanged();

            preferedPos = -1;
        }

        internal void GoUp(bool shift)
        {
            if (preferedPos < 0)
                preferedPos = start.iChar - tb[start.iLine].GetWordWrapStringStartPosition(tb[start.iLine].GetWordWrapStringIndex(start.iChar));

            int iWW = tb[start.iLine].GetWordWrapStringIndex(start.iChar);
            if (iWW == 0)
            {
                if (start.iLine <= 0) return;
                int i = tb.FindPrevVisibleLine(start.iLine);
                if (i == start.iLine) return;
                start.iLine = i;
                iWW = tb[start.iLine].WordWrapStringsCount;
            }

            if (iWW > 0)
            {
                int finish = tb[start.iLine].GetWordWrapStringFinishPosition(iWW - 1);
                start.iChar = tb[start.iLine].GetWordWrapStringStartPosition(iWW - 1) + preferedPos;
                if (start.iChar > finish + 1)
                    start.iChar = finish + 1;
            }

            if (!shift)
                end = start;

            OnSelectionChanged();
        }

        internal void GoPageUp(bool shift)
        {
            if (preferedPos < 0)
                preferedPos = start.iChar - tb[start.iLine].GetWordWrapStringStartPosition(tb[start.iLine].GetWordWrapStringIndex(start.iChar));

            int pageHeight = tb.ClientRectangle.Height / tb.CharHeight - 1;

            for (int i = 0; i < pageHeight; i++)
            {
                int iWW = tb[start.iLine].GetWordWrapStringIndex(start.iChar);
                if (iWW == 0)
                {
                    if (start.iLine <= 0) break;
                    //pass hidden
                    int newLine = tb.FindPrevVisibleLine(start.iLine);
                    if (newLine == start.iLine) break;
                    start.iLine = newLine;
                    iWW = tb[start.iLine].WordWrapStringsCount;
                }

                if (iWW > 0)
                {
                    int finish = tb[start.iLine].GetWordWrapStringFinishPosition(iWW - 1);
                    start.iChar = tb[start.iLine].GetWordWrapStringStartPosition(iWW - 1) + preferedPos;
                    if (start.iChar > finish + 1)
                        start.iChar = finish + 1;
                }
            }

            if (!shift)
                end = start;

            OnSelectionChanged();
        }

        internal void GoDown(bool shift)
        {
            if (preferedPos < 0)
                preferedPos = start.iChar - tb[start.iLine].GetWordWrapStringStartPosition(tb[start.iLine].GetWordWrapStringIndex(start.iChar));

            int iWW = tb[start.iLine].GetWordWrapStringIndex(start.iChar);
            if (iWW >= tb[start.iLine].WordWrapStringsCount - 1)
            {
                if (start.iLine >= tb.LinesCount - 1) return;
                //pass hidden
                int i = tb.FindNextVisibleLine(start.iLine);
                if (i == start.iLine) return;
                start.iLine = i;
                iWW = -1;
            }

            if (iWW < tb[start.iLine].WordWrapStringsCount - 1)
            {
                int finish = tb[start.iLine].GetWordWrapStringFinishPosition(iWW + 1);
                start.iChar = tb[start.iLine].GetWordWrapStringStartPosition(iWW + 1) + preferedPos;
                if (start.iChar > finish + 1)
                    start.iChar = finish + 1;
            }

            if (!shift)
                end = start;

            OnSelectionChanged();
        }

        internal void GoPageDown(bool shift)
        {
            if (preferedPos < 0)
                preferedPos = start.iChar - tb[start.iLine].GetWordWrapStringStartPosition(tb[start.iLine].GetWordWrapStringIndex(start.iChar));

            int pageHeight = tb.ClientRectangle.Height / tb.CharHeight - 1;

            for (int i = 0; i < pageHeight; i++)
            {
                int iWW = tb[start.iLine].GetWordWrapStringIndex(start.iChar);
                if (iWW >= tb[start.iLine].WordWrapStringsCount - 1)
                {
                    if (start.iLine >= tb.LinesCount - 1) break;
                    //pass hidden
                    int newLine = tb.FindNextVisibleLine(start.iLine);
                    if (newLine == start.iLine) break;
                    start.iLine = newLine;
                    iWW = -1;
                }

                if (iWW < tb[start.iLine].WordWrapStringsCount - 1)
                {
                    int finish = tb[start.iLine].GetWordWrapStringFinishPosition(iWW + 1);
                    start.iChar = tb[start.iLine].GetWordWrapStringStartPosition(iWW + 1) + preferedPos;
                    if (start.iChar > finish + 1)
                        start.iChar = finish + 1;
                }
            }

            if (!shift)
                end = start;

            OnSelectionChanged();
        }

        internal void GoHome(bool shift)
        {
            if (start.iLine < 0)
                return;

            if (tb[start.iLine].VisibleState != VisibleState.Visible)
                return;

            start = new Place(0, start.iLine);

            if (!shift)
                end = start;

            OnSelectionChanged();

            preferedPos = -1;
        }

        internal void GoEnd(bool shift)
        {
            if (start.iLine < 0)
                return;
            if (tb[start.iLine].VisibleState != VisibleState.Visible)
                return;

            start = new Place(tb[start.iLine].Count, start.iLine);

            if (!shift)
                end = start;

            OnSelectionChanged();

            preferedPos = -1;
        }

        /// <summary>
        /// Set style for range
        /// </summary>
        public void SetStyle(Style style)
        {
            //search code for style
            int code = tb.GetOrSetStyleLayerIndex(style);
            //set code to chars
            SetStyle(ToStyleIndex(code));
            //
            tb.Invalidate();
        }

        /// <summary>
        /// Set style for given regex pattern
        /// </summary>
        public void SetStyle(Style style, string regexPattern)
        {
            //search code for style
            StyleIndex layer = ToStyleIndex(tb.GetOrSetStyleLayerIndex(style));
            SetStyle(layer, regexPattern, RegexOptions.None);
        }

        /// <summary>
        /// Set style for given regex
        /// </summary>
        public void SetStyle(Style style, Regex regex)
        {
            //search code for style
            StyleIndex layer = ToStyleIndex(tb.GetOrSetStyleLayerIndex(style));
            SetStyle(layer, regex);
        }


        /// <summary>
        /// Set style for given regex pattern
        /// </summary>
        public void SetStyle(Style style, string regexPattern, RegexOptions options)
        {
            //search code for style
            StyleIndex layer = ToStyleIndex(tb.GetOrSetStyleLayerIndex(style));
            SetStyle(layer, regexPattern, options);
        }

        /// <summary>
        /// Set style for given regex pattern
        /// </summary>
        public void SetStyle(StyleIndex styleLayer, string regexPattern, RegexOptions options)
        {
            if (Math.Abs(Start.iLine - End.iLine) > 1000)
                options |= RegexOptions.Compiled;
            //
            foreach (var range in GetRanges(regexPattern, options))
                range.SetStyle(styleLayer);
            //
            tb.Invalidate();
        }

        /// <summary>
        /// Set style for given regex pattern
        /// </summary>
        public void SetStyle(StyleIndex styleLayer, Regex regex)
        {
            foreach (var range in GetRanges(regex))
                range.SetStyle(styleLayer);
            //
            tb.Invalidate();
        }

        /// <summary>
        /// Appends style to chars of range
        /// </summary>
        public void SetStyle(StyleIndex styleIndex)
        {
            //set code to chars
            int fromLine = Math.Min(End.iLine, Start.iLine);
            int toLine = Math.Max(End.iLine, Start.iLine);
            int fromChar = FromX;
            int toChar = ToX;
            if (fromLine < 0) return;
            //
            for (int y = fromLine; y <= toLine; y++)
            {
                int fromX = y == fromLine ? fromChar : 0;
                int toX = y == toLine ? toChar - 1 : tb[y].Count - 1;
                for (int x = fromX; x <= toX; x++)
                {
                    Char c = tb[y][x];
                    c.style |= styleIndex;
                    tb[y][x] = c;
                }
            }
        }

        /// <summary>
        /// Sets folding markers
        /// </summary>
        /// <param name="startFoldingPattern">Pattern for start folding line</param>
        /// <param name="finishFoldingPattern">Pattern for finish folding line</param>
        public void SetFoldingMarkers(string startFoldingPattern, string finishFoldingPattern)
        {
            SetFoldingMarkers(startFoldingPattern, finishFoldingPattern, RegexOptions.Compiled);
        }

        /// <summary>
        /// Sets folding markers
        /// </summary>
        /// <param name="startFoldingPattern">Pattern for start folding line</param>
        /// <param name="finishFoldingPattern">Pattern for finish folding line</param>
        public void SetFoldingMarkers(string startFoldingPattern, string finishFoldingPattern, RegexOptions options)
        {
            foreach (var range in GetRanges(startFoldingPattern, options))
                tb[range.Start.iLine].FoldingStartMarker = startFoldingPattern;

            foreach (var range in GetRanges(finishFoldingPattern, options))
                tb[range.Start.iLine].FoldingEndMarker = startFoldingPattern;
            //
            tb.Invalidate();
        }
        /// <summary>
        /// Finds ranges for given regex pattern
        /// </summary>
        /// <param name="regexPattern">Regex pattern</param>
        /// <returns>Enumeration of ranges</returns>
        public IEnumerable<Range> GetRanges(string regexPattern)
        {
            return GetRanges(regexPattern, RegexOptions.None);
        }

        /// <summary>
        /// Finds ranges for given regex pattern
        /// </summary>
        /// <param name="regexPattern">Regex pattern</param>
        /// <returns>Enumeration of ranges</returns>
        public IEnumerable<Range> GetRanges(string regexPattern, RegexOptions options)
        {
            //get text
            string text;
            List<Place> charIndexToPlace;
            GetText(out text, out charIndexToPlace);
            //create regex
            Regex regex = new Regex(regexPattern, options);
            //
            foreach (Match m in regex.Matches(text))
            {
                Range r = new Range(this.tb);
                //try get 'range' group, otherwise use group 0
                Group group = m.Groups["range"];
                if (!group.Success)
                    group = m.Groups[0];
                //
                r.Start = charIndexToPlace[group.Index];
                r.End = charIndexToPlace[group.Index + group.Length];
                yield return r;
            }
        }

        /// <summary>
        /// Finds ranges for given regex
        /// </summary>
        /// <returns>Enumeration of ranges</returns>
        public IEnumerable<Range> GetRanges(Regex regex)
        {
            //get text
            string text;
            List<Place> charIndexToPlace;
            GetText(out text, out charIndexToPlace);
            //
            foreach (Match m in regex.Matches(text))
            {
                Range r = new Range(this.tb);
                //try get 'range' group, otherwise use group 0
                Group group = m.Groups["range"];
                if (!group.Success)
                    group = m.Groups[0];
                //
                r.Start = charIndexToPlace[group.Index];
                r.End = charIndexToPlace[group.Index + group.Length];
                yield return r;
            }
        }

        /// <summary>
        /// Clear styles of range
        /// </summary>
        public void ClearStyle(params Style[] styles)
        {
            try
            {
               ClearStyle(tb.GetStyleIndexMask(styles));
            }
            catch { ;}
        }

        /// <summary>
        /// Clear styles of range
        /// </summary>
        public void ClearStyle(StyleIndex styleIndex)
        {
            //set code to chars
            int fromLine = Math.Min(End.iLine, Start.iLine);
            int toLine = Math.Max(End.iLine, Start.iLine);
            int fromChar = FromX;
            int toChar = ToX;
            if (fromLine < 0) return;
            //
            for (int y = fromLine; y <= toLine; y++)
            {
                int fromX = y == fromLine ? fromChar : 0;
                int toX = y == toLine ? toChar - 1 : tb[y].Count - 1;
                for (int x = fromX; x <= toX; x++)
                {
                    Char c = tb[y][x];
                    c.style &= ~styleIndex;
                    tb[y][x] = c;
                }
            }
            //
            tb.Invalidate();
        }

        /// <summary>
        /// Clear folding markers of all lines of range
        /// </summary>
        public void ClearFoldingMarkers()
        {
            //set code to chars
            int fromLine = Math.Min(End.iLine, Start.iLine);
            int toLine = Math.Max(End.iLine, Start.iLine);
            if (fromLine < 0) return;
            //
            for (int y = fromLine; y <= toLine; y++)
                tb[y].ClearFoldingMarkers();
            //
            tb.Invalidate();
        }

        void OnSelectionChanged()
        {
            //clear cache
            cachedTextVersion = -1;
            cachedText = null;
            cachedCharIndexToPlace = null;
            //
            if (tb.Selection == this)
                if (updating == 0)
                    tb.OnSelectionChanged();
        }

        /// <summary>
        /// Starts selection position updating
        /// </summary>
        public void BeginUpdate()
        {
            updating++;
        }

        /// <summary>
        /// Ends selection position updating
        /// </summary>
        public void EndUpdate()
        {
            updating--;
            if (updating == 0)
                OnSelectionChanged();
        }

        public override string ToString()
        {
            return "Start: " + Start + " End: " + End;
        }

        /// <summary>
        /// Exchanges Start and End if End appears before Start
        /// </summary>
        public void Normalize()
        {
            if (Start > End)
                Inverse();
        }

        /// <summary>
        /// Exchanges Start and End
        /// </summary>
        public void Inverse()
        {
            var temp = start;
            start = end;
            end = temp;
        }

        /// <summary>
        /// Expands range from first char of Start line to last char of End line
        /// </summary>
        public void Expand()
        {
            Normalize();
            start = new Place(0, start.iLine);
            end = new Place(tb.GetLineLength(end.iLine), end.iLine);
        }

        IEnumerator<Place> IEnumerable<Place>.GetEnumerator()
        {
            int fromLine = Math.Min(end.iLine, start.iLine);
            int toLine = Math.Max(end.iLine, start.iLine);
            int fromChar = FromX;
            int toChar = ToX;
            if (fromLine < 0) yield break;
            //
            for (int y = fromLine; y <= toLine; y++)
            {
                int fromX = y == fromLine ? fromChar : 0;
                int toX = y == toLine ? toChar - 1 : tb[y].Count - 1;
                for (int x = fromX; x <= toX; x++)
                    yield return new Place(x, y);
            }
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return (this as IEnumerable<Place>).GetEnumerator();
        }

        /// <summary>
        /// Get fragment of text around Start place. Returns maximal mathed to pattern fragment.
        /// </summary>
        /// <param name="allowedSymbolsPattern">Allowed chars pattern for fragment</param>
        /// <returns>Range of found fragment</returns>
        public Range GetFragment(string allowedSymbolsPattern)
        {
            return GetFragment(allowedSymbolsPattern, RegexOptions.None);
        }

        /// <summary>
        /// Get fragment of text around Start place. Returns maximal mathed to pattern fragment.
        /// </summary>
        /// <param name="allowedSymbolsPattern">Allowed chars pattern for fragment</param>
        /// <returns>Range of found fragment</returns>
        public Range GetFragment(string allowedSymbolsPattern, RegexOptions options)
        {
            Range r = new Range(tb);
            r.Start = Start;
            Regex regex = new Regex(allowedSymbolsPattern, options);
            //go left, check symbols
            while (r.GoLeftThroughFolded())
            {
                if (!regex.IsMatch(r.CharAfterStart.ToString()))
                {
                    r.GoRightThroughFolded();
                    break;
                }
            }
            Place startFragment = r.Start;

            r.Start = Start;
            //go right, check symbols
            do
            {
                if (!regex.IsMatch(r.CharAfterStart.ToString()))
                    break;
            } while (r.GoRightThroughFolded()) ;
            Place endFragment = r.Start;

            return new Range(tb, startFragment, endFragment);
        }

        bool IsIdentifierChar(char c)
        {
            return char.IsLetterOrDigit(c) || c == '_';
        }

        internal void GoWordLeft(bool shift)
        {
            Range range = this.Clone();//for OnSelectionChanged disable

            Place prev;
            bool findIdentifier = IsIdentifierChar(range.CharBeforeStart);

            do{
                prev = range.Start;
                if (IsIdentifierChar(range.CharBeforeStart) ^ findIdentifier)
                    break;

                //move left
                range.GoLeft(shift);
            } while (prev != range.Start);

            this.Start = range.Start;
            this.End = range.End;

            if (tb[Start.iLine].VisibleState != VisibleState.Visible)
                GoRight(shift);
        }

        internal void GoWordRight(bool shift)
        {
            Range range = this.Clone();//for OnSelectionChanged disable

            Place prev;
            bool findIdentifier = IsIdentifierChar(range.CharAfterStart);

            do
            {
                prev = range.Start;
                if (IsIdentifierChar(range.CharAfterStart) ^ findIdentifier)
                    break;

                //move right
                range.GoRight(shift);
            } while (prev != range.Start);

            this.Start = range.Start;
            this.End = range.End;

            if (tb[Start.iLine].VisibleState != VisibleState.Visible)
                GoLeft(shift);
        }

        internal void GoFirst(bool shift)
        {
            start = new Place(0, 0);
            if (tb[Start.iLine].VisibleState != VisibleState.Visible)
                GoRight(shift);
            if(!shift)
                end = start;

            OnSelectionChanged();
        }

        internal void GoLast(bool shift)
        {
            start = new Place(tb[tb.LinesCount - 1].Count, tb.LinesCount-1);
            if (tb[Start.iLine].VisibleState != VisibleState.Visible)
                GoLeft(shift);
            if (!shift)
                end = start;

            OnSelectionChanged();
        }

        public static StyleIndex ToStyleIndex(int i)
        {
            return (StyleIndex)(1 << i);
        }
    }
}
