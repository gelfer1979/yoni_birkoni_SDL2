#include "stdafx.h"
#include <dshow.h>
#include <malloc.h>
#include "dshow.h"

bool loopp;
// DirectShow Graph, Filter & Pins used
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

IGraphBuilder *g_pGraphBuilder = NULL;
IMediaControl *g_pMediaControl = NULL;
IMediaSeeking *g_pMediaSeeking = NULL;
IMediaEventEx	  *g_pMediaEventEx = NULL;
IBaseFilter   *g_pSourceCurrent = NULL;
IBaseFilter   *g_pSourceNext = NULL;
IPin *pPin = NULL;


WCHAR wFileName[MAX_PATH];


bool dsinit(HWND mwnd)//init dshow interface(mwnd-application window handle)
{
	 HRESULT hr;
    // Initialize COM
    if (FAILED (hr = CoInitialize(NULL)) )
        return false;

    // Create DirectShow Graph
    if (FAILED (hr = CoCreateInstance(CLSID_FilterGraph, NULL,
                                      CLSCTX_INPROC, IID_IGraphBuilder,
                                      reinterpret_cast<void **>(&g_pGraphBuilder))) )
        return false;

    // Get the IMediaControl Interface
    if (FAILED (g_pGraphBuilder->QueryInterface(IID_IMediaControl,
                                 reinterpret_cast<void **>(&g_pMediaControl))))
        return false;

    // Get the IMediaControl Interface
    if (FAILED (g_pGraphBuilder->QueryInterface(IID_IMediaSeeking,
                                 reinterpret_cast<void **>(&g_pMediaSeeking))))
        return false;

	// Get the IMediaEvent Interface
    if (FAILED (g_pGraphBuilder->QueryInterface(IID_IMediaEventEx,
                                 reinterpret_cast<void **>(&g_pMediaEventEx))))
        return false;
	hr=g_pMediaEventEx->SetNotifyWindow((OAHWND)mwnd,WM_DSHOW,0);
	if (SUCCEEDED(hr)) return true; else return false;
}

bool dsstop()
{
	HRESULT hr;
	if (g_pMediaControl)
        hr=g_pMediaControl->Stop();
	else return false;
	if (SUCCEEDED(hr)) return true;
		else return false;
}

bool dsstart()
{
	HRESULT hr;
	if (g_pMediaControl)
        hr=g_pMediaControl->Run();
	if (SUCCEEDED(hr)) return true;
		else return false;
}

void dsclose(void)//close dshow interface
{
    // Stop playback
    if (g_pMediaControl)
        g_pMediaControl->Stop();

    // Release all remaining pointers
    SAFE_RELEASE( g_pSourceNext);
    SAFE_RELEASE( g_pSourceCurrent);
    SAFE_RELEASE( g_pMediaSeeking);
    SAFE_RELEASE( g_pMediaControl);
    SAFE_RELEASE( g_pGraphBuilder);
	SAFE_RELEASE( g_pMediaEventEx);

    // Clean up COM
    CoUninitialize();
    return;
}

bool dsplay(char *fname,bool loop)
{
	HRESULT hr;
//set file name to play
	#ifndef UNICODE
        MultiByteToWideChar(CP_ACP, 0,fname, -1, wFileName, MAX_PATH);
    #else
        lstrcpy(wFileName,fname);
    #endif

	    // Add the new source filter to the graph. (Graph can still be running)
    hr = g_pGraphBuilder->AddSourceFilter(wFileName, wFileName, &g_pSourceNext);

    // Get the first output pin of the new source filter. Audio sources 
    // typically have only one output pin, so for most audio cases finding 
    // any output pin is sufficient.
    if (SUCCEEDED(hr)) {
        hr = g_pSourceNext->FindPin(L"Output", &pPin);  
    }

    // Stop the graph
    if (SUCCEEDED(hr)) {
        hr = g_pMediaControl->Stop();
    }

    // Break all connections on the filters. You can do this by adding 
    // and removing each filter in the graph
    if (SUCCEEDED(hr)) {
        IEnumFilters *pFilterEnum = NULL;
        IBaseFilter  *pFilterTemp = NULL;

        if (SUCCEEDED(hr = g_pGraphBuilder->EnumFilters(&pFilterEnum))) {
            int iFiltCount = 0;
            int iPos = 0;

            // Need to know how many filters. If we add/remove filters during the
            // enumeration we'll invalidate the enumerator
            while (S_OK == pFilterEnum->Skip(1)) {
                iFiltCount++;
            }

            // Allocate space, then pull out all of the 
            IBaseFilter **ppFilters = reinterpret_cast<IBaseFilter **>
                                      (_alloca(sizeof(IBaseFilter *) * iFiltCount));
            pFilterEnum->Reset();

            while (S_OK == pFilterEnum->Next(1, &(ppFilters[iPos++]), NULL));
            SAFE_RELEASE(pFilterEnum);

            for (iPos = 0; iPos < iFiltCount; iPos++) {
                g_pGraphBuilder->RemoveFilter(ppFilters[iPos]);
				// Put the filter back, unless it is the old source
				if (ppFilters[iPos] != g_pSourceCurrent) {
					g_pGraphBuilder->AddFilter(ppFilters[iPos], NULL);
                }
				SAFE_RELEASE(ppFilters[iPos]);
            }
        }
    }

    // We have the new ouput pin. Render it
    if (SUCCEEDED(hr)) {
        hr = g_pGraphBuilder->Render(pPin);
        g_pSourceCurrent = g_pSourceNext;
        g_pSourceNext = NULL;
    }

    SAFE_RELEASE(pPin);
    SAFE_RELEASE(g_pSourceNext); // In case of errors

    // Re-seek the graph to the beginning
    if (SUCCEEDED(hr)) {
        LONGLONG llPos = 0;
        hr = g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
                                           &llPos, AM_SEEKING_NoPositioning);
    } 

    // Start the graph
    if (SUCCEEDED(hr)) {
        hr = g_pMediaControl->Run();
    }

    // Release the old source filter.
    SAFE_RELEASE(g_pSourceCurrent);

    if (SUCCEEDED(hr)) {loopp=loop;return true;} else {loopp=false;return false;}
}

void dsevent()
{
	LONG evCode, evParam1, evParam2;
    HRESULT hr=S_OK;

    while(SUCCEEDED(g_pMediaEventEx->GetEvent(&evCode, &evParam1, &evParam2, 0)))
    {
        // Spin through the events
        hr = g_pMediaEventEx->FreeEventParams(evCode, evParam1, evParam2);

        if(EC_COMPLETE == evCode)
        {
            // If looping, reset to beginning and continue playing
            if (loopp)
            {
                LONGLONG pos=0;

                // Reset to first frame of movie
                hr = g_pMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,
                                       NULL, AM_SEEKING_NoPositioning);
			}
		}
	}
}