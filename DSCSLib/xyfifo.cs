using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DSCSLib
{
    public class xyfifo
    {
        struct XY
        {
            public int x;
            public int y;
        }
        XY [] fifo;
        int m_size;
        int m_readIndex = 0;
        int m_writeIndex = 0;
        public xyfifo(int size)
        {
            fifo = new XY[size];
            m_size = size;
        }

        public int GetSize()
        {
            lock (this)
            {
                if (m_readIndex == m_writeIndex)
                    return 0;

                if (m_writeIndex > m_readIndex)
                    return m_writeIndex - m_readIndex;

                return m_size - m_readIndex + m_writeIndex;
            }
        }

        public bool Push(int x , int y)
        {
            lock (this)
            {
                if (GetSize() == m_size -1)
                {
                    return false;
                }
                fifo[m_writeIndex].x = x;
                fifo[m_writeIndex].y = y;
                m_writeIndex = (m_writeIndex + 1) % m_size;
                return true;
            }
        }
        public bool Pop(out int x, out int y)
        {
           
            lock (this)
            {
                if (GetSize() == 0)
                {
                    x = 0;
                    y = 0;
                    return false;
                }
                x = fifo[m_readIndex].x;
                y = fifo[m_readIndex].y;
                m_readIndex = (m_readIndex + 1) % m_size;
                return true;
            }
        }


    }
}
