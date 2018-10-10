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

DSC2CSLIB_API HRESULT DSShow_InitilizeRSTPSource(HWND hwnd, const WCHAR *url, bool Audio)
{
	return m_dsShowPlayer.InitilizeRSTPSource(hwnd, url, Audio);
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




 
