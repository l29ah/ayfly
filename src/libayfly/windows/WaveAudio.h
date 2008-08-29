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

#ifndef WAVEAUDIO_H_
#define WAVEAUDIO_H_

#include "AbstractAudio.h"

class WaveAudio : public AbstractAudio
{
public:
	WaveAudio(unsigned long _sr, AYSongInfo *info);
	virtual ~WaveAudio();
	virtual bool Start(void);
	virtual void Stop();
private:
	HWAVEOUT hwo;
	DWORD bufferSize;
	unsigned char *buffer0, *buffer1;
	HANDLE hEvent, hSyncEvent, hPlayingThread;
	WAVEHDR hdr0, hdr1;
	static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
	static DWORD CALLBACK ThreadProc(void *arg);
	void WaveProcess();
};

#endif /*WAVEAUDIO_H_*/
