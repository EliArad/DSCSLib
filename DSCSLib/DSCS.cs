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

        public enum SELECTED_DECODER
        {
            RENDER,
            ELECARD,
            LEADTOOLS
        }

        public enum SHAPE_IDs
        {
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
                                                            bool shapeFilter,
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
        public static extern int DSShow_Repaint(IntPtr hdc);



        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_Close();



        public void Close()
        {
            DSShow_Close();
        }
        public int SetFileName(string sFileName)
        {
            return DSShow_SetFileName(sFileName);
        }

        public int Play()
        {
            return DSShow_Play();
        }

        public int Run()
        {
            return DSShow_Play();
        }

        public int Start()
        {
            return DSShow_Play();
        }


        public int Pause()
        {
            return DSShow_Pause();
        }

        public int Stop()
        {
            return DSShow_Stop();
        }
        public int InitializePlayer(IntPtr handle)
        {
            return DSShow_InitializePlayer(handle);
        }

        public int InitilizeRSTPSource(IntPtr handle, string url, bool audio, bool shapeFilter, bool SaveToFile, string saveFileName)
        {
            return DSShow_InitilizeRSTPSource(handle, url, audio, shapeFilter, SaveToFile, saveFileName);
        }

        public int Repaint(IntPtr hdc)
        {
            return DSShow_Repaint(hdc);

        }

        public void SelectDecoder(SELECTED_DECODER decoder)
        {
            DSShow_SelectDecoder((int)decoder);
        }
        public void SetWindowHandle(IntPtr handle)
        {
            DSShow_SetWindowHandle(handle);
        }

        public void UpdateVideoWindow(Rect prc)
        {
            DSShow_UpdateVideoWindow((int)prc.X , (int)prc.Y, (int)prc.Width, (int)prc.Height);
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



        public int AddTextOverlay2(string text, int id,
                                       int left,
                                       int top,
                                       int right,
                                       int bottom,
                                       Color color,
                                       float fontSize)
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


        public int Clear()
        {
            return DSShow_Clear();
        }


        public int Visible(SHAPE_IDs id, bool visible)
        {
            return DSShow_Visible((int)id, visible);
        }

        public int Visible(int id, bool visible)
        {
            return DSShow_Visible(id, visible);
        }

        public int AddLine(int id,
                            int x1,
                            int y1,
                            int x2,
                            int y2,
                            Color color,
                            int width)
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


     
        public int AddCircle(int id,
                       int x1,
                       int y1,
                       int radios_w,
                       int radios_h,
                       Color color,
                       int width)
        {

           int c = color.B << 16 | color.G << 8 | color.R;
           return  DSShow_AddCircle(id,
                                    x1,
                                    y1,
                                    radios_w,
                                    radios_h,
                                    c,
                                    width);

        }



    }
}
