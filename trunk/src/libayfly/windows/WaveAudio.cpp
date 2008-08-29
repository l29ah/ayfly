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

WaveAudio::WaveAudio(unsigned long _sr, AYSongInfo *info)
:AbstractAudio(_sr, info)
{
	buffer0 = buffer1 = 0;
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hSyncEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	
	bufferSize = 16384;
	buffer0 = new unsigned char [bufferSize];
	buffer1 = new unsigned char [bufferSize];
}

WaveAudio::~WaveAudio()
{	
	Stop();
	CloseHandle(hEvent);
	if(buffer0)
	{
		delete [] buffer0;
	}
	if(buffer1)
	{
		delete [] buffer1;
	}
}

bool WaveAudio::Start()
{
	WAVEFORMATEX wfx;
	// Set up WAV format structure.

	memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = 44100;
	wfx.nBlockAlign = 4;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.wBitsPerSample = 16;
	hwo = 0;
	started = false;	

	MMRESULT res = waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)hEvent, (DWORD)this, CALLBACK_EVENT | WAVE_FORMAT_DIRECT);
	if(res != MMSYSERR_NOERROR)
		return false;
	

	started = true;
	ResetEvent(hSyncEvent);		
	hPlayingThread = CreateThread(NULL, 0, WaveAudio::ThreadProc, this, 0, 0);
	return true;
}

void WaveAudio::Stop()
{

	started = false;
	SetEvent(hSyncEvent);
	WaitForSingleObject(hPlayingThread, INFINITE);
	if(hwo)
	{
		waveOutClose(hwo);
		hwo = 0;
	}	
	return;
}

void WaveAudio::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	WaveAudio *pwave = (WaveAudio *)dwInstance;
	if(uMsg != WOM_DONE)
		return;

}

DWORD WaveAudio::ThreadProc(void *arg)
{
	WaveAudio *pwave = (WaveAudio *)arg;
	pwave->WaveProcess();
	return 0;
}

void WaveAudio::WaveProcess()
{
	ResetEvent(hEvent);
	SetThreadPriority(hPlayingThread, THREAD_PRIORITY_TIME_CRITICAL);
	memset(&hdr0, 0, sizeof(WAVEHDR));
	memset(&hdr1, 0, sizeof(WAVEHDR));
	hdr0.lpData = (LPSTR)buffer0;
	hdr0.dwBufferLength = bufferSize;
	hdr1.lpData = (LPSTR)buffer1;
	hdr1.dwBufferLength = bufferSize;
	waveOutPrepareHeader(hwo, &hdr0, sizeof(WAVEHDR));
	waveOutPrepareHeader(hwo, &hdr1, sizeof(WAVEHDR));
	ay8910->ayProcess(buffer0, bufferSize);
	ay8910->ayProcess(buffer1, bufferSize);
	waveOutWrite(hwo, &hdr0, sizeof(WAVEHDR));
	waveOutWrite(hwo, &hdr1, sizeof(WAVEHDR));
	WAVEHDR *currentHdr;
	unsigned char *currentBuffer;

	while(WaitForSingleObject(hSyncEvent, 0) != WAIT_OBJECT_0)
	{
		if(WaitForSingleObject(hEvent, INFINITE) != WAIT_OBJECT_0)
			return;
		if(hdr0.dwFlags & WHDR_DONE)
		{
			waveOutUnprepareHeader(hwo, &hdr0, sizeof(WAVEHDR));
			ay8910->ayProcess(buffer0, bufferSize);
			hdr0.lpData = (LPSTR)buffer0;
			hdr0.dwBufferLength = bufferSize;
			waveOutPrepareHeader(hwo, &hdr0, sizeof(WAVEHDR));
			waveOutWrite(hwo, &hdr0, sizeof(WAVEHDR));
		}

		if(hdr1.dwFlags & WHDR_DONE)
		{
			waveOutUnprepareHeader(hwo, &hdr1, sizeof(WAVEHDR));
			ay8910->ayProcess(buffer1, bufferSize);
			hdr0.lpData = (LPSTR)buffer1;
			hdr0.dwBufferLength = bufferSize;
			waveOutPrepareHeader(hwo, &hdr1, sizeof(WAVEHDR));
			waveOutWrite(hwo, &hdr1, sizeof(WAVEHDR));
		}

								
	}
	
}