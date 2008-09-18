/***************************************************************************
*   Copyright (C) 2008 by Deryabin Andrew   				               *
*   andrew@it-optima.ru                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "ayfly.h"

const UINT DefBufferLength      = 100;
const UINT DefBufferServiceInterval  = ((DefBufferLength / 2) - 20);

DXAudio::DXAudio(AYSongInfo *info) :
AbstractAudio(info)
{
    buffer_size = 16384;
    hNotifyEvent1 = CreateEvent(NULL, FALSE, FALSE, NULL);
    hNotifyEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);
    hSyncEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    pDsb8 = 0;
    lpds = 0;    
    dwOffset = 0;
    OleInitialize(0);
}

DXAudio::~DXAudio()
{
    Stop();
    CloseHandle(hNotifyEvent1);
    CloseHandle(hNotifyEvent2);
    CloseHandle(hSyncEvent);
    OleUninitialize();
}

bool DXAudio::Start()
{
    dx_created = false;    
    if(SUCCEEDED(CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC_SERVER, IID_IDirectSound8, (LPVOID *)&lpds)))
        if(SUCCEEDED(lpds->Initialize(NULL)))
            if(SUCCEEDED(lpds->SetCooperativeLevel(hWndMain, DSSCL_PRIORITY)))
                if(SUCCEEDED(CreateBasicBuffer()))
                    dx_created = true;
    if(dx_created)
    {
        
        ResetEvent(hSyncEvent);
        ResetEvent(hNotifyEvent1);
        ResetEvent(hNotifyEvent2);        
        InterlockedExchange(&srvExch, FALSE);
        m_nIDTimer = 0;
        HRESULT hr = pDsb8->Play (0, 0, DSBPLAY_LOOPING);
        if(SUCCEEDED(hr))
        {
            m_nIDTimer = timeSetEvent(DefBufferServiceInterval, DefBufferServiceInterval, 
                                      TimerProc, (DWORD_PTR)this, TIME_PERIODIC);
            started = true;
        }
        //hPlayingThread = CreateThread(NULL, 0, PlayingThread, this, 0, 0)

    }
    return started;

}

void DXAudio::Stop()
{
    if (started)
    {
        pDsb8->Stop();
        timeKillEvent(m_nIDTimer);
        started = false;
        //SetEvent(hSyncEvent);
        //WaitForSingleObject(hPlayingThread, INFINITE);
        //hPlayingThread = 0;
        ClearBuffer();
        ResetEvent(hSyncEvent);
        ResetEvent(hNotifyEvent1);
        ResetEvent(hNotifyEvent2);
    }

    if(pDsb8)
    {
        while(pDsb8->Release()) {};
        pDsb8 = 0;
    }
    if(lpds)
    {
        while(lpds->Release()){};
        lpds = 0;
    }
}

HRESULT DXAudio::CreateBasicBuffer()
{
    WAVEFORMATEX wfx;
    DSBUFFERDESC dsbdesc;
    LPDIRECTSOUNDBUFFER pDsb = NULL;
    HRESULT hr;

    // Set up WAV format structure.

    memset(&wfx, 0, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = songinfo->sr;
    wfx.nBlockAlign = 4;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.wBitsPerSample = 16;

    // Set up DSBUFFERDESC structure.

    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_GLOBALFOCUS;
    dsbdesc.dwBufferBytes = (wfx.nAvgBytesPerSec * DefBufferLength) / 1000;
    dsbdesc.lpwfxFormat = &wfx;

    // Create buffer.

    hr = lpds->CreateSoundBuffer(&dsbdesc, &pDsb, NULL);
    if (SUCCEEDED(hr))
    {
        hr = pDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*) &pDsb8);
        pDsb->Release();
    }
    DSBCAPS bCaps;
    RtlZeroMemory(&bCaps, sizeof(DSBCAPS));
    bCaps.dwSize = sizeof(DSBCAPS);
    hr = pDsb8->GetCaps(&bCaps);
    if(SUCCEEDED(hr))
    {
        buffer_size = bCaps.dwBufferBytes;
    }
    return hr;

}

HRESULT DXAudio::SetNotificationPositions()
{
    LPDIRECTSOUNDNOTIFY8 lpDsNotify;
    DSBPOSITIONNOTIFY PositionNotify [2];
    HRESULT hr;

    if (SUCCEEDED(hr = pDsb8->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&lpDsNotify)))
    {
        PositionNotify [0].dwOffset = 0;
        PositionNotify [0].hEventNotify = hNotifyEvent1;
        PositionNotify [1].dwOffset = buffer_size >> 1;
        PositionNotify [1].hEventNotify = hNotifyEvent2;
        hr = lpDsNotify->SetNotificationPositions(2, PositionNotify);
        lpDsNotify->Release();
    }
    return hr;

}

void DXAudio::FillBuffer(DWORD dwSoundBytes)
{
    LPVOID  lpvPtr1;
    DWORD dwBytes1;
    LPVOID  lpvPtr2;
    DWORD dwBytes2;
    HRESULT hr;

    // Obtain memory address of write block. This will be in two parts
    // if the block wraps around.

    hr = pDsb8->Lock(dwOffset, dwSoundBytes, &lpvPtr1,
        &dwBytes1, &lpvPtr2, &dwBytes2, 0);

    // If the buffer was lost, restore and retry lock.

    if (DSERR_BUFFERLOST == hr)
    {
        pDsb8->Restore();
        hr = pDsb8->Lock(dwOffset, dwSoundBytes,
            &lpvPtr1, &dwBytes1,
            &lpvPtr2, &dwBytes2, 0);
    }
    if (SUCCEEDED(hr))
    {
        // Write to pointers.
        ay8910->ayProcess((unsigned char *)lpvPtr1, dwBytes1);
        if (NULL != lpvPtr2)
        {
            ay8910->ayProcess((unsigned char *)lpvPtr2, dwBytes2);
        }

        // Release the data back to DirectSound.

        dwOffset = (dwOffset + dwBytes1 + dwBytes2) % buffer_size;

        hr = pDsb8->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
    }

    // Lock, Unlock, or Restore failed.

}

void DXAudio::ClearBuffer()
{
    LPVOID  lpvPtr1;
    DWORD dwBytes1;
    LPVOID  lpvPtr2;
    DWORD dwBytes2;
    HRESULT hr;

    dwOffset = 0;
    DWORD dwSoundBytes = buffer_size;

    // Obtain memory address of write block. This will be in two parts
    // if the block wraps around.

    hr = pDsb8->Lock(dwOffset, dwSoundBytes, &lpvPtr1,
        &dwBytes1, &lpvPtr2, &dwBytes2, 0);

    // If the buffer was lost, restore and retry lock.

    if (DSERR_BUFFERLOST == hr)
    {
        pDsb8->Restore();
        hr = pDsb8->Lock(dwOffset, dwSoundBytes,
            &lpvPtr1, &dwBytes1,
            &lpvPtr2, &dwBytes2, 0);
    }
    if (SUCCEEDED(hr))
    {
        // Write to pointers.
        RtlZeroMemory(lpvPtr1, dwBytes1);
        if (NULL != lpvPtr2)
        {
            RtlZeroMemory(lpvPtr2, dwBytes2);
        }

        // Release the data back to DirectSound.

        hr = pDsb8->Unlock(lpvPtr1, dwBytes1, lpvPtr2,
            dwBytes2);

    }

    // Lock, Unlock, or Restore failed.

}


void DXAudio::TimerProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD dw1, DWORD dw2)
{
    DXAudio *pdx = (DXAudio *)dwUser;
    pdx->DXProcess();

}

void DXAudio::DXProcess()
{
    if (InterlockedExchange (&srvExch, TRUE) == FALSE)
    {
        DWORD dwWriteCursor, dwPlayCursor, dwMaxSize;

        // Get current play position
        if (pDsb8->GetCurrentPosition (&dwPlayCursor, &dwWriteCursor) == DS_OK)
        {
            if (dwOffset <= dwPlayCursor)
            {
                // Our write position trails play cursor
                dwMaxSize = dwPlayCursor - dwOffset;
            }

            else // (m_cbBufOffset > dwPlayCursor)
            {
                // Play cursor has wrapped
                dwMaxSize = buffer_size - dwOffset + dwPlayCursor;
            }
        }
        else
        {
            // GetCurrentPosition call failed
            dwMaxSize = 0;
        }
        if(dwMaxSize)
        {
            FillBuffer(dwMaxSize);
        }
        
        if(songinfo->stopping)
        {
            songinfo->stopping = false;
            Stop();
            if(songinfo->s_callback)
                songinfo->s_callback(songinfo->s_callback_arg);
        }

        InterlockedExchange (&srvExch, FALSE);        
    }


}

