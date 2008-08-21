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

SDLAudio::SDLAudio(unsigned long _sr, AYSongInfo *info) :
    AbstractAudio(_sr, info)
{
    songinfo = info;
    ay8910 = new ay(songinfo, 1024 * 2 * 2); // 16 bit, 2 ch.

}

SDLAudio::~SDLAudio()
{
    Stop();
    if(ay8910)
    {
        delete ay8910;
        ay8910 = 0;
    }
}

bool SDLAudio::Start()
{
    if(!started)
    {
        SDL_AudioSpec fmt;
        SDL_memset(&fmt, 0, sizeof(SDL_AudioSpec));
        fmt.callback = SDLAudio::Play;
        fmt.channels = 2;
        fmt.format = AUDIO_S16;
        fmt.samples = 1024 * 2 * 2;
        fmt.freq = sr;
        fmt.userdata = this;
        if(SDL_OpenAudio(&fmt, &fmt_out) < 0)
        {
            return false;
        }

        if(ay8910 == 0)
            ay8910 = new ay(songinfo, fmt_out.size >> 2); // 16 bit, 2 ch.
        else
            ay8910->SetBufferSize(fmt_out.size >> 2);
        SDL_PauseAudio(0);
        started = true;
    }
    return started;

}

void SDLAudio::Stop()
{
    if(started)
    {
        started = false;
        SDL_PauseAudio(1);
        SDL_CloseAudio();
    }

}

void SDLAudio::Play(void *udata, Uint8 *stream, int len)
{
    SDLAudio *audio = (SDLAudio *)udata;
    audio->ay8910->ayProcess(stream, len);
}
