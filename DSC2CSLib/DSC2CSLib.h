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
	DSC2CSLIB_API HRESULT DSShow_Play();
	DSC2CSLIB_API HRESULT DSShow_Stop();
	DSC2CSLIB_API HRESULT DSShow_Pause();
	DSC2CSLIB_API void DSShow_SetWindowHandle(HWND hwnd);
	DSC2CSLIB_API void DSShow_SetFileName(const WCHAR* sFileName);
	DSC2CSLIB_API HRESULT DSShow_InitializePlayer(HWND hwnd);
	DSC2CSLIB_API HRESULT DSShow_InitilizeRSTPSource(HWND hwnd, const WCHAR *url, bool Audio, bool ShapeFilter, bool SaveToFile, const WCHAR *saveFileName);
	DSC2CSLIB_API  void DSShow_UpdateVideoWindow(int x, int y, int width, int height);
	DSC2CSLIB_API  void DSShow_SelectDecoder(int selectedDecoder);
	DSC2CSLIB_API  void DSShow_Close();

	DSC2CSLIB_API HRESULT DSShow_AddCircle(int id,
											int x1,
											int y1,
											int radios_w,
											int radios_h,
											COLORREF color,
											int width);



	DSC2CSLIB_API HRESULT DSShow_AddLine(int id,
										int x1,
										int y1,
										int x2,
										int y2,
										int color,
										int width);


	HRESULT	DSShow_Remove(int id);
	HRESULT	DSShow_Clear();


	HRESULT	DSShow_AddTextOverlay2(WCHAR *text, int id,
		int left,
		int top,
		int right,
		int bottom,
		int color,
		float fontSize);;

	HRESULT	DSShow_AddTextOverlay(WCHAR *text,
		int id,
		int left,
		int top,
		int right,
		int bottom,
		int color,
		float fontSize,
		int fontStyle);


}
