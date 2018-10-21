//////////////////////////////////////////////////////////////////////////
// DShowPlayer.h: Implements DirectShow playback functionality.
// 
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0A00		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0600		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0501	// Change this to the appropriate value to target IE 5.0 or later.
#endif

 
 

// Windows Header Files:
#include <windows.h>
#include <strsafe.h>
#include <dshow.h>
#include <atlbase.h>
#include "Overlay.h"

#include <map>
using namespace std;


MIDL_INTERFACE("B6F36855-D861-4ADB-B76F-5F3CF52410AC")
IShapeSourceFilter : public IUnknown
{
public:


	virtual HRESULT STDMETHODCALLTYPE AddTextOverlay(WCHAR* text, int id,
		int    left,
		int    top,
		int    right,
		int    bottom,
		COLORREF color,
		float fontSize,
		int fontStyle) PURE;


	virtual HRESULT STDMETHODCALLTYPE AddTextOverlay2(WCHAR* text,
		int id,
		int    left,
		int    top,
		int    right,
		int    bottom,
		COLORREF color,
		float fontSize) PURE;


	virtual HRESULT STDMETHODCALLTYPE Clear(void) PURE;
	virtual HRESULT STDMETHODCALLTYPE Visible(int id, bool visible) PURE;
	virtual HRESULT STDMETHODCALLTYPE AddLine(int id, int    x1, int   y1, int    x2, int    y2, COLORREF color, int width) PURE;
	virtual HRESULT STDMETHODCALLTYPE AddCircle(int id, int    x1, int   y1, int radios_w, int, COLORREF color, int width) PURE;
	virtual HRESULT STDMETHODCALLTYPE Close();


};

// Include the v6 common controls in the manifest
#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) { if (x) { x->Release(); x = NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { delete x; x = NULL; }
#endif


#include "video.h"

const long MIN_VOLUME = -10000;
const long MAX_VOLUME = 0;

enum PlaybackState
{
	STATE_RUNNING,
	STATE_PAUSED,
	STATE_STOPPED,
	STATE_CLOSED
};

enum  VIDEO_RENDER
{
	Try_EVR, Try_VMR9, Try_VMR7
};

struct GraphEventCallback
{
	virtual void OnGraphEvent(long eventCode, LONG_PTR param1, LONG_PTR param2) = 0;
};


enum SELECTED_DECODER
{
	RENDER,
	ELECARD,
	LEADTOOLS
};


class DShowPlayer
{
public:

	DShowPlayer();
	~DShowPlayer();

	void SetWindowHandle(HWND hwnd);

	HRESULT SetEventWindow(HWND hwnd, UINT msg);

	PlaybackState State() const { return m_state; }

	void SetFileName(const WCHAR* sFileName);
	HRESULT InitilizeRSTPSource(HWND hwnd, const WCHAR *url, bool Audio,  bool SaveToFile, const WCHAR *saveFileName);
	HRESULT InitilizePlayer(HWND hwnd);
	
	
	void Close();
	// Streaming
	HRESULT Play();
	HRESULT Pause();
	HRESULT Stop();

	// VMR functionality
	BOOL    HasVideo() const;
	HRESULT UpdateVideoWindow(const LPRECT prc);
	HRESULT Repaint(HDC hdc);
	HRESULT DisplayModeChanged();
	void SelectDecoder(SELECTED_DECODER selectedDecoder);
	void SetVideoWindow(HWND hwnd);
	// events
	HRESULT HandleGraphEvent(GraphEventCallback *pCB);

	HRESULT ApplyOverlay(float alpha_opacity);

	// seeking
	BOOL	CanSeek() const;
	HRESULT SetPosition(REFERENCE_TIME pos);
	HRESULT GetDuration(LONGLONG *pDuration);
	HRESULT GetCurrentPosition(LONGLONG *pTimeNow);

	// Audio
	HRESULT	Mute(BOOL bMute);
	BOOL	IsMuted() const { return m_bMute; }
	HRESULT	SetVolume(long lVolume);
	long	GetVolume() const { return m_lVolume; }

	// Shapes
	 

	HRESULT	AddCircle(int id, int x1, int y1, int radios_w, int radios_h, COLORREF color, int width);
	HRESULT	AddTextOverlay(WCHAR *text,
		int id,
		int left,
		int top,
		int right,
		int bottom,
		int color,
		float fontSize,
		int fontStyle);

	HRESULT	AddLine(int id,
		int x1,
		int y1,
		int x2,
		int y2,
		int color,
		int width);


	HRESULT	Visible(int id, bool visible);
	HRESULT	Clear();
	HRESULT	AddTextOverlay2(WCHAR *text, int id,
		int left,
		int top,
		int right,
		int bottom,
		int color,
		float fontSize);

	 

private:
	
	void DrawLine1(int xx1, int yy1, int xx2, int yy2, HDC hdcBmp, int Thickness);
	map<int, Overlay*> m_overlays;
	HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin, int num = 0);
	HRESULT InitializeGraph();
	void	TearDownGraph();
    HRESULT CreateVideoRenderer();
	HRESULT CreateVideoRenderer(VIDEO_RENDER render);
	HRESULT	RenderStreams(IBaseFilter *pSource);
	HRESULT UpdateVolume();

	PlaybackState	m_state;

	SELECTED_DECODER m_selectedDecoder;

	HWND			m_hwndVideo;	// Video clipping window
	HWND			m_hwndEvent;	// Window to receive events
	UINT			m_EventMsg;		// Windows message for graph events

	DWORD			m_seekCaps;		// Caps bits for IMediaSeeking

	// Audio
    BOOL            m_bAudioStream; // Is there an audio stream?
	long			m_lVolume;		// Current volume (unless muted)
	BOOL			m_bMute;		// Volume muted?		

	CComPtr<IMediaControl> pControl;

	CComPtr<IGraphBuilder> m_pGraph;
	CComPtr<IMediaControl> m_pControl;
	CComPtr<IMediaEventEx> m_pEvent;
	CComPtr<IMediaSeeking> m_pSeek;
	CComPtr<IBasicAudio> m_pAudio;

	CComPtr<IBaseFilter> pAudioRenderer;
	CComPtr<IBaseFilter> pVideoDecoder;
	CComPtr<IBaseFilter> pLeadToolsRTSPSource;
	CComPtr<IBaseFilter> pInfTeeFilter;
	CComPtr<IBaseFilter> pSinkFilter;
	 
	BaseVideoRenderer *m_pVideo;
	CComPtr<IShapeSourceFilter>  pShapeFilterInterface;

};

HRESULT RemoveUnconnectedRenderer(IGraphBuilder *pGraph, IBaseFilter *pRenderer, BOOL *pbRemoved);
