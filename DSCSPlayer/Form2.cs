using DSCSLib;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;

namespace DSCSPlayer
{
    public partial class Form2 : Form
    {
        DSCS m_player;
        int count = 0;
        public Form2()
        {
            InitializeComponent();
        }
        public void SetPanel(DSCS player)
        {
            m_player = player;
            timer1.Enabled = true;
            timer1.Interval = 10;
        }

        private void Form2_Shown(object sender, EventArgs e)
        {
            Rect r = new Rect(0, 0, panel1.Width, panel1.Height);
            m_player.UpdateVideoWindow(r);
            m_player.SetWindowHandle(panel1.Handle);
        }

        private void Form2_Paint(object sender, PaintEventArgs e)
        {

           
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            count++;

            Graphics g = panel1.CreateGraphics();
            IntPtr hDC = g.GetHdc();
            m_player.Repaint(hDC);
            if (count == 10)
                timer1.Enabled = false;
        }
    }
}
