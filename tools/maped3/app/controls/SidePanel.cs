using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace winmaped2
{
    public class CollapsiblePanel : Panel
    {

        #region Designer code


        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // SidePanel
            // 
            this.Size = new System.Drawing.Size(346, 268);
            this.ResumeLayout(false);

        }
        #endregion

        #region Attributes and properties
        Size expandedSize;
        Rectangle collapseArea, hideArea, titleArea;

        private bool collapsed;
        [DefaultValue(false)]
        public bool Collapsed
        {
            get 
            { 
                return collapsed; 
            }
            set 
            {                
                collapsed = value;
                UpdateStatus();                
            }
        }

        private string title;

        public string Title
        {
            get 
            { 
                return title; 
            }
            set 
            { 
                title = value;
                menuItem.Text = Title;
                Invalidate(titleArea);
            }
        }

        private MenuItem menuItem;

        public MenuItem MenuItem
        {
            get { return menuItem; }            
        }


        
        #endregion

        public CollapsiblePanel()
        {
            InitializeComponent();

            menuItem = new MenuItem();
            menuItem.Click += menuItem_Click;

            expandedSize = this.ClientSize;

            collapseArea = new Rectangle(ClientRectangle.Width - 2 * SystemInformation.ToolWindowCaptionHeight, 0, SystemInformation.ToolWindowCaptionHeight, SystemInformation.ToolWindowCaptionHeight);
            hideArea = new Rectangle(ClientRectangle.Width - SystemInformation.ToolWindowCaptionHeight, 0, SystemInformation.ToolWindowCaptionHeight, SystemInformation.ToolWindowCaptionHeight);
            titleArea = new Rectangle(0, 0, ClientRectangle.Width, SystemInformation.ToolWindowCaptionHeight);
            
            Title = this.Name;
            
        }

        protected override Padding DefaultPadding
        {
            get
            {
                return new Padding(0, SystemInformation.ToolWindowCaptionHeight, 0, 0);
            }
        }
        
        void menuItem_Click(object sender, EventArgs e)
        {
            this.Visible = !this.Visible;
        }

        private void UpdateStatus()
        {
            if (collapsed)
            {
                expandedSize = this.ClientSize;
                this.ClientSize = titleArea.Size;                
            }
            else
            {
                this.ClientSize = expandedSize;
            }

            Invalidate();
        }

        private void UpdateAreas()
        {
            collapseArea = new Rectangle(ClientRectangle.Width - 2 * SystemInformation.ToolWindowCaptionHeight, 0, SystemInformation.ToolWindowCaptionHeight, SystemInformation.ToolWindowCaptionHeight);
            hideArea = new Rectangle(ClientRectangle.Width - SystemInformation.ToolWindowCaptionHeight, 0, SystemInformation.ToolWindowCaptionHeight, SystemInformation.ToolWindowCaptionHeight);
            titleArea = new Rectangle(0, 0, ClientRectangle.Width, SystemInformation.ToolWindowCaptionHeight);
        }

        protected override void OnResize(EventArgs e)
        {
            UpdateAreas();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            
            Graphics g = e.Graphics;
            
            
            Font buttonFont = new Font(SystemFonts.SmallCaptionFont, FontStyle.Bold);

            
            Brush backBrush = Enabled ? SystemBrushes.GradientActiveCaption : SystemBrushes.GradientInactiveCaption;
            Brush textBrush = Enabled ? SystemBrushes.ActiveCaptionText : SystemBrushes.InactiveCaptionText;

            g.FillRectangle(backBrush, titleArea);

            g.DrawRectangle(new Pen(SystemBrushes.ControlDark), 0, 0, this.ClientSize.Width - 1, this.ClientSize.Height - 1);
            
            g.DrawString(title, Font, textBrush, titleArea);

            g.DrawString("X", buttonFont, textBrush, hideArea);
            g.DrawString(collapsed ? "v" : "^", buttonFont, textBrush, collapseArea);
            
            base.OnPaint(e);
        }

        protected override void OnMouseClick(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (hideArea.Contains(e.Location))
                {
                    this.Visible = false;
                }
                else
                {
                    Collapsed = !Collapsed;
                }
            }
            base.OnMouseClick(e);
        }

        protected override void OnVisibleChanged(EventArgs e)
        {
            menuItem.Checked = Visible;
            base.OnVisibleChanged(e);
        }
    }
}
