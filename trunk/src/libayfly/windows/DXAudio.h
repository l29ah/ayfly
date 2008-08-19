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

#ifndef DXAUDIO_H_
#define DXAUDIO_H_

#include "ole2.h"
#include "mmsystem.h"
#ifdef _MSC_VER
#include "strsafe.h"
#endif
#include "dsound.h"
#include "windows/DXAudio.h"

#include "AbstractAudio.h"

class DXAudio: public AbstractAudio
{
public:
    DXAudio(unsigned long _sr, AYSongInfo *info);
    virtual ~DXAudio();
    virtual bool Start(void);
    virtual void Stop();
private:
    LPDIRECTSOUND8 lpds;
    LPDIRECTSOUNDBUFFER8 pDsb8;
    int buffer_size;
    HANDLE hNotifyEvent1;
    HANDLE hNotifyEvent2;
    HANDLE hSyncEvent;
    HANDLE hPlayingThread;
    bool dx_created;
    HRESULT CreateBasicBuffer();
    HRESULT SetNotificationPositions();
    static DWORD pascal PlayingThread(void *arg);
    void FillBuffer(DWORD dwOffset, DWORD dwSoundBytes);
    void ClearBuffer();
    DWORD DXProcess();
    AYSongInfo *songinfo;

};

#endif /*DXAUDIO_H_*/

