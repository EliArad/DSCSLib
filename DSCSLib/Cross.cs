using DSCSLib;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DSCSLib
{
    public class Cross 
    {
        DSCS m_dsc;
        int[] m_id = { 0, 0 };
        protected bool draw = false;
        public Cross(int line1Id , int line2Id , DSCS dsc) 
        {
            m_dsc = dsc;
            m_id[0] = line1Id;
            m_id[1] = line2Id;
        }
        public void Draw(int x1, int y1, int height, double m_resolutionScale, Color color1, Color color2, int width1, int width2)
        {
            m_dsc.AddLine(m_id[0], x1 , y1, x1 , y1 - height, color1 , width1);
            m_dsc.AddLine(m_id[1], x1 - height / 2, y1 - height / 2, x1 + height / 2, y1 - height / 2, color2, width2);
            draw = true;
        }
        public void Draw(int x1, int y1, int height, double m_resolutionScale, Color color1, Color color2, int width)
        {
            m_dsc.AddLine(m_id[0], x1, y1, x1, y1 - height, color1, width);
            m_dsc.AddLine(m_id[1], x1 - height / 2, y1 - height / 2, x1 + height / 2, y1 - height / 2, color2, width);
            draw = true;
        }
        public void Draw(int x1, int y1, int height, double m_resolutionScale, Color color, int width)
        {
            int n = (int)(m_resolutionScale * (double)height / 2.0);
            m_dsc.AddLine(m_id[0], x1, y1 + height / 2, x1 , y1 - height / 2, color, width );
            m_dsc.AddLine(m_id[1], x1 -  height/2 - n , y1 , x1 + height / 2 + n, y1 , color, width);
            draw = true;
        }
        public void Clear()
        {
            if (draw == false)
                return;

            m_dsc.Clear();
        }
        public void Remove()
        {
            m_dsc.Visible(m_id[0], false);
            m_dsc.Visible(m_id[1], false);
        }
    }
}
