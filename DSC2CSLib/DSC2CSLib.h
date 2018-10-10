// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DSC2CSLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DSC2CSLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DSC2CSLIB_EXPORTS
#define DSC2CSLIB_API __declspec(dllexport)
#else
#define DSC2CSLIB_API __declspec(dllimport)
#endif


extern "C"
{
	DSC2CSLIB_API int Test(void);

	DSC2CSLIB_API HRESULT DSShow_PlayFile(const WCHAR* sFileName);
	DSC2CSLIB_API HRESULT DSShow_Play();
	DSC2CSLIB_API HRESULT DSShow_Stop();
	DSC2CSLIB_API HRESULT DSShow_Pause();
	DSC2CSLIB_API void DSShow_SetWindowHandle(HWND hwnd);
	DSC2CSLIB_API void DSShow_SetFileName(const WCHAR* sFileName);
	DSC2CSLIB_API void DSShow_Initialize(HWND hwnd);
	DSC2CSLIB_API  void DSShow_UpdateVideoWindow(int x, int y, int width, int height);
	DSC2CSLIB_API  void DSShow_SelectDecoder(int selectedDecoder);

}
