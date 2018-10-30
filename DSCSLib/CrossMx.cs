using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DSCSLib
{
    public class CrossMx : Cross
    {
        Stopwatch m_swPush = new Stopwatch();
        Cross [] m_cross;
        DSCS m_dsc;
        static int index = 0;
        int m_ixStart;
        int m_ixEnd;
        struct XY
        {
            public int x;
            public int y;
        }
        int m_size = 0;
        XY[] m_xy;
        XY m_last;
        int m_wix = 0;
        int m_fsize = 0;

        public CrossMx(int line1Id, int line2Id, DSCS dsc) : base(line1Id, line2Id, dsc)
        {
            m_dsc = dsc;
        }
        public void Init(int size)
        {           
            m_size = size;
            m_xy = new XY[size];
            m_wix = 0;
            m_fsize = 0;
            m_cross = new Cross[size];
            int j = 0;
            m_ixStart = index;
            
            for (int i = 0; i < size * 2; i+=2)
            {
                m_cross[j++] = new CrossMx(index, index + 1, m_dsc);
                index += 2;
            }
            m_ixEnd = index;
            m_swPush.Restart();
        }
         
        public void Push(int x , int y, bool removeDups, int keepTime)
        {
            if (removeDups && m_last.x == x && m_last.y == y)
                return;

            if (m_swPush.ElapsedMilliseconds < keepTime)
                return;

            m_swPush.Restart();

            m_xy[m_wix].x = x;
            m_xy[m_wix].y = y;
            m_last.x = x;
            m_last.y = y;
            m_wix = (m_wix + 1) % m_size;
            if (m_wix != 0)
            {
                if (m_fsize < m_size)
                    m_fsize = m_fsize + 1;
            }
        }
        public void Draw(int  height, Color color, int width, double m_resolutionScale)
        {
            for (int i = 0; i < m_fsize; i++)
            {
                m_cross[i].Draw(m_xy[i].x, m_xy[i].y, height, color, width);
            }
            draw = true;
        }
        public void Clear()
        {
            
            base.Clear();
            m_wix = 0;
            m_fsize = 0;
        }

        public void DrawFirst(int height, Color color, int width, double m_resolutionScale)
        {
            m_cross[0].Draw(m_xy[0].x, m_xy[0].y, height, color, width);
        }
    }
}
