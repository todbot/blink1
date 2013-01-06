using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace MyDownloader.App.Controls
{
    public class CheckableTreeView: TreeView
    {
        private static bool IsSomeChildNodeChecked(TreeNode node)
        {
            for (int i = 0; i < node.Nodes.Count; i++)
            {
                if (node.Nodes[i].Checked)
                {
                    return true;
                }
            }

            return false;
        }

        private static void CheckAllChildren(TreeNode node, bool check)
        {
            for (int i = 0; i < node.Nodes.Count; i++)
            {
                if (node.Nodes[i].Checked != check)
                {
                    node.Nodes[i].Checked = check;
                }

                CheckAllChildren(node.Nodes[i], check);
            }
        }

        protected override void OnAfterCheck(TreeViewEventArgs e)
        {
            base.OnAfterCheck(e);

            if (!(e.Action == TreeViewAction.ByKeyboard || e.Action == TreeViewAction.ByMouse))
            {
                return;
            }

            try
            {
                this.BeginUpdate();

                // the node if checked... 
                if (e.Node.Checked)
                {                    
                    // check all child nodes...
                    CheckAllChildren(e.Node, true);

                    // check all parents...
                    TreeNode parent = e.Node.Parent;

                    while (parent != null)
                    {
                        if (!parent.Checked) parent.Checked = true;
                        parent = parent.Parent;
                    }                    
                }
                else
                {
                    // check all child nodes...
                    CheckAllChildren(e.Node, false);
                    
                    // uncheck the parent node if all child nodes are unchecked...
                    TreeNode parent = e.Node.Parent;
                    while (parent != null)
                    {
                        if (!IsSomeChildNodeChecked(parent)) parent.Checked = false;
                        parent = parent.Parent;
                    }
                }
            }
            finally
            {
                this.EndUpdate();
            }            
        }

    
    }
}
