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
    stopping_thread = 0;
}

SDLAudio::~SDLAudio()
{
    if(stopping_thread)
    {
        SDL_KillThread(stopping_thread);
        stopping_thread = 0;
    }
    Stop();
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
            ay8910 = new ay(songinfo); // 16 bit, 2 ch.
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
    if(audio->songinfo->stopping)
    {
        audio->songinfo->stopping = false;
        audio->stopping_thread = SDL_CreateThread(SDLAudio::StoppingThread, audio);
        memset(stream, 0, len);
        return;
    }
    audio->ay8910->ayProcess(stream, len);
}

int SDLAudio::StoppingThread(void *arg)
{
    SDLAudio *audio = (SDLAudio *)arg;
    audio->stopping_thread = 0;
    audio->Stop();
    if(audio->songinfo->s_callback)
        audio->songinfo->s_callback(audio->songinfo->s_callback_arg);
    return 0;
}
