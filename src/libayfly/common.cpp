/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew                                 *
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

AYSongInfo *ay_sys_getnewinfo()
{
    AYSongInfo *info = new AYSongInfo;
    if(!info)
        return 0;
    info->FilePath = TXT("");
    info->Name = TXT("");
    info->Author = TXT("");
    info->Length = info->Loop = 0;
    info->bEmul = false;
    info->init_proc = 0;
    info->play_proc = 0;
    info->cleanup_proc = 0;
    info->data = 0;
    info->file_len = 0;
    info->z80ctx = 0;
    info->timeElapsed = 0;
    info->callback = 0;
    info->z80_freq = Z80_FREQ;
    info->ay_freq = info->z80_freq / 2;
    info->int_freq = INT_FREQ;
#ifndef __SYMBIAN32__
    info->sr = 44100;
#else
    info->sr = 32000;
#endif
    info->player = 0;
    memset(info->module, 0, 65536);
    memset(info->file_data, 0, 65536);
    memset(info->z80IO, 0, 65536);
    return info;
}

#ifndef __SYMBIAN32__
void *ay_initsong(const wchar_t *FilePath, unsigned long sr)
#else
void *ay_initsong(TFileName FilePath, unsigned long sr)
#endif
{
    AYSongInfo *info = ay_sys_getnewinfo();
    if(!info)
        return 0;
#ifndef __SYMBIAN32__
#ifdef WINDOWS
    info->player = new DXAudio(sr, info);
#else
    info->player = new SDLAudio(sr, info);
#endif
#else
    info->player = new Cayfly_s60Audio(info);
#endif
    if(!info->player)
    {
        delete info;
        return 0;
    }
    info->FilePath = FilePath;
    info->sr = sr;

    if(!ay_sys_initz80(*info))
    {
        delete info;
        info = 0;
    }
    else
    {
        if(!ay_sys_readfromfile(*info))
        {
            delete info;
            info = 0;
        }
        else
        {
            if(!ay_sys_initsong(*info))
            {
                delete info;
                info = 0;
            }
            else
            {
                if(!info->bEmul)
                {
                    if(info->init_proc)
                        info->init_proc(*info);
                }
#ifndef __SYMBIAN32__
                ay_sys_getsonginfo(*info);
#else
                info->Length = -1;
#endif
            }
        }
    }

    return info;
}

#ifndef __SYMBIAN32__
void *ay_initsongindirect(unsigned char *module, unsigned long sr, wchar_t *type, unsigned long size)
#else
void *ay_initsongindirect(unsigned char *module, unsigned long sr, TFileName type, unsigned long size)
#endif
{
    AYSongInfo *info = ay_sys_getnewinfo();
    if(!info)
        return 0;
    info->FilePath = type;
    info->file_len = size;
    memcpy(info->file_data, module, size);
    info->sr = sr;
#ifndef __SYMBIAN32__
#ifdef WINDOWS
    info->player = new DXAudio(sr, info);
#else
    info->player = new SDLAudio(sr, info);
#endif
#else
    info->player = new Cayfly_s60Audio(info);
#endif
    if(!info->player)
    {
        delete info;
        return 0;
    }
    if(!ay_sys_initsong(*info))
    {
        delete info;
        info = 0;
    }
    else
    {
        if(!ay_sys_initz80(*info))
        {
            delete info;
            info = 0;
        }
        else
        {
            if(info->bEmul)
                ay_sys_resetz80(*info);
            else if(info->init_proc)
                info->init_proc(*info);
#ifndef __SYMBIAN32__
            ay_sys_getsonginfo(*info);
#else
            info->Length = -1;
#endif
        }
    }

    return info;
}

#ifndef __SYMBIAN32__
void *ay_getsonginfo(const wchar_t *FilePath)
#else
void *ay_getsonginfo(TFileName FilePath)
#endif
{
    AYSongInfo *info = ay_sys_getnewinfo();
    if(!info)
        return 0;
    info->FilePath = FilePath;
    if(!ay_sys_getsonginfo(*info))
    {
        delete info;
        info = 0;
    }

    return info;
}

#ifndef __SYMBIAN32__
void *ay_getsonginfoindirect(unsigned char *module, wchar_t *type, unsigned long size)
#else
void *ay_getsonginfoindirect(unsigned char *module, TFileName type, unsigned long size)
#endif
{
    AYSongInfo *info = ay_sys_getnewinfo();
    if(!info)
        return 0;
    info->FilePath = type;
    memcpy(info->file_data, module, size);
    if(!ay_sys_getsonginfoindirect(*info))
    {
        delete info;
        info = 0;
    }
    return info;
}

#ifndef __SYMBIAN32__
const wchar_t *ay_getsongname(void *info)
{
    return ((AYSongInfo *) info)->Name.c_str();
}
#else
TFileName ay_getsongname(void *info)
{
    return ((AYSongInfo *) info)->Name;
}
#endif

#ifndef __SYMBIAN32__
const wchar_t *ay_getsongauthor(void *info)
{
    return ((AYSongInfo *) info)->Author.c_str();
}
#else
TFileName ay_getsongauthor(void *info)
{
    return ((AYSongInfo *) info)->Author;
}
#endif

#ifndef __SYMBIAN32__
const wchar_t *ay_getsongpath(void *info)
{
    return ((AYSongInfo *) info)->FilePath.c_str();
}
#else
TFileName ay_getsongpath(void *info)
{
    return ((AYSongInfo *) info)->FilePath;
}
#endif

void ay_seeksong(void *info, long new_position)
{
    ay_sys_rewindsong(*(AYSongInfo *) info, new_position);
}

void ay_resetsong(void *info)
{
    AYSongInfo *song = (AYSongInfo *) info;
    if(!song->player)
        return;
    bool started = song->player->Started();
    if(started)
        song->player->Stop();
    song->timeElapsed = 0;
    ay_sys_initsong(*song);
    if(song->bEmul)
    {
        ay_sys_resetz80(*song);
    }
    else
    {
        if(song->init_proc)
            song->init_proc(*song);
    }
    if(started)
        song->player->Start();
}

void ay_closesong(void **info)
{
    AYSongInfo *song = (AYSongInfo *) *info;
    AYSongInfo **ppsong = (AYSongInfo **) info;    
    if(song->cleanup_proc)
    {
        song->cleanup_proc(*song);
    }

    delete song;
    *ppsong = 0;
}

void ay_setvolume(void *info, unsigned long chnl, double volume)
{
    ((AYSongInfo *) info)->player->SetVolume(chnl, volume);

}
double ay_getvolume(void *info, unsigned long chnl)
{
    return ((AYSongInfo *) info)->player->GetVolume(chnl);
}

void ay_chnlmute(void *info, unsigned long chnl, bool mute)
{
    ((AYSongInfo *) info)->player->ChnlMute(chnl, mute);
}

double ay_chnlmuted(void *info, unsigned long chnl)
{
    return ((AYSongInfo *) info)->player->ChnlMuted(chnl);
}

void ay_setcallback(void *info, ELAPSED_CALLBACK callback, void *callback_arg)
{
    ((AYSongInfo *) info)->callback = callback;
    ((AYSongInfo *) info)->callback_arg = callback_arg;
}

bool ay_songstarted(void *info)
{
    return ((AYSongInfo *) info)->player->Started();
}

void ay_startsong(void *info)
{
    if(!ay_songstarted(info))
        ((AYSongInfo *) info)->player->Start();
}

void ay_stopsong(void *info)
{
    if(ay_songstarted(info))
    {
        ((AYSongInfo *) info)->player->Stop();
    }
}

unsigned long ay_getsonglength(void *info)
{
    return ((AYSongInfo *) info)->Length;
}

unsigned long ay_getelapsedtime(void *info)
{
    return ((AYSongInfo *) info)->timeElapsed;
}

unsigned long ay_getsongloop(void *info)
{
    return ((AYSongInfo *) info)->Loop;
}

const unsigned char *ay_getregs(void *info, unsigned long chip_num)
{
    return ((AYSongInfo *) info)->player->GetAYRegs(chip_num);
}

void ay_rendersongbuffer(void *info, unsigned char *buffer, unsigned long buffer_length, unsigned long chip_num)
{
    ay_stopsong(info);
    ((AYSongInfo *) info)->player->GetAYBuffer(buffer, buffer_length, chip_num);
}

unsigned long ay_getz80freq(void *info)
{
    return ((AYSongInfo *) info)->z80_freq;
}
void ay_setz80freq(void *info, unsigned long z80_freq)
{
    ((AYSongInfo *) info)->z80_freq = z80_freq;
    ((AYSongInfo *) info)->player->SetAYParameters();
}
unsigned long ay_getayfreq(void *info)
{
    return ((AYSongInfo *) info)->ay_freq;
}
void ay_setayfreq(void *info, unsigned long ay_freq)
{
    ((AYSongInfo *) info)->ay_freq = ay_freq;
    ((AYSongInfo *) info)->player->SetAYParameters();
}
unsigned long ay_getintfreq(void *info)
{
    return ((AYSongInfo *) info)->int_freq;
}

void ay_setintfreq(void *info, unsigned long int_freq)
{
    ((AYSongInfo *) info)->int_freq = int_freq;
    ((AYSongInfo *) info)->player->SetAYParameters();
}

void ay_setsongplayer(void *info, void * /* class AbstractAudio */player)
{
    if(((AYSongInfo *) info)->player)
    {
        ay_stopsong(info);
        delete ((AYSongInfo *) info)->player;
        ((AYSongInfo *) info)->player = 0;
    }
    ((AYSongInfo *) info)->player = (AbstractAudio *) player;
}

void *ay_getsongplayer(void *info)
{
    return ((AYSongInfo *) info)->player;
}

void ay_z80xec(void *info)
{
    return ay_sys_z80exec(*(AYSongInfo *) info);
}

AYSongInfo::~AYSongInfo()
{
    if(player)
    {
    	if(player->Started())
    		player->Stop();
        delete player;
        player = 0;
    }
}
