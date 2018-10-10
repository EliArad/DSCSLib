﻿using DSCSLib;
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
        bool m_replaceWindow = false;
        DSCS m_player = new DSCS();
        public Form1()
        {
            InitializeComponent();


            //m_player.SetFileName(@"C:\Users\elia\Downloads\Truck.ts");
            m_player.SetFileName(@"C:\savevideo.h264");
            m_player.Initialize(panel1.Handle);
            m_player.SelectDecoder(SELECTED_DECODER.ELECARD);
            Rect r = new Rect(0, 0, panel1.Width, panel1.Height);
            m_player.UpdateVideoWindow(r);
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
                m_player.Stop();
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
    }
}