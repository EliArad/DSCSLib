using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows;

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

        const string dsPath = @"D:\Program Files\Microsoft SDKs\Windows\v7.0\Samples\multimedia\directshow\baseclasses\Debug\DSC2CSLib.dll";

        

        [DllImport(dsPath, CallingConvention= CallingConvention.Cdecl)]
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
        public static extern int DSShow_Initialize([MarshalAs(UnmanagedType.SysInt)]IntPtr handle);

        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_SelectDecoder(int selectedDecoder);


        [DllImport(dsPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern int DSShow_UpdateVideoWindow(int x , int y , int width, int height);


        public int SetFileName(string sFileName)
        {
            return DSShow_SetFileName(sFileName);
        }

        public int Play()
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
        public void Initialize(IntPtr handle)
        {
            DSShow_Initialize(handle);
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
    }
}
