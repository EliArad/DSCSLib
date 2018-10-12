// DSC2CSLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DSC2CSLib.h"
#include "DShowPlayer.h"


DShowPlayer m_dsShowPlayer;

// This is an example of an exported function.
DSC2CSLIB_API int Test(void)
{
    return 42;
}

DSC2CSLIB_API void DSShow_SetFileName(const WCHAR* sFileName)
{
	m_dsShowPlayer.SetFileName(sFileName);

}

DSC2CSLIB_API HRESULT DSShow_Play()
{
	return m_dsShowPlayer.Play();
}

DSC2CSLIB_API HRESULT DSShow_Pause()
{
	return m_dsShowPlayer.Pause();
}

DSC2CSLIB_API HRESULT DSShow_Stop()
{
	return m_dsShowPlayer.Stop();
}

DSC2CSLIB_API void DSShow_SetWindowHandle(HWND hwnd)
{ 
	m_dsShowPlayer.SetWindowHandle(hwnd);
}

DSC2CSLIB_API HRESULT DSShow_InitializePlayer(HWND hwnd)
{
	return m_dsShowPlayer.InitilizePlayer(hwnd);
} 

DSC2CSLIB_API HRESULT DSShow_InitilizeRSTPSource(HWND hwnd, const WCHAR *url, bool Audio, bool ShapeFilter, bool SaveToFile, const WCHAR *saveFileName)
{
	return m_dsShowPlayer.InitilizeRSTPSource(hwnd, url, Audio , ShapeFilter, SaveToFile, saveFileName);
}

DSC2CSLIB_API  void DSShow_UpdateVideoWindow(int x, int y, int width, int height)
{
	RECT r;
	r.left = x;
	r.top = y;
	r.right = width;
	r.bottom = height;

	m_dsShowPlayer.UpdateVideoWindow((LPRECT)&r);
}
 

DSC2CSLIB_API  void DSShow_SelectDecoder(int selectedDecoder)
{
	m_dsShowPlayer.SelectDecoder((SELECTED_DECODER)selectedDecoder);
}



DSC2CSLIB_API HRESULT DSShow_AddCircle(int id,
									   int x1,
									   int y1,
									   int radios_w,
									   int radios_h,
									   COLORREF color,
									   int width)
{


	return m_dsShowPlayer.AddCircle(id,
									 x1,
									 y1,
									 radios_w,
									 radios_h,
									 color,
									 width);

}

DSC2CSLIB_API HRESULT DSShow_AddLine(int id,
	int x1,
	int y1,
	int x2,
	int y2,
	int color,
	int width)
{

	return m_dsShowPlayer.AddLine(id,
		x1,
		y1,
		x2,
		y2,
		color,
		width);
}


DSC2CSLIB_API HRESULT	DSShow_Visible(int id, bool visible)
{
	return m_dsShowPlayer.Visible(id, visible);
}
DSC2CSLIB_API HRESULT	DSShow_Clear()
{
	return m_dsShowPlayer.Clear();
}


DSC2CSLIB_API void DSShow_Close()
{
	return m_dsShowPlayer.Close();
}

HRESULT	DSShow_AddTextOverlay2(WCHAR *text, int id,
	int left,
	int top,
	int right,
	int bottom,
	int color,
	float fontSize)
{
	return m_dsShowPlayer.AddTextOverlay2(text, id,
		 left,
		 top,
		 right,
		 bottom,
		 color,
		fontSize);

}

DSC2CSLIB_API HRESULT	DSShow_AddTextOverlay(WCHAR *text,
	int id,
	int left,
	int top,
	int right,
	int bottom,
	int color,
	float fontSize,
	int fontStyle)
{
	return m_dsShowPlayer.AddTextOverlay(text,
		id,
		left,
		top,
		right,
		bottom,
		color,
		fontSize,
		fontStyle);

}

DSC2CSLIB_API HRESULT DSShow_Repaint(HDC hdc)
{
	return m_dsShowPlayer.Repaint(hdc);

}

