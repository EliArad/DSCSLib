using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows;
using System.Drawing;

namespace DSCSLib
{


    public class DSCS
    {
        bool m_videoInitialized = false;

        public enum SELECTED_DECODER
        {
            RENDER,
            ELECARD,
            LEADTOOLS
        }

        public enum SHAPE_IDs
        {
            LINE1,
            LINE2,
            CIRCLE = 2,
            DISTANCE_TEXT = 3
        }

        //const string dsPath = @"D:\Projects\DSLibrary\Debug\DSC2CSLib.dll";
        const string dsPath = @"D:\Projects\DSLibrary\Release\DSC2CSLib.dll";

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_SetFileName([MarshalAs(UnmanagedType.LPWStr)]String sFileName);

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_Play();

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_Pause();
        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_Stop();

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_SetWindowHandle(IntPtr handle);

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_InitializePlayer([MarshalAs(UnmanagedType.SysInt)]IntPtr handle);

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_InitilizeRSTPSource([MarshalAs(UnmanagedType.SysInt)]IntPtr handle,
                                                            [MarshalAs(UnmanagedType.LPWStr)]String url,
                                                            bool audio,
                                                            bool SaveToFile,
                                                            [MarshalAs(UnmanagedType.LPWStr)]String saveFileName);


        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_SelectDecoder(int selectedDecoder);


        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_UpdateVideoWindow(int x, int y, int width, int height);

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_AddTextOverlay([MarshalAs(UnmanagedType.LPWStr)]String text,
                                                         int id,
                                                         int left,
                                                         int top,
                                                         int right,
                                                         int bottom,
                                                         int color,
                                                         float fontSize,
                                                         int fontStyle);


        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_AddTextOverlay2([MarshalAs(UnmanagedType.LPWStr)]String text, int id,
                                      int left,
                                      int top,
                                      int right,
                                      int bottom,
                                      int color,
                                      float fontSize);

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_Clear();

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_Visible(int id, bool visible);


        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_AddLine(int id,
                                                int x1,
                                                int y1,
                                                int x2,
                                                int y2,
                                                int color,
                                                int width);



        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_AddCircle(int id,
                                                  int x1,
                                                  int y1,
                                                  int radios_w,
                                                  int radios_h,
                                                  int color,
                                                  int width);



        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_ApplyOverlay(float alpha_opacity);


        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_Repaint(IntPtr hdc);



        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_Close();



        public void Close()
        {
            lock (this)
            {
                if (m_videoInitialized == true)
                    DSShow_Close();
            }
        }
        public int SetFileName(string sFileName)
        {
            lock (this)
            {
                return DSShow_SetFileName(sFileName);
            }
        }

        public int Play()
        {
            lock (this)
            {
                return DSShow_Play();
            }
        }

        public int Run()
        {
            lock (this)
            {
                return DSShow_Play();
            }
        }

        public int Start()
        {
            lock (this)
            {
                return DSShow_Play();
            }
        }


        public int Pause()
        {
            lock (this)
            {
                return DSShow_Pause();
            }
        }

        public int Stop()
        {
            lock (this)
            {
                return DSShow_Stop();
            }
        }
        public int InitializePlayer(IntPtr handle)
        {
            lock (this)
            {
                int r = DSShow_InitializePlayer(handle);
                if (r == 0)
                {
                    m_videoInitialized = true;
                }
                return r;
            }
        }

        public int InitilizeRSTPSource(IntPtr handle, string url, bool audio,  bool SaveToFile, string saveFileName)
        {
            lock (this)
            {
                int r = DSShow_InitilizeRSTPSource(handle, url, audio,  SaveToFile, saveFileName);
                if (r == 0)
                {
                    m_videoInitialized = true;
                }
                return r;
            }
        }

        public int Repaint(IntPtr hdc)
        {
            lock (this)
            {
                if (m_videoInitialized == false)
                    return 0;
                return DSShow_Repaint(hdc);
            }

        }

        public void SelectDecoder(SELECTED_DECODER decoder)
        {
            lock (this)
            {
                DSShow_SelectDecoder((int)decoder);
            }
        }
        public void SetWindowHandle(IntPtr handle)
        {
            lock (this)
            {
                try
                {
                    if (m_videoInitialized == false)
                        return;
                    DSShow_SetWindowHandle(handle);
                }
                catch (Exception err)
                {
                    Logger.Instance.Write("Set window handle: " + err.Message);
                }
            }
        }

        public void UpdateVideoWindow(Rect prc)
        {
            lock (this)
            {
                if (m_videoInitialized == false)
                    return;
                DSShow_UpdateVideoWindow((int)prc.X, (int)prc.Y, (int)prc.Width, (int)prc.Height);
            }
        }
         
        public int AddTextOverlay(string text, 
                                    int id,
                                    int left,
                                    int top,
                                    int right,
                                    int bottom,
                                    Color color,
                                    float fontSize,
                                    int fontStyle)
        {

            lock (this)
            {
                int c = color.B << 16 | color.G << 8 | color.R;

                return DSShow_AddTextOverlay(text,
                                             id,
                                             left,
                                             top,
                                             right,
                                             bottom,
                                             c,
                                             fontSize,
                                             fontStyle);
            }
        }



        public int AddTextOverlay2(string text, int id,
                                       int left,
                                       int top,
                                       int right,
                                       int bottom,
                                       Color color,
                                       float fontSize)
        {

            lock (this)
            {
                int c = color.B << 16 | color.G << 8 | color.R;

                return DSShow_AddTextOverlay2(text,
                                        id,
                                        left,
                                        top,
                                        right,
                                        bottom,
                                        c,
                                        fontSize);
            }
        }


        public int ApplyOverlay(float alpha_opacity = 1f)
        {
            lock (this)
            {
                return DSShow_ApplyOverlay(alpha_opacity);
            }
        }

        public int Clear()
        {
            lock (this)
            {
                return DSShow_Clear();
            }
        }


        public int Visible(SHAPE_IDs id, bool visible)
        {
            lock (this)
            {
                return DSShow_Visible((int)id, visible);
            }
        }

        public int Visible(int id, bool visible)
        {
            lock (this)
            {
                return DSShow_Visible(id, visible);
            }
        }

        public int AddLine(int id,
                            int x1,
                            int y1,
                            int x2,
                            int y2,
                            Color color,
                            int width)
        {
            lock (this)
            {
                int c = color.B << 16 | color.G << 8 | color.R;
                return DSShow_AddLine(id,
                                       x1,
                                       y1,
                                       x2,
                                       y2,
                                       c,
                                       width);
            }

        }
     
        public int AddCircle(int id,
                             int x1,
                             int y1,
                             int radios_w,
                             int radios_h,
                             Color color,
                             int width)
        {

            lock (this)
            {
                int c = color.B << 16 | color.G << 8 | color.R;
                return DSShow_AddCircle(id,
                                         x1,
                                         y1,
                                         radios_w,
                                         radios_h,
                                         c,
                                         width);
            }

        }

    }
}
