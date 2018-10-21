//////////////////////////////////////////////////////////////////////////
// DShowPlayer.cpp: Implements DirectShow playback functionality.
// 
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include "DShowPlayer.h"
#include "DshowUtil.h"
#include <gdiplus.h>




//-----------------------------------------------------------------------------
// DShowPlayer constructor.
//-----------------------------------------------------------------------------

DShowPlayer::DShowPlayer() :
	m_state(STATE_CLOSED),	
	m_hwndEvent(NULL),
	m_EventMsg(0),
	m_pGraph(NULL),
	m_pControl(NULL),
	m_pEvent(NULL),
	m_pSeek(NULL),
	m_pAudio(NULL),
    m_pVideo(NULL),
	m_seekCaps(0),
	m_bMute(FALSE),
	m_lVolume(MAX_VOLUME)
{
	 
	pShapeFilterInterface = NULL;

}

void DShowPlayer::SetWindowHandle(HWND hwnd)
{
	m_hwndVideo = hwnd;
	m_pVideo->SetVideoWindow(hwnd);
	 
}


//-----------------------------------------------------------------------------
// DShowPlayer destructor.
//-----------------------------------------------------------------------------

DShowPlayer::~DShowPlayer()
{
	TearDownGraph();
}

extern CComPtr<IVMRMixerBitmap9> pBmp;


void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

HRESULT DShowPlayer::ApplyOverlay(float alpha_opacity)
{
	int cx, cy;
	HRESULT hr;
	HBITMAP hbm;
	RECT rcClient;

	GetDesktopResolution(cx, cy);

	GetClientRect(m_hwndVideo, &rcClient);

	HDC hdc = GetDC(m_hwndVideo);

	if (hdc == NULL)
	{
		return E_FAIL;
	}
	HDC hdcBmp = CreateCompatibleDC(hdc);
	if (hdcBmp == NULL)
	{
		return E_FAIL;
	}
	hbm = CreateCompatibleBitmap(hdc, cx, cy);
	BITMAP bm;
	if (0 == GetObject(hbm, sizeof(bm), &bm))
	{
		DeleteDC(hdcBmp);
		return E_FAIL;
	}

	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcBmp, hbm);
	if (hbmOld == 0)
	{
		DeleteDC(hdcBmp);
		return E_FAIL;
	}
	//To draw line
	//DrawLine1(100, 100, 200, 200, hdcBmp, 12);
	map<int, Overlay*>::iterator it;
	for (it = m_overlays.begin(); it != m_overlays.end(); it++)
	{
		Overlay* over = (*it).second;
		if (over->visible == 0)
			continue;

		if (over->type == 1) // line
		{

			HPEN hPen;
			//RGB(255, 0, 0)
			hPen = CreatePen(PS_SOLID, over->lineWidth, over->color.ToCOLORREF());
			SelectObject(hdcBmp, hPen);
			 
			MoveToEx(hdcBmp, over->x1, over->y1, NULL);
			LineTo(hdcBmp, over->x2, over->y2);
			DeletePen(hPen);
		}
		else
		if (over->type == 2) // circle
		{
			HPEN hPen = CreatePen(PS_SOLID, over->lineWidth, over->color.ToCOLORREF());
			SelectObject(hdcBmp, hPen);
			SelectObject(hdcBmp, GetStockObject(HOLLOW_BRUSH));			
			//Ellipse(hdcBmp , 1000 , 800 ,1400, 1000);
			Ellipse(hdcBmp , over->x1, over->y1 , over->radios_w , over->radios_h);
			DeletePen(hPen);
		}
		else
		if (over->type == 0) // text
		{
			RECT rect;
			HBRUSH hBrush;
			HFONT hFont;
		 

#if 0 

			hFont = CreateFont(over->fontSize, 0, 0, 0,
				FW_NORMAL, FALSE, FALSE, FALSE,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_ROMAN,
				L"Times New Roman");
#endif 

			hFont = CreateFont(45, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
				CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_MODERN, __T("Ariel"));




			SelectObject(hdcBmp, hFont);
			
		 
			//Sets the coordinates for the rectangle in which the text is to be formatted.
			SetRect(&rect, over->pos.X, over->pos.Y, over->pos.Width, over->pos.Height);
			SetTextColor(hdcBmp, over->color.ToCOLORREF());
			SetBkColor(hdcBmp, TRANSPARENT);
			DrawText(hdcBmp, over->text, -1, &rect, DT_NOCLIP);
			DeleteObject(hFont);
			 
 
		}
	}
	 

 


	VMR9AlphaBitmap bmpInfo;
	ZeroMemory(&bmpInfo, sizeof(bmpInfo));

	bmpInfo.dwFlags = VMRBITMAP_HDC | VMRBITMAP_SRCCOLORKEY;

	bmpInfo.hdc = hdcBmp;

	SetRect(&bmpInfo.rSrc, 0, 0, bm.bmWidth, bm.bmHeight);
	bmpInfo.rDest.left = 0.f;
	bmpInfo.rDest.top = 0.f;
	bmpInfo.rDest.right = 1.0f;
	bmpInfo.rDest.bottom = 1.0f;

	// Set the transparency value (1.0 is opaque, 0.0 is transparent).
	bmpInfo.fAlpha = alpha_opacity;  // 1;// 0.5f;
	bmpInfo.clrSrcKey = RGB(0, 0, 0);

	if (m_pVideo != NULL)
	{ 
		hr = pBmp->SetAlphaBitmap(&bmpInfo);
		if (FAILED(hr))
		{
			return E_FAIL;
		}
		//pBmp->Release();
		//SAFE_RELEASE(pBmp);	 
	}
	// Clean up.
	ReleaseDC(m_hwndVideo, hdc);
	DeleteBitmap(hbm);
	DeleteObject(SelectObject(hdcBmp, hbmOld));
	DeleteDC(hdcBmp);
}

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

void DShowPlayer::DrawLine1(int xx1, int yy1, int xx2, int yy2, HDC hdcBmp, int Thickness)
{
	RECT clntRc;
	int temp, s1, s2, swap;
	double dx, dy, p, x, y;
	x = xx1;
	y = yy1;
	dx = abs(xx2 - xx1);
	dy = abs(yy2 - yy1);
	s1 = sign(xx2 - xx1);
	s2 = sign(yy2 - yy1);
	swap = 0;
	if (dy > dx)
	{
		temp = dx;
		dx = dy;
		dy = temp;
		swap = 1;
	}
	p = 2 * dy - dx;
	for (int i = 0; i < dx; i++)
	{
		clntRc.left = x;
		clntRc.top = y;
		clntRc.right = x + Thickness;
		clntRc.bottom = y + Thickness;
		HBRUSH brush1 = CreateSolidBrush(RGB(0, 255, 0));
		FillRect(hdcBmp, &clntRc, brush1);

		while (p >= 0)
		{
			p = p - 2 * dx;
			if (swap)
				x += s1;
			else
				y += s2;
		}
		p = p + 2 * dy;
		if (swap)
			y += s2;
		else
			x += s1;
		DeleteObject(brush1);
	}

}




//-----------------------------------------------------------------------------
// DShowPlayer::SetEventWindow
// Description: Set the window to receive graph events.
//
// hwnd: Window to receive the events.
// msg: Private window message that window will receive whenever a 
//      graph event occurs. (Must be in the range WM_APP through 0xBFFF.)
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::SetEventWindow(HWND hwnd, UINT msg)
{
	m_hwndEvent = hwnd;
	m_EventMsg = msg;
	return S_OK;
}


//-----------------------------------------------------------------------------
// DShowPlayer::OpenFile
// Description: Open a new file for playback.
//-----------------------------------------------------------------------------

WCHAR* m_sFileName;
void DShowPlayer::SetFileName(const WCHAR* sFileName)
{

	m_sFileName = new wchar_t[500];
	wcscpy(m_sFileName, sFileName);
	 
}

HRESULT DShowPlayer::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin, int num)
{
	CComPtr<IEnumPins>  pEnum = NULL;
	IPin       *pPin = NULL;
	HRESULT    hr;

	if (ppPin == NULL)
	{
		return E_POINTER;
	}

	int count = 0;

	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while (pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		hr = pPin->QueryDirection(&PinDirThis);
		if (FAILED(hr))
		{
			//pPin->Release();
			//pEnum->Release();
			return hr;
		}
		if (PinDir == PinDirThis && count == num)
		{
			// Found a match. Return the IPin pointer to the caller.
			*ppPin = pPin;
			//pPin->Release();
		    //pEnum->Release();
			return S_OK;
		}
		if (PinDir == PinDirThis)
			count++;
		// Release the pin for the next time through the loop.
		//pPin->Release();
	}
	// No more pins. We did not find a match.
	//pEnum->Release();
	return E_FAIL;
}

HRESULT DShowPlayer::InitilizePlayer(HWND hwnd)
{ 
	HRESULT hr = S_OK;
	m_hwndVideo = hwnd;

	IBaseFilter *pSource = NULL;

	// Create a new filter graph. (This also closes the old one, if any.)
	hr = InitializeGraph();
	
	// Add the source filter to the graph.
	if (SUCCEEDED(hr))
	{
		hr = m_pGraph->AddSourceFilter(m_sFileName, NULL, &pSource);
	}

	// Try to render the streams.
	if (SUCCEEDED(hr))
	{
		hr = RenderStreams(pSource);
	}

	// Get the seeking capabilities.
	if (SUCCEEDED(hr))
	{
		hr = m_pSeek->GetCapabilities(&m_seekCaps);
	}

    // Set the volume.
	if (SUCCEEDED(hr))
	{
		hr = UpdateVolume();
	}

	// Update our state.
	if (SUCCEEDED(hr))
	{
		m_state = STATE_STOPPED;
	}

	SAFE_RELEASE(pSource);

	return hr;
}
 
 


 

HRESULT DShowPlayer::InitilizeRSTPSource(HWND hwnd, 
										 const WCHAR *url, 
										 bool Audio, 
										 bool SaveToFile,
										 const WCHAR *saveFileName)
{
	HRESULT hr = S_OK;
	m_hwndVideo = hwnd;

	 
	// Create a new filter graph. (This also closes the old one, if any.)
	hr = InitializeGraph();

	// Add the source filter to the graph.
	 
	BOOL bRenderedAnyPin = FALSE;
	CComPtr<IFileSourceFilter> pFS = NULL;
	CComPtr<IFilterGraph2> pGraph2 = NULL;
	IEnumPins *pEnum = NULL;


	hr = m_pGraph->QueryInterface(IID_IFilterGraph2, (void**)&pGraph2);

	hr = CreateVideoRenderer(VIDEO_RENDER::Try_VMR9);
	if (FAILED(hr))
	{
		return hr;
	}

	 

	static const GUID CLSID_LEADTOOLS_RTSP_SOURCE =
	{ 0xE2B7DE48, 0x38C5, 0x11D5,{ 0x91, 0xF6, 0x00, 0x10, 0x4B, 0xDB, 0x8F, 0xF9 } };
	
	hr = AddFilterByCLSID(m_pGraph, CLSID_LEADTOOLS_RTSP_SOURCE, &pLeadToolsRTSPSource, L"LEAD Tools RTSP Source");
	if (FAILED(hr))
	{
		return hr;
	}


	static const GUID IID_IFileSourceFilter = {
		0x56a868a6, 0x0ad4, 0x11ce,{ 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70 } };

	 
	hr = pLeadToolsRTSPSource->QueryInterface(IID_IFileSourceFilter, (void **)&pFS);
	if (FAILED(hr))
	{
		return hr;
	}

	// Load the source file.
	hr = pFS->Load(url, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	//SAFE_RELEASE(pFS);

	  

	if (Audio == true)
	{	// Add the DSound Renderer to the graph.
		if (SUCCEEDED(hr))
		{
			hr = AddFilterByCLSID(m_pGraph, CLSID_DSoundRender, &pAudioRenderer, L"Audio Renderer");
		}
	}


	if (m_selectedDecoder == SELECTED_DECODER::ELECARD && SUCCEEDED(hr))
	{
		// {5C122C6D-8FCC-46f9-AAB7-DCFB0841E04D}
		static const GUID CLSID_EAVCDEC =
		{ 0x5c122c6D, 0x8fcc, 0x46f9,{ 0xaa, 0xb7, 0xdc, 0xfb, 0x8, 0x41, 0xe0, 0x4d } };

		hr = AddFilterByCLSID(m_pGraph, CLSID_EAVCDEC, &pVideoDecoder, L"Elecard AVC Video Decoder");
		if (FAILED(hr))
		{
			return hr;
		}
	}

	if (m_selectedDecoder == SELECTED_DECODER::LEADTOOLS && SUCCEEDED(hr))
	{
		static const GUID CLSID_LEADTOOLS_H264_DECODER =
		{ 0xE2B7DF25,0x38C5,0x11D5,{ 0x91,0xF6, 0x00, 0x10, 0x4B , 0xDB ,0x8F , 0xF9 } };

		hr = AddFilterByCLSID(m_pGraph, CLSID_LEADTOOLS_H264_DECODER, &pVideoDecoder, L"LeadTools AVC Video Decoder");
		if (FAILED(hr))
		{
			return hr;
		}
	}

	
	 
	if (SaveToFile == true)
	{
		 

		static const GUID CLSID_INFPie =
		{ 0xD098F8A1, 0x145E, 0x4e45,{ 0x87, 0xE4, 0x87, 0x43, 0xAA, 0x10, 0x68, 0xBA } };
	 
		hr = AddFilterByCLSID(m_pGraph, CLSID_INFPie, &pInfTeeFilter, L"InfTee");
		if (FAILED(hr))
		{
			return hr;
		}

		static const GUID CLSID_SinkFilter =
		{ 0x36A5F770,0xFE4C,0x11CE,{ 0xA8,0xED, 0x00, 0xAA, 0x00, 0x2F, 0xEA, 0xB5 } };

		hr = AddFilterByCLSID(m_pGraph, CLSID_SinkFilter, &pSinkFilter, L"Sink Filter");
		if (FAILED(hr))
		{
			return hr;
		}

	  
		CComPtr<IFileSinkFilter>  iSink;
		hr = pSinkFilter->QueryInterface(IID_IFileSinkFilter, (void **)&iSink);
		if (FAILED(hr))
		{
			return hr;
		}

		// Load the source file.
		hr = iSink->SetFileName(saveFileName, NULL);
		if (FAILED(hr))
		{
			return hr;
		}
		//SAFE_RELEASE(iSink);

	}
   
 
	PIN_INFO pininfo;
	CComPtr<IPin> ppRTSPSourceVideoOutPin;
	GetPin(pLeadToolsRTSPSource, PIN_DIRECTION::PINDIR_OUTPUT, &ppRTSPSourceVideoOutPin);
	hr = ppRTSPSourceVideoOutPin->QueryPinInfo(&pininfo);
	if (FAILED(hr))
	{
		return hr;
	}



	CComPtr<IPin> ppVideoDecoderInPin;
	GetPin(pVideoDecoder, PIN_DIRECTION::PINDIR_INPUT, &ppVideoDecoderInPin);
	hr = ppVideoDecoderInPin->QueryPinInfo(&pininfo);
	if (FAILED(hr))
	{
		return hr;
	}
	 
 

	if (SaveToFile == true)
	{


		CComPtr<IPin> ppInfteeInput;
		GetPin(pInfTeeFilter, PIN_DIRECTION::PINDIR_INPUT, &ppInfteeInput);
		hr = ppInfteeInput->QueryPinInfo(&pininfo);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = pGraph2->Connect(ppRTSPSourceVideoOutPin, ppInfteeInput);
		if (FAILED(hr))
		{
			return hr;
		}

		CComPtr<IPin> ppinfTeeOutput1;
		GetPin(pInfTeeFilter, PIN_DIRECTION::PINDIR_OUTPUT, &ppinfTeeOutput1);
		hr = ppinfTeeOutput1->QueryPinInfo(&pininfo);
		if (FAILED(hr))
		{
			return hr;
		}


	 
		CComPtr<IPin> ppDumpInput;
		GetPin(pSinkFilter, PIN_DIRECTION::PINDIR_INPUT, &ppDumpInput);
		hr = ppDumpInput->QueryPinInfo(&pininfo);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = pGraph2->Connect(ppinfTeeOutput1, ppDumpInput);
		if (FAILED(hr))
		{
			return hr;
		}

		CComPtr<IPin> ppinfTeeOutput2;
		GetPin(pInfTeeFilter, PIN_DIRECTION::PINDIR_OUTPUT, &ppinfTeeOutput2, 1);
		hr = ppinfTeeOutput2->QueryPinInfo(&pininfo);
		if (FAILED(hr))
		{
			return hr;
		}

		 

		hr = pGraph2->Connect(ppinfTeeOutput2, ppVideoDecoderInPin);
		if (FAILED(hr))
		{
			return hr;
		}



		CComPtr<IPin> ppVideoDecoderOutPin;
		GetPin(pVideoDecoder, PIN_DIRECTION::PINDIR_OUTPUT, &ppVideoDecoderOutPin);
		hr = ppVideoDecoderOutPin->QueryPinInfo(&pininfo);
		if (FAILED(hr))
		{
			return hr;
		}
		hr = pGraph2->RenderEx(ppVideoDecoderOutPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);
		 
	}
	else 
	{  
		 
		hr = pGraph2->Connect(ppRTSPSourceVideoOutPin, ppVideoDecoderInPin);
		if (FAILED(hr))
		{
			return hr;
		}


		CComPtr<IPin> ppVideoDecoderOutPin;
		GetPin(pVideoDecoder, PIN_DIRECTION::PINDIR_OUTPUT, &ppVideoDecoderOutPin);
		hr = ppVideoDecoderOutPin->QueryPinInfo(&pininfo);
		if (FAILED(hr))
		{
			return hr;
		}
		hr = pGraph2->RenderEx(ppVideoDecoderOutPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);
		//hr = pGraph2->Render(ppVideoDecoderOutPin);
		if (FAILED(hr))
		{
			return hr;
		}
 	}
	 
	// Get the seeking capabilities.
	if (SUCCEEDED(hr))
	{
		hr = m_pSeek->GetCapabilities(&m_seekCaps);
	}

	  
	// Update our state.
	if (SUCCEEDED(hr))
	{
		m_state = STATE_STOPPED;
	}
  
	return hr;
}



//-----------------------------------------------------------------------------
// DShowPlayer::HandleGraphEvent
// Description: Respond to a graph event.
//
// The owning window should call this method when it receives the window
// message that the application specified when it called SetEventWindow.
//
// pCB: Pointer to the GraphEventCallback callback, implemented by 
//      the application. This callback is invoked once for each event
//      in the queue. 
//
// Caution: Do not tear down the graph from inside the callback.
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::HandleGraphEvent(GraphEventCallback *pCB)
{
	if (pCB == NULL)
	{
		return E_POINTER;
	}

	if (!m_pEvent)
	{
		return E_UNEXPECTED;
	}

	long evCode = 0;
	LONG_PTR param1 = 0, param2 = 0;

	HRESULT hr = S_OK;

    // Get the events from the queue.
	while (SUCCEEDED(m_pEvent->GetEvent(&evCode, &param1, &param2, 0)))
	{
        // Invoke the callback.
		pCB->OnGraphEvent(evCode, param1, param2);

        // Free the event data.
		hr = m_pEvent->FreeEventParams(evCode, param1, param2);
		if (FAILED(hr))
		{
			break;
		}
	}

	return hr;
}


// state changes

HRESULT DShowPlayer::Play()
{
	if (m_state != STATE_PAUSED && m_state != STATE_STOPPED)
	{
		return VFW_E_WRONG_STATE;
	}

	assert(m_pGraph); // If state is correct, the graph should exist.
	
	//DrawlineOverlay(m_hwndVideo , 0 , 200 ,500 , 200 );

	HRESULT hr = m_pControl->Run();

	if (SUCCEEDED(hr))
	{
		m_state = STATE_RUNNING;
	}

	return hr;
}

HRESULT DShowPlayer::Pause()
{
	if (m_state != STATE_RUNNING)
	{
		return VFW_E_WRONG_STATE;
	}

	assert(m_pGraph); // If state is correct, the graph should exist.

	HRESULT hr = m_pControl->Pause();

	if (SUCCEEDED(hr))
	{
		m_state = STATE_PAUSED;
	}

	return hr;
}


HRESULT DShowPlayer::Stop()
{
	if (m_state != STATE_RUNNING && m_state != STATE_PAUSED)
	{
		return VFW_E_WRONG_STATE;
	}

	assert(m_pGraph); // If state is correct, the graph should exist.

	HRESULT hr = m_pControl->Stop();

	if (SUCCEEDED(hr))
	{
		m_state = STATE_STOPPED;
	}

	return hr;
}


// EVR/VMR functionality


BOOL DShowPlayer::HasVideo() const
{
    return (m_pVideo && m_pVideo->HasVideo());
}


//-----------------------------------------------------------------------------
// DShowPlayer::UpdateVideoWindow
// Description: Sets the destination rectangle for the video.
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::UpdateVideoWindow(const LPRECT prc)
{
    HRESULT hr = S_OK;

    if (m_pVideo)
    {
        hr = m_pVideo->UpdateVideoWindow(m_hwndVideo, prc);
    }

    return hr;
}

void DShowPlayer::SelectDecoder(SELECTED_DECODER selectedDecoder)
{
	 
	m_selectedDecoder = selectedDecoder;
}

//-----------------------------------------------------------------------------
// DShowPlayer::Repaint
// Description: Repaints the video.
//
// Call this method when the application receives WM_PAINT.
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::Repaint(HDC hdc)
{
    HRESULT hr = S_OK;

    if (m_pVideo)
    {
        hr = m_pVideo->Repaint(m_hwndVideo, hdc);
    }

    return hr;
}


//-----------------------------------------------------------------------------
// DShowPlayer::DisplayModeChanged
// Description: Notifies the VMR that the display mode changed.
//
// Call this method when the application receives WM_DISPLAYCHANGE.
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::DisplayModeChanged()
{
    HRESULT hr = S_OK;

    if (m_pVideo)
    {
        hr = m_pVideo->DisplayModeChanged();
    }

    return hr;
}

// Seeking


//-----------------------------------------------------------------------------
// DShowPlayer::CanSeek
// Description: Returns TRUE if the current file is seekable.
//-----------------------------------------------------------------------------

BOOL DShowPlayer::CanSeek() const
{
	const DWORD caps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanGetDuration;
	return ((m_seekCaps & caps) == caps);
}


//-----------------------------------------------------------------------------
// DShowPlayer::SetPosition
// Description: Seeks to a new position.
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::SetPosition(REFERENCE_TIME pos)
{
	if (m_pControl == NULL || m_pSeek == NULL)
	{
		return E_UNEXPECTED;
	}

	HRESULT hr = S_OK;

	hr = m_pSeek->SetPositions(&pos, AM_SEEKING_AbsolutePositioning,
		NULL, AM_SEEKING_NoPositioning);

	if (SUCCEEDED(hr))
	{
		// If playback is stopped, we need to put the graph into the paused
		// state to update the video renderer with the new frame, and then stop 
		// the graph again. The IMediaControl::StopWhenReady does this.
		if (m_state == STATE_STOPPED)
		{
			hr = m_pControl->StopWhenReady();
		}
	}

	return hr;
}

//-----------------------------------------------------------------------------
// DShowPlayer::GetDuration
// Description: Gets the duration of the current file.
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::GetDuration(LONGLONG *pDuration)
{
	if (m_pSeek == NULL)
	{
		return E_UNEXPECTED;
	}

	return m_pSeek->GetDuration(pDuration);
}

//-----------------------------------------------------------------------------
// DShowPlayer::GetCurrentPosition
// Description: Gets the current playback position.
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::GetCurrentPosition(LONGLONG *pTimeNow)
{
	if (m_pSeek == NULL)
	{
		return E_UNEXPECTED;
	}

	return m_pSeek->GetCurrentPosition(pTimeNow);
}


// Audio

//-----------------------------------------------------------------------------
// DShowPlayer::Mute
// Description: Mutes or unmutes the audio.
//-----------------------------------------------------------------------------

HRESULT	DShowPlayer::Mute(BOOL bMute)
{
	m_bMute = bMute;
	return UpdateVolume();
}

//-----------------------------------------------------------------------------
// DShowPlayer::SetVolume
// Description: Sets the volume. 
//-----------------------------------------------------------------------------

HRESULT	DShowPlayer::SetVolume(long lVolume)
{
	m_lVolume = lVolume;
	return UpdateVolume();
}


//-----------------------------------------------------------------------------
// DShowPlayer::UpdateVolume
// Description: Update the volume after a call to Mute() or SetVolume().
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::UpdateVolume()
{
	HRESULT hr = S_OK;

	if (m_bAudioStream && m_pAudio)
	{
        // If the audio is muted, set the minimum volume. 
		if (m_bMute)
		{
			hr = m_pAudio->put_Volume(MIN_VOLUME);
		}
		else
		{
			// Restore previous volume setting
			hr = m_pAudio->put_Volume(m_lVolume);
		}
	}

	return hr;
}




// Graph building


//-----------------------------------------------------------------------------
// DShowPlayer::InitializeGraph
// Description: Create a new filter graph. (Tears down the old graph.) 
//-----------------------------------------------------------------------------

HRESULT DShowPlayer::InitializeGraph()
{
	HRESULT hr = S_OK;

	TearDownGraph();

	 
	// Create the Filter Graph Manager.
	/*
	hr = CoCreateInstance(
		CLSID_FilterGraph, 
		NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder,
		(void**)&m_pGraph
		);
	*/

	// Use its member function CoCreateInstance to
	// create the COM object and obtain the IGraphBuilder pointer.
	hr = m_pGraph.CoCreateInstance(CLSID_FilterGraph);

	// Query for graph interfaces.
	if (SUCCEEDED(hr))
	{
		hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pControl);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (void**)&m_pEvent);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pGraph->QueryInterface(IID_IMediaSeeking, (void**)&m_pSeek);
	}

#if 0 
	if (SUCCEEDED(hr))
	{
		hr = m_pGraph->QueryInterface(IID_IBasicAudio, (void**)&m_pAudio);
	}
#endif 


	// Set up event notification.
	if (SUCCEEDED(hr))
	{
		hr = m_pEvent->SetNotifyWindow((OAHWND)m_hwndEvent, m_EventMsg, NULL);
	}

	return hr;
}

//-----------------------------------------------------------------------------
// DShowPlayer::TearDownGraph
// Description: Tear down the filter graph and release resources. 
//-----------------------------------------------------------------------------


void DShowPlayer::Close()
{

	if (pSinkFilter != NULL)
		m_pGraph->RemoveFilter(pSinkFilter);

	if (pInfTeeFilter != NULL)
		m_pGraph->RemoveFilter(pInfTeeFilter);

	if (pAudioRenderer != NULL)
		m_pGraph->RemoveFilter(pAudioRenderer);

	if (pVideoDecoder != NULL)
		m_pGraph->RemoveFilter(pVideoDecoder);


	//if (pLeadToolsRTSPSource != NULL)
		//m_pGraph->RemoveFilter(pLeadToolsRTSPSource);

	if (m_pVideo != NULL)
		m_pVideo->FinalizeGraph(m_pGraph);
	

	/*
	// Enumerate the filters in the graph.
	CComPtr<IEnumFilters> pEnum = NULL;
	HRESULT hr = m_pGraph->EnumFilters(&pEnum);
	if (SUCCEEDED(hr))
	{
		IBaseFilter *pFilter = NULL;
		while (S_OK == pEnum->Next(1, &pFilter, NULL))
		{
			FILTER_INFO FilterInfo;
			hr = pFilter->QueryFilterInfo(&FilterInfo);
			// Remove the filter.
			m_pGraph->RemoveFilter(pFilter);
			// Reset the enumerator.
			pEnum->Reset();
			pFilter->Release();
		}
		//pEnum->Release();
	}
	*/
	
	TearDownGraph();

	CoUninitialize();

}

void DShowPlayer::TearDownGraph()
{
	// Stop sending event messages
	if (m_pEvent)
	{
		m_pEvent->SetNotifyWindow((OAHWND)NULL, NULL, NULL);
	}
	 

    SAFE_DELETE(m_pVideo);

	m_state = STATE_CLOSED;
	m_seekCaps = 0;

    m_bAudioStream = FALSE;
}


HRESULT DShowPlayer::CreateVideoRenderer()
{
    HRESULT hr = E_FAIL;

    enum { Try_EVR, Try_VMR9, Try_VMR7 };

    for (DWORD i = Try_EVR; i <= Try_VMR7; i++)
    {
        switch (i)
        {
        case Try_EVR:
            m_pVideo = new EVR();
            break;

        case Try_VMR9:
            m_pVideo = new VMR9();
            break;

        case Try_VMR7:
            m_pVideo = new VMR7();
            break;
        }

        if (m_pVideo == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        hr = m_pVideo->AddToGraph(m_pGraph, m_hwndVideo);
        if (SUCCEEDED(hr))
        {
            break;
        }

        SAFE_DELETE(m_pVideo);
    }

    if (FAILED(hr))
    {
        SAFE_DELETE(m_pVideo);
    }
    return hr;
}

HRESULT DShowPlayer::CreateVideoRenderer(VIDEO_RENDER render)
{
	HRESULT hr = E_FAIL;
	 
	 
	switch (render)
	{
	case Try_EVR:
		m_pVideo = new EVR();
		break;

	case Try_VMR9:
		m_pVideo = new VMR9();
		break;

	case Try_VMR7:
		m_pVideo = new VMR7();
		break;
	}

	if (m_pVideo == NULL)
	{
		hr = E_OUTOFMEMORY;
		return hr;
	}

	hr = m_pVideo->AddToGraph(m_pGraph, m_hwndVideo);
	if (SUCCEEDED(hr))
	{
		return hr;
	}

	SAFE_DELETE(m_pVideo);
	 
	 
	return hr;
}

void DShowPlayer::SetVideoWindow(HWND hwnd)
{
	m_pVideo->SetVideoWindow(hwnd);
	m_pVideo->Repaint(hwnd, 0);
}



//-----------------------------------------------------------------------------
// DShowPlayer::RenderStreams
// Description: Render the streams from a source filter. 
//-----------------------------------------------------------------------------

HRESULT	DShowPlayer::RenderStreams(IBaseFilter *pSource)
{
	HRESULT hr = S_OK;

	BOOL bRenderedAnyPin = FALSE;

	IFilterGraph2 *pGraph2 = NULL;
	IEnumPins *pEnum = NULL;
	IBaseFilter *pAudioRenderer = NULL;
	IBaseFilter *pElecardVideoRenderer = NULL;

	hr = m_pGraph->QueryInterface(IID_IFilterGraph2, (void**)&pGraph2);

    // Add the video renderer to the graph
    if (SUCCEEDED(hr))
    {
        hr = CreateVideoRenderer();
    }

#if 0 
	// Add the DSound Renderer to the graph.
	if (SUCCEEDED(hr))
	{
		hr = AddFilterByCLSID(m_pGraph, CLSID_DSoundRender, &pAudioRenderer, L"Audio Renderer");
	}
#endif 

	if (m_selectedDecoder == SELECTED_DECODER::ELECARD && SUCCEEDED(hr))
	{
		// {5C122C6D-8FCC-46f9-AAB7-DCFB0841E04D}
		static const GUID CLSID_EAVCDEC =
		{ 0x5c122c6D, 0x8fcc, 0x46f9,{ 0xaa, 0xb7, 0xdc, 0xfb, 0x8, 0x41, 0xe0, 0x4d } };

		hr = AddFilterByCLSID(m_pGraph, CLSID_EAVCDEC, &pElecardVideoRenderer, L"Elecard ACV Video Decoder");
	}

	if (m_selectedDecoder == SELECTED_DECODER::LEADTOOLS && SUCCEEDED(hr))
	{
		// {5C122C6D-8FCC-46f9-AAB7-DCFB0841E04D}
		static const GUID CLSID_EAVCDEC =
		{ 0x5c122c6D, 0x8fcc, 0x46f9,{ 0xaa, 0xb7, 0xdc, 0xfb, 0x8, 0x41, 0xe0, 0x4d } };

		hr = AddFilterByCLSID(m_pGraph, CLSID_EAVCDEC, &pElecardVideoRenderer, L"Elecard ACV Video Decoder");
	}


    // Enumerate the pins on the source filter.
	if (SUCCEEDED(hr))
	{
		hr = pSource->EnumPins(&pEnum);
	}
	 
	if (SUCCEEDED(hr))
	{
		// Loop through all the pins
		IPin *pPin = NULL;

		while (S_OK == pEnum->Next(1, &pPin, NULL))
		{			
			// Try to render this pin. 
			// It's OK if we fail some pins, if at least one pin renders.
			HRESULT hr2 = pGraph2->RenderEx(pPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);

			pPin->Release();

			if (SUCCEEDED(hr2))
			{
				bRenderedAnyPin = TRUE;
			}
		}
	}


	// Remove un-used renderers.

    // Try to remove the VMR.
	if (SUCCEEDED(hr))
	{
        hr = m_pVideo->FinalizeGraph(m_pGraph);
	}

#if 0 
    // Try to remove the audio renderer.
	if (SUCCEEDED(hr))
	{
    	BOOL bRemoved = FALSE;
		hr = RemoveUnconnectedRenderer(m_pGraph, pAudioRenderer, &bRemoved);

        if (bRemoved)
        {
            m_bAudioStream = FALSE;
        }
        else
        {
            m_bAudioStream = TRUE;
        }
	}
#endif 

	SAFE_RELEASE(pEnum);
	//SAFE_RELEASE(pVMR);
	SAFE_RELEASE(pAudioRenderer);
	SAFE_RELEASE(pGraph2);

	// If we succeeded to this point, make sure we rendered at least one 
	// stream.
	if (SUCCEEDED(hr))
	{
		if (!bRenderedAnyPin)
		{
			hr = VFW_E_CANNOT_RENDER;
		}
	}

	return hr;
}


//-----------------------------------------------------------------------------
// DShowPlayer::RemoveUnconnectedRenderer
// Description: Remove a renderer filter from the graph if the filter is
//              not connected. 
//-----------------------------------------------------------------------------

HRESULT RemoveUnconnectedRenderer(IGraphBuilder *pGraph, IBaseFilter *pRenderer, BOOL *pbRemoved)
{
	IPin *pPin = NULL;

	BOOL bRemoved = FALSE;

	// Look for a connected input pin on the renderer.

	HRESULT hr = FindConnectedPin(pRenderer, PINDIR_INPUT, &pPin);
	SAFE_RELEASE(pPin);

	// If this function succeeds, the renderer is connected, so we don't remove it.
	// If it fails, it means the renderer is not connected to anything, so
	// we remove it.

	if (FAILED(hr))
	{
		hr = pGraph->RemoveFilter(pRenderer);
		bRemoved = TRUE;
	}

	if (SUCCEEDED(hr))
	{
		*pbRemoved = bRemoved;
	}

	return hr;
}


HRESULT	DShowPlayer::AddCircle(int id,
	int x,
	int y,
	int radios_w,
	int radios_h,
	COLORREF color,
	int width)
{
	HRESULT hr;

	map<int, Overlay*>::iterator it;
	if ((it = m_overlays.find(id)) != m_overlays.end()) // id already in the map
	{
		Overlay* over = (*it).second;
		over->type = 2;
		over->visible = 1;
		over->color.SetFromCOLORREF(color);
		over->fontSize = 0;

		over->x1 = x;
		over->y1 = y;
		over->radios_w = radios_w;
		over->radios_h = radios_h;
		over->lineWidth = width;

		m_overlays[id] = over;

		return S_OK;
	}
	else {

		Overlay* overlay = new Overlay();
		overlay->type = 2;
		overlay->visible = 1;
		overlay->color.SetFromCOLORREF(color);
		overlay->fontSize = 0;

		overlay->x1 = x;
		overlay->y1 = y;
		overlay->radios_w = radios_w;
		overlay->radios_h = radios_h;
		overlay->lineWidth = width;

		m_overlays[id] = overlay;

		return S_OK;
}




#if 0 
	if (pShapeFilter == NULL)
	{
		return S_FALSE;
	}
	// {B6F36855-D861-4ADB-B76F-5F3CF52410AC}
	static const GUID IID_ITextAdditor =
	{ 0xb6f36855, 0xd861, 0x4adb,{ 0xb7, 0x6f, 0x5f, 0x3c, 0xf5, 0x24, 0x10, 0xac } };

	if (pShapeFilterInterface == NULL)
	{
		hr = pShapeFilter->QueryInterface(IID_ITextAdditor, (void **)&pShapeFilterInterface);
		if (FAILED(hr))
		{
			return hr;

		}
	}
 

	return pShapeFilterInterface->AddCircle(id,
		x1,
		y1,
		radios_w,
		radios_h,
		color,
		width);

#endif 

}
 
HRESULT	DShowPlayer::AddTextOverlay(WCHAR *text,
	int id,
	int left,
	int top,
	int right,
	int bottom,
	int color,
	float fontSize,
	int fontStyle)
{

	HRESULT hr;

	map<int, Overlay*>::iterator it;
	if ((it = m_overlays.find(id)) != m_overlays.end()) // id already in the map
	{

		Overlay* over = (*it).second;
		over->type = 0;
		over->visible = 1;
		over->fontStyle = fontStyle;
		over->color.SetFromCOLORREF(color);
		over->fontSize = fontSize;
		RectF rcBounds(left, top, right, bottom);
		over->pos = rcBounds;
		m_overlays[id] = over;

	}
	else {

		Overlay* overlay = new Overlay(text);
		overlay->type = 0;
		overlay->visible = 1;
		overlay->fontStyle = fontStyle;
		overlay->color.SetFromCOLORREF(color);
		overlay->fontSize = fontSize;
		RectF rcBounds(left, top, right, bottom);
		overlay->pos = rcBounds;
		m_overlays[id] = overlay;

	}

	return S_OK;

#if 0 

	if (pShapeFilter == NULL)
	{
		return S_FALSE;
	}

	// {B6F36855-D861-4ADB-B76F-5F3CF52410AC}
	static const GUID IID_ITextAdditor =
	{ 0xb6f36855, 0xd861, 0x4adb,{ 0xb7, 0x6f, 0x5f, 0x3c, 0xf5, 0x24, 0x10, 0xac } };

	if (pShapeFilterInterface == NULL)
	{
		hr = pShapeFilter->QueryInterface(IID_ITextAdditor, (void **)&pShapeFilterInterface);
		if (FAILED(hr))
		{
			return hr;

		}
	}
	
	return pShapeFilterInterface->AddTextOverlay(text,
											 id,
											 left,
											 top,
											 right,
											 bottom,
											 color,
											fontSize,
											 fontStyle);


	#endif 
} 

HRESULT	DShowPlayer::AddTextOverlay2(WCHAR *text, int id,
	int left,
	int top,
	int right,
	int bottom,
	int color,
	float fontSize)
{


	map<int, Overlay*>::iterator it;
	if ((it = m_overlays.find(id)) != m_overlays.end()) // id already in the map
	{
		Overlay* over = (*it).second;
		over->type = 0;
		over->visible = 1;
		over->fontStyle = FontStyle::FontStyleRegular;
		over->color.SetFromCOLORREF(color);
		over->fontSize = fontSize;
		RectF rcBounds(left, top, right, bottom);
		over->pos = rcBounds;
		m_overlays[id] = over;

	}
	else
	{

		Overlay* overlay = new Overlay(text);
		overlay->type = 0;
		overlay->visible = 1;
		overlay->fontStyle = FontStyle::FontStyleRegular;
		overlay->color.SetFromCOLORREF(color);
		overlay->fontSize = fontSize;
		RectF rcBounds(left, top, right, bottom);
		overlay->pos = rcBounds;
		m_overlays[id] = overlay;
	}

	return S_OK;

#if 0 
	HRESULT hr;
	if (pShapeFilter == NULL)
	{
		return S_FALSE;
	}

	// {B6F36855-D861-4ADB-B76F-5F3CF52410AC}
	static const GUID IID_ITextAdditor =
	{ 0xb6f36855, 0xd861, 0x4adb,{ 0xb7, 0x6f, 0x5f, 0x3c, 0xf5, 0x24, 0x10, 0xac } };

	if (pShapeFilterInterface == NULL)
	{
		hr = pShapeFilter->QueryInterface(IID_ITextAdditor, (void **)&pShapeFilterInterface);
		if (FAILED(hr))
		{
			return hr;

		}
	}

	return pShapeFilterInterface->AddTextOverlay2(text, id,
												  left,
												  top,
												  right,
												  bottom,
												  color,
												  fontSize);
	#endif 
}


HRESULT	DShowPlayer::Clear()
{

	map<int, Overlay*>::iterator it;
	for (it = m_overlays.begin(); it != m_overlays.end(); it++)
	{
		Overlay* over = (*it).second;
		over->visible = 0;
	}
	return S_OK;
#if 0 
	HRESULT hr;
	if (pShapeFilter == NULL)
	{
		return S_FALSE;
	}

	// {B6F36855-D861-4ADB-B76F-5F3CF52410AC}
	static const GUID IID_ITextAdditor =
	{ 0xb6f36855, 0xd861, 0x4adb,{ 0xb7, 0x6f, 0x5f, 0x3c, 0xf5, 0x24, 0x10, 0xac } };

	if (pShapeFilterInterface == NULL)
	{
		hr = pShapeFilter->QueryInterface(IID_ITextAdditor, (void **)&pShapeFilterInterface);
		if (FAILED(hr))
		{
			return hr;
		}
	}
	return pShapeFilterInterface->Clear();
#endif 

}
HRESULT	DShowPlayer::Visible(int id, bool visible)
{
	 
	if (m_overlays[id] != NULL)
	{
		m_overlays[id]->visible = visible;
		 
	}
	return S_OK;
#if 0 
	HRESULT hr;
	if (pShapeFilter == NULL)
	{
		return S_FALSE;
	}

	// {B6F36855-D861-4ADB-B76F-5F3CF52410AC}
	static const GUID IID_ITextAdditor =
	{ 0xb6f36855, 0xd861, 0x4adb,{ 0xb7, 0x6f, 0x5f, 0x3c, 0xf5, 0x24, 0x10, 0xac } };

	if (pShapeFilterInterface == NULL)
	{
		hr = pShapeFilter->QueryInterface(IID_ITextAdditor, (void **)&pShapeFilterInterface);
		if (FAILED(hr))
		{
			return hr;

		}
	}
	return pShapeFilterInterface->Visible(id, visible);
#endif 

}

HRESULT	DShowPlayer::AddLine(int id,
	int x1,
	int y1,
	int x2,
	int y2,
	int color,
	int width)
{
	HRESULT hr;
	 

	map<int, Overlay*>::iterator it;
	if ((it = m_overlays.find(id)) != m_overlays.end()) // id already in the map
	{
		Overlay* over = (*it).second;
		over->type = 1;
		over->visible = 1;
		over->color.SetFromCOLORREF(color);
		over->fontSize = 0;

		over->x1 = x1;
		over->x2 = x2;
		over->y1 = y1;
		over->y2 = y2;
		over->lineWidth = width;

		m_overlays[id] = over;

		return S_OK;
	}
	else {

		Overlay* overlay = new Overlay();
		overlay->type = 1;
		overlay->visible = 1;
		overlay->color.SetFromCOLORREF(color);
		overlay->fontSize = 0;

		overlay->x1 = x1;
		overlay->x2 = x2;
		overlay->y1 = y1;
		overlay->y2 = y2;
		overlay->lineWidth = width;

		m_overlays[id] = overlay;

		return S_OK;
	}
	 
	

#if 0 
	

	if (pShapeFilter == NULL)
	{
		return S_FALSE;
	}

	// {B6F36855-D861-4ADB-B76F-5F3CF52410AC}
	static const GUID IID_ITextAdditor =
	{ 0xb6f36855, 0xd861, 0x4adb,{ 0xb7, 0x6f, 0x5f, 0x3c, 0xf5, 0x24, 0x10, 0xac } };

	if (pShapeFilterInterface == NULL)
	{
		hr = pShapeFilter->QueryInterface(IID_ITextAdditor, (void **)&pShapeFilterInterface);
		if (FAILED(hr))
		{
			return hr;
		}
	}
	  

	return pShapeFilterInterface->AddLine(id,
		x1,
		y1,
		x2,
		y2,
		color,
		width);
#endif 

}

 
 
