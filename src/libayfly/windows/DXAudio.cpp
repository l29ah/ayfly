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

const UINT DefBufferLength      = 200;
const UINT DefBufferServiceInterval  = (DefBufferLength / 8);

DXAudio::DXAudio(AYSongInfo *info) :
AbstractAudio(info)
{
	buffer_size = 16384;
	pDsb8 = 0;
	lpds = 0;    
	dwOffset = 0;
	OleInitialize(0);
	dx_created = false;    
	if(SUCCEEDED(CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC_SERVER, IID_IDirectSound8, (LPVOID *)&lpds)))
		if(SUCCEEDED(lpds->Initialize(NULL)))            
			if(SUCCEEDED(CreateBasicBuffer()))
				dx_created = true;
}

DXAudio::~DXAudio()
{
	Stop();
	if(pDsb8)
	{
		pDsb8->Release();
		pDsb8 = 0;
	}
	if(lpds)
	{
		lpds->Release();
		lpds = 0;
	}
}

bool DXAudio::Start()
{

	if(dx_created)
	{
		if(SUCCEEDED(lpds->SetCooperativeLevel(hWndMain, DSSCL_PRIORITY)))
		{

			InterlockedExchange(&srvExch, FALSE);
			m_nIDTimer = 0;
			ClearBuffer();
			HRESULT hr = pDsb8->Play (0, 0, DSBPLAY_LOOPING);
			if(SUCCEEDED(hr))
			{
				m_nIDTimer = timeSetEvent(DefBufferServiceInterval, DefBufferServiceInterval, 
					TimerProc, (DWORD_PTR)this, TIME_PERIODIC);
				started = true;
			}
		}
	}
	return started;

}

void DXAudio::Stop()
{
	if (started)
	{
		pDsb8->Stop();
		DWORD dxStatus;
		timeKillEvent(m_nIDTimer);
		while(1)
		{
			pDsb8->GetStatus(&dxStatus);
			if(!(dxStatus & DSBSTATUS_PLAYING) && (InterlockedExchange (&srvExch, TRUE) == FALSE))
				break;
			Sleep(10);
		}
		ClearBuffer();        
		started = false;
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
	wfx.cbSize = sizeof(wfx);


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
		ay_rendersongbuffer(songinfo, (unsigned char *)lpvPtr1, dwBytes1);
		if (NULL != lpvPtr2)
		{
			ay_rendersongbuffer(songinfo, (unsigned char *)lpvPtr2, dwBytes2);
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

	// Obtain memory address of write block. This will be in two parts
	// if the block wraps around.

	hr = pDsb8->Lock(0, buffer_size, &lpvPtr1,
		&dwBytes1, &lpvPtr2, &dwBytes2, 0);

	// If the buffer was lost, restore and retry lock.

	if (DSERR_BUFFERLOST == hr)
	{
		pDsb8->Restore();
		hr = pDsb8->Lock(dwOffset, buffer_size,
			&lpvPtr1, &dwBytes1,
			&lpvPtr2, &dwBytes2, 0);
	}
	if (SUCCEEDED(hr))
	{
		// Write to pointers.
		memset(lpvPtr1, 0, dwBytes1);
		if (NULL != lpvPtr2)
		{
			memset(lpvPtr2, 0, dwBytes2);
		}

		// Release the data back to DirectSound.

		hr = pDsb8->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
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
			timeKillEvent(m_nIDTimer);
			songinfo->stopping = false;
			InterlockedExchange (&srvExch, FALSE);			
			Stop();
			if(songinfo->s_callback)
				songinfo->s_callback(songinfo->s_callback_arg);
			return;
		}

		InterlockedExchange (&srvExch, FALSE);        
	}


}

