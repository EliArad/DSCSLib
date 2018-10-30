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
using static DSCSLib.DSCS;

namespace DSCSPlayer
{
    public partial class Form1 : Form
    {

        double m_resolutionScale = 1;
        bool m_replaceWindow = false;
        DSCS m_player = new DSCS();
        Cross m_cross;
        bool m_mouseDown = false;
        public Form1()
        {
            InitializeComponent();
        }

        private void btnPlay_Click(object sender, EventArgs e)
        {
            m_player.Play();
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            m_player.Stop();
        }

        private void btnPause_Click(object sender, EventArgs e)
        {
            m_player.Pause();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (m_replaceWindow == false)
            {
                m_player.SetWindowHandle(panel2.Handle);
                Rect r = new Rect(0, 0, panel2.Width, panel1.Height);
                m_player.UpdateVideoWindow(r);
                panel1.Update();
                panel1.Refresh();
                panel1.Invalidate();
            }
            else
            {
                m_player.SetWindowHandle(panel1.Handle);
                Rect r = new Rect(0, 0, panel1.Width, panel1.Height);
                m_player.UpdateVideoWindow(r);
                panel2.Update();
                panel2.Refresh();
            }
            m_replaceWindow = !m_replaceWindow;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (m_player != null)
            {
                m_player.Stop();
                m_player.Close();
            }
        }

        private void Form1_Resize(object sender, EventArgs e)
        {

            if (m_replaceWindow == false)
            {
                Rect r = new Rect(0, 0, panel1.Width, panel1.Height);
                m_player.UpdateVideoWindow(r);
            }
            else
            {
                Rect r = new Rect(0, 0, panel2.Width, panel1.Height);
                m_player.UpdateVideoWindow(r);
            }
        }

        private void btnPlayerExample_Click(object sender, EventArgs e)
        {

            m_player.SetFileName(@"C:\Users\elia\Downloads\Truck.ts");
            //m_player.SetFileName(@"C:\savevideo.h264");
            m_player.SelectDecoder(SELECTED_DECODER.ELECARD);
            m_player.InitializePlayer(panel1.Handle);            
            Rect r = new Rect(0, 0, panel1.Width, panel1.Height);
            m_player.UpdateVideoWindow(r);
        }

        private void btnRTSP_Click(object sender, EventArgs e)
        {

            m_player.SelectDecoder(SELECTED_DECODER.LEADTOOLS);
            int hr = m_player.InitilizeRSTPSource(panel1.Handle, "rtsp://127.0.0.1:554/savevideo.h264", false, false, @"c:\xxx.h264");
            //int hr = m_player.InitilizeRSTPSource(panel1.Handle, "rtsp://127.0.0.1:554/long_record.avc", false, true, false, @"c:\xxx.h264");
            if (hr != 0)
            {
                MessageBox.Show("Failed to initialize RTSP");
                return;
            }
             
            Rect r = new Rect(0, 0, panel1.Width, panel1.Height);           
            m_player.AddCircle(0,
                               100,
                               100,
                               50,
                               50,
                               Color.Red,
                               10);
            

            m_cross = new Cross(1, 2, m_player);
            m_cross.Draw(200, 200, 100, Color.Green, 10);
            m_player.AddTextOverlay("Eli Arad", 3, 200, 200, 400, 100, Color.Blue, 40, 0);

            m_player.ApplyOverlay(1f);

            m_player.Repaint(panel1.Handle);
           

        }

        private void button2_Click(object sender, EventArgs e)
        {
            m_player.SetFileName(@"C:\savevideo.h264");
            m_player.SelectDecoder(SELECTED_DECODER.ELECARD);
            m_player.InitilizeRSTPSource(panel1.Handle, "rtsp://127.0.0.1:554/savevideo.h264", false,  true, "c:\\1.h264");
            Rect r = new Rect(0, 0, panel1.Width, panel1.Height);
            m_player.UpdateVideoWindow(r);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            m_player.Close();
        }

        private void button4_Click(object sender, EventArgs e)
        {

            Form2 f = new Form2();
            f.SetPanel(m_player);
            f.ShowDialog();
            m_player.SetWindowHandle(panel1.Handle);
            Rect r = new Rect(0, 0, panel1.Width, panel1.Height);
            m_player.UpdateVideoWindow(r);


        }

        private void panel1_MouseDown(object sender, MouseEventArgs e)
        {
            m_mouseDown = true;
        }

        private void panel1_MouseMove(object sender, MouseEventArgs e)
        {
            if (m_mouseDown == true)
            {
                m_cross.Draw(e.X, e.Y, 100, Color.Green, 10);
                m_player.ApplyOverlay(1f);
            }
        }

        private void panel1_MouseUp(object sender, MouseEventArgs e)
        {
            m_mouseDown = false;
        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            if (m_player != null)
                m_player.Repaint(panel1.Handle);
        }
    }
}
