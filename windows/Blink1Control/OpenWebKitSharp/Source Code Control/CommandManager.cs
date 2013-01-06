using System.Collections.Generic;

namespace FastColoredTextBoxNS
{
    internal class CommandManager
    {
        readonly int maxHistoryLength = 200;
        LimitedStack<UndoableCommand> history;
        Stack<UndoableCommand> redoStack = new Stack<UndoableCommand>();

        public CommandManager()
        {
            history = new LimitedStack<UndoableCommand>(maxHistoryLength);
        }

        public void ExecuteCommand(Command cmd)
        {
            if (disabledCommands > 0)
                return;

            if (cmd is UndoableCommand)
            {
                (cmd as UndoableCommand).autoUndo = autoUndoCommands > 0;
                history.Push(cmd as UndoableCommand);
            }
            cmd.Execute();
            //
            redoStack.Clear();
        }

        public void Undo()
        {
            if (history.Count > 0)
            {
                var cmd = history.Pop();
                //
                BeginDisableCommands();//prevent text changing into handlers
                try
                {
                    cmd.Undo();
                }
                finally
                {
                    EndDisableCommands();
                }
                //
                redoStack.Push(cmd);
            }

            //undo next autoUndo command
            if (history.Count > 0)
            {
                if (history.Peek().autoUndo)
                    Undo();
            }
        }

        int disabledCommands = 0;

        private void EndDisableCommands()
        {
            disabledCommands--;
        }

        private void BeginDisableCommands()
        {
            disabledCommands++;
        }

        int autoUndoCommands = 0;

        public void EndAutoUndoCommands()
        {
            autoUndoCommands--;
            if (autoUndoCommands == 0)
                if (history.Count > 0)
                    history.Peek().autoUndo = false;
        }

        public void BeginAutoUndoCommands()
        {
            autoUndoCommands++;
        }

        internal void ClearHistory()
        {
            history.Clear();
            redoStack.Clear();
        }

        internal void Redo()
        {
            if (redoStack.Count == 0)
                return;
            UndoableCommand cmd;
            BeginDisableCommands();//prevent text changing into handlers
            try
            {
                cmd = redoStack.Pop();
                cmd.tb.Selection.Start = cmd.sel.Start;
                cmd.tb.Selection.End = cmd.sel.End;
                cmd.Execute();
                history.Push(cmd);
            }
            finally
            {
                EndDisableCommands();
            }

            //redo command after autoUndoable command
            if (cmd.autoUndo)
                Redo();
        }

        public bool UndoEnabled 
        { 
            get
            {
                return history.Count>0;
            }
        }

        public bool RedoEnabled
        {
            get
            {
                return redoStack.Count > 0;
            }
        }
    }

    internal abstract class Command
    {
        internal FastColoredTextBox tb;
        public abstract void Execute();
    }

    internal abstract class UndoableCommand : Command
    {
        internal Range sel;
        internal Range lastSel;
        internal bool autoUndo;

        public UndoableCommand(FastColoredTextBox tb)
        {
            this.tb = tb;
            sel = tb.Selection.Clone();
        }

        public virtual void Undo()
        {
            OnTextChanged(true);
        }

        public override void Execute()
        {
            lastSel = tb.Selection.Clone();
            OnTextChanged(false);
        }

        protected virtual void OnTextChanged(bool invert)
        {
            bool b = sel.Start.iLine < lastSel.Start.iLine;
            if (invert)
            {
                if (b)
                    tb.OnTextChanged(sel.Start.iLine, sel.Start.iLine);
                else
                    tb.OnTextChanged(sel.Start.iLine, lastSel.Start.iLine);
            }
            else
            {
                if (b)
                    tb.OnTextChanged(sel.Start.iLine, lastSel.Start.iLine);
                else
                    tb.OnTextChanged(lastSel.Start.iLine, lastSel.Start.iLine);
            }
        }
    }
}