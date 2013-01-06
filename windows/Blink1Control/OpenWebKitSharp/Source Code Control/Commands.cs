using System;
using System.Collections.Generic;

namespace FastColoredTextBoxNS
{
    /// <summary>
    /// Insert single char
    /// </summary>
    /// <remarks>This operation includes also insertion of new line and removing char by backspace</remarks>
    internal class InsertCharCommand: UndoableCommand
    {
        char c;
        char deletedChar = '\x0';

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="tb">Underlaying textbox</param>
        /// <param name="c">Inserting char</param>
        public InsertCharCommand(FastColoredTextBox tb, char c): base(tb)
        {
            this.c = c;
        }

        /// <summary>
        /// Undo operation
        /// </summary>
        public override void Undo()
        {
            tb.OnTextChanging();
            switch (c)
            {
                case '\n': MergeLines(sel.Start.iLine, tb); break;
                case '\r': break;
                case '\b':
                    tb.Selection.Start = lastSel.Start;
                    char cc = '\x0';
                    if (deletedChar != '\x0')
                    {
                        tb.ExpandBlock(tb.Selection.Start.iLine);
                        InsertChar(deletedChar, ref cc, tb);
                    }
                    break;
                default:
                    tb.ExpandBlock(sel.Start.iLine);
                    tb[sel.Start.iLine].RemoveAt(sel.Start.iChar);
                    tb.Selection.Start = sel.Start;
                    break;
            }

            tb.needRecalc = true;

            base.Undo();
        }

        /// <summary>
        /// Execute operation
        /// </summary>
        public override void Execute()
        {
            tb.ExpandBlock(tb.Selection.Start.iLine);
            string s = c.ToString();
            tb.OnTextChanging(ref s);
            if (s.Length == 1)
                c = s[0];

            if (tb.LinesCount == 0)
                InsertLine(tb);
            InsertChar(c, ref deletedChar, tb);
            tb.needRecalc = true;
            base.Execute();
        }

        internal static void InsertChar(char c, ref char deletedChar, FastColoredTextBox tb)
        {
            switch (c)
            {
                case '\n':
                    if (tb.LinesCount == 0)
                        InsertLine(tb);
                    InsertLine(tb);
                    break;
                case '\r': break;
                case '\b'://backspace
                    if (tb.Selection.Start.iChar == 0 && tb.Selection.Start.iLine == 0)
                        return;
                    if (tb.Selection.Start.iChar == 0)
                    {
                        if (tb[tb.Selection.Start.iLine - 1].VisibleState != VisibleState.Visible)
                            tb.ExpandBlock(tb.Selection.Start.iLine - 1);
                        deletedChar = '\n';
                        MergeLines(tb.Selection.Start.iLine - 1, tb);
                    }
                    else
                    {
                        deletedChar = tb[tb.Selection.Start.iLine][tb.Selection.Start.iChar - 1].c;
                        tb[tb.Selection.Start.iLine].RemoveAt(tb.Selection.Start.iChar - 1);
                        tb.Selection.Start = new Place(tb.Selection.Start.iChar - 1, tb.Selection.Start.iLine);
                    }
                    break;
                default:
                    tb[tb.Selection.Start.iLine].Insert(tb.Selection.Start.iChar, new Char(c));
                    tb.Selection.Start = new Place(tb.Selection.Start.iChar + 1, tb.Selection.Start.iLine);
                    break;
            }
        }

        internal static void InsertLine(FastColoredTextBox tb)
        {
            if (tb.LinesCount == 0)
                tb.InsertLine(tb.Selection.Start.iLine + 1, new Line(tb.GenerateUniqueLineId()));
            else
                BreakLines(tb.Selection.Start.iLine, tb.Selection.Start.iChar, tb);

            tb.Selection.Start = new Place(0, tb.Selection.Start.iLine + 1);
            tb.needRecalc = true;
        }

        /// <summary>
        /// Merge lines i and i+1
        /// </summary>
        internal static void MergeLines(int i, FastColoredTextBox tb)
        {
            if (i + 1 >= tb.LinesCount)
                return;
            tb.ExpandBlock(i);
            tb.ExpandBlock(i + 1);
            int pos = tb[i].Count;
            //
            if(tb[i].Count == 0)
                tb.RemoveLine(i);
            else
            if (tb[i + 1].Count == 0)
                tb.RemoveLine(i + 1);
            else
            {
                tb[i].AddRange(tb[i + 1]);
                tb.RemoveLine(i + 1);
            }
            tb.Selection.Start = new Place(pos, i);
            tb.needRecalc = true;
        }

        internal static void BreakLines(int iLine, int pos, FastColoredTextBox tb)
        {
            Line newLine = new Line(tb.GenerateUniqueLineId());
            for(int i=pos;i<tb[iLine].Count;i++)
                newLine.Add(tb[iLine][i]);
            tb[iLine].RemoveRange(pos, tb[iLine].Count - pos);
            tb.InsertLine(iLine+1, newLine);
        }
    }

    /// <summary>
    /// Insert text
    /// </summary>
    internal class InsertTextCommand : UndoableCommand
    {
        string insertedText;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="tb">Underlaying textbox</param>
        /// <param name="insertedText">Text for inserting</param>
        public InsertTextCommand(FastColoredTextBox tb, string insertedText): base(tb)
        {
            this.insertedText = insertedText;
        }

        /// <summary>
        /// Undo operation
        /// </summary>
        public override void Undo()
        {
            tb.Selection.Start = sel.Start;
            tb.Selection.End = lastSel.Start;
            tb.OnTextChanging();
            ClearSelectedCommand.ClearSelected(tb);
            base.Undo();
        }

        /// <summary>
        /// Execute operation
        /// </summary>
        public override void Execute()
        {
            tb.OnTextChanging(ref insertedText);
            InsertText(insertedText, tb);
            base.Execute();
        }

        internal static void InsertText(string insertedText, FastColoredTextBox tb)
        {
            try
            {
                tb.Selection.BeginUpdate();
                char cc = '\x0';
                if (tb.LinesCount == 0)
                    InsertCharCommand.InsertLine(tb);
                tb.ExpandBlock(tb.Selection.Start.iLine);
                foreach (char c in insertedText)
                    InsertCharCommand.InsertChar(c, ref cc, tb);
                tb.needRecalc = true;
            }
            finally {
                tb.Selection.EndUpdate();
            }
        }
    }

    /// <summary>
    /// Insert text into given ranges
    /// </summary>
    /// <remarks>This opertaion is very heavy, because it save ALL text for undo.
    /// Use this class only for ReplaceAll command</remarks>
    internal class ReplaceTextCommand : UndoableCommand
    {
        string insertedText;
        List<Range> ranges;
        string prevText;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="tb">Underlaying textbox</param>
        /// <param name="ranges">List of ranges for replace</param>
        /// <param name="insertedText">Text for inserting</param>
        public ReplaceTextCommand(FastColoredTextBox tb, List<Range> ranges, string insertedText)
            : base(tb)
        {
            this.ranges = ranges;
            this.insertedText = insertedText;
            sel = tb.Selection.Clone();
            sel.SelectAll();
        }

        /// <summary>
        /// Undo operation
        /// </summary>
        public override void Undo()
        {
            tb.Text = prevText;
        }

        /// <summary>
        /// Execute operation
        /// </summary>
        public override void Execute()
        {
            tb.OnTextChanging(ref insertedText);

            this.prevText = tb.Text;

            tb.Selection.BeginUpdate();
            for (int i = ranges.Count - 1; i >= 0; i--)
            {
                tb.Selection.Start = ranges[i].Start;
                tb.Selection.End = ranges[i].End;
                ClearSelectedCommand.ClearSelected(tb);
                InsertTextCommand.InsertText(insertedText, tb);
            }
            tb.Selection.SelectAll();
            tb.Selection.EndUpdate();
            tb.needRecalc = true;

            lastSel = tb.Selection.Clone();
            tb.OnTextChanged(lastSel.Start.iLine, lastSel.End.iLine);
            //base.Execute();
        }
    }

    /// <summary>
    /// Clear selected text
    /// </summary>
    internal class ClearSelectedCommand : UndoableCommand
    {
        string deletedText;

        /// <summary>
        /// Construstor
        /// </summary>
        /// <param name="tb">Underlaying textbox</param>
        public ClearSelectedCommand(FastColoredTextBox tb): base(tb)
        {
        }

        /// <summary>
        /// Undo operation
        /// </summary>
        public override void Undo()
        {
            tb.Selection.Start = new Place(sel.FromX, Math.Min(sel.Start.iLine, sel.End.iLine));
            tb.OnTextChanging();
            InsertTextCommand.InsertText(deletedText, tb);
            tb.OnTextChanged(sel.Start.iLine, sel.End.iLine);
        }

        /// <summary>
        /// Execute operation
        /// </summary>
        public override void Execute()
        {
            tb.OnTextChanging();
            deletedText = tb.Selection.Text;
            ClearSelected(tb);
            lastSel = tb.Selection.Clone();
            tb.OnTextChanged(lastSel.Start.iLine, lastSel.Start.iLine);
        }

        internal static void ClearSelected(FastColoredTextBox tb)
        {
            Place start = tb.Selection.Start;
            Place end = tb.Selection.End;
            int fromLine = Math.Min(end.iLine, start.iLine);
            int toLine = Math.Max(end.iLine, start.iLine);
            int fromChar = tb.Selection.FromX;
            int toChar = tb.Selection.ToX;
            if (fromLine < 0) return;
            //
            if (fromLine == toLine)
                tb[fromLine].RemoveRange(fromChar, toChar - fromChar);
            else
            {
                tb[fromLine].RemoveRange(fromChar, tb[fromLine].Count - fromChar);
                tb[toLine].RemoveRange(0, toChar);
                tb.RemoveLine(fromLine + 1, toLine - fromLine - 1);
                InsertCharCommand.MergeLines(fromLine, tb);
            }
            //
            tb.Selection.Start = new Place(fromChar, fromLine);
            //
            tb.needRecalc = true;
        }
    }
}
