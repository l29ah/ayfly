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
    info->e_callback = 0;
    info->e_callback_arg = 0;
    info->s_callback = 0;
    info->s_callback_arg = 0;
    info->z80_freq = Z80_FREQ;
    info->ay_freq = info->z80_freq / 2;
    info->int_freq = INT_FREQ;
    info->file_data = info->module = 0;
    info->sr = 44100;
    info->player = 0;
    info->chip_type = 0;
    info->own_player = true;
    info->stopping = false;
    memset(info->z80IO, 0, 65536);
    return info;
}

#ifndef __SYMBIAN32__
AYFLY_API void *ay_initsong(const AY_CHAR *FilePath, unsigned long sr, AbstractAudio *player)
#else
AYFLY_API void *ay_initsong(TFileName FilePath, unsigned long sr, AbstractAudio *player)
#endif
{
    AYSongInfo *info = ay_sys_getnewinfo();
    if(!info)
        return 0;

    info->FilePath = FilePath;
    info->sr = sr;
    if(player)
    {
        info->player = player;
        info->own_player = false;
        player->SetSongInfo(info);
    }
    else
    {
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
    }
    
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
                ay_sys_getsonginfoindirect(*info);                
            }
        }
    }
    return info;
}

#ifndef __SYMBIAN32__
AYFLY_API void *ay_initsongindirect(unsigned char *module, unsigned long sr, AY_CHAR *type, unsigned long size, AbstractAudio *player)
#else
AYFLY_API void *ay_initsongindirect(unsigned char *module, unsigned long sr, TFileName type, unsigned long size, AbstractAudio *player)
#endif
{
    AYSongInfo *info = ay_sys_getnewinfo();
    if(!info)
        return 0;
    info->FilePath = type;
    info->file_len = size;
    unsigned long to_allocate = size < 65536 ? 65536 : size;
    info->file_data = new unsigned char[to_allocate];
    if(!info->file_data)
    {
        delete info;
        return 0;
    }
    memset(info->file_data, 0, to_allocate);
    memcpy(info->file_data, module, size);
    info->module = new unsigned char[to_allocate];
    if(!info->module)
    {
        delete info;
        return 0;
    }
    info->sr = sr;
    if(player)
    {
        info->player = player;
        info->own_player = false;
        player->SetSongInfo(info);
    }
    else
    {
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
            ay_sys_getsonginfoindirect(*info);
        }
    }

    return info;
}

#ifndef __SYMBIAN32__
AYFLY_API void *ay_getsonginfo(const AY_CHAR *FilePath)
#else
AYFLY_API void *ay_getsonginfo(TFileName FilePath)
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
AYFLY_API void *ay_getsonginfoindirect(unsigned char *module, AY_CHAR *type, unsigned long size)
#else
AYFLY_API void *ay_getsonginfoindirect(unsigned char *module, TFileName type, unsigned long size)
#endif
{
    AYSongInfo *info = ay_sys_getnewinfo();
    if(!info)
        return 0;
    info->FilePath = type;
    unsigned long to_allocate = size < 65536 ? 65536 : size;
    info->file_data = new unsigned char[to_allocate];
    if(!info->file_data)
    {
        delete info;
        return 0;
    }
    memset(info->file_data, 0, to_allocate);
    memcpy(info->file_data, module, size);
    info->module = new unsigned char[to_allocate];
    if(!info->module)
    {
        delete info;
        return 0;
    }
    memcpy(info->file_data, module, size);
    if(!ay_sys_getsonginfoindirect(*info))
    {
        delete info;
        info = 0;
    }
    return info;
}

#ifndef __SYMBIAN32__
AYFLY_API const AY_CHAR *ay_getsongname(void *info)
{
    return ((AYSongInfo *)info)->Name.c_str();
}
#else
AYFLY_API TFileName ay_getsongname(void *info)
{
    return ((AYSongInfo *) info)->Name;
}
#endif

#ifndef __SYMBIAN32__
AYFLY_API const AY_CHAR *ay_getsongauthor(void *info)
{
    return ((AYSongInfo *)info)->Author.c_str();
}
#else
AYFLY_API TFileName ay_getsongauthor(void *info)
{
    return ((AYSongInfo *) info)->Author;
}
#endif

#ifndef __SYMBIAN32__
AYFLY_API const AY_CHAR *ay_getsongpath(void *info)
{
    return ((AYSongInfo *)info)->FilePath.c_str();
}
#else
AYFLY_API TFileName ay_getsongpath(void *info)
{
    return ((AYSongInfo *) info)->FilePath;
}
#endif

AYFLY_API void ay_seeksong(void *info, long new_position)
{
    ((AYSongInfo *)info)->stopping = false;
    ay_sys_rewindsong(*(AYSongInfo *)info, new_position);
}

AYFLY_API void ay_resetsong(void *info)
{
    AYSongInfo *song = (AYSongInfo *)info;
    if(!song->player)
        return;
    song->stopping = false;
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

AYFLY_API void ay_closesong(void **info)
{
    AYSongInfo *song = (AYSongInfo *)*info;
    AYSongInfo **ppsong = (AYSongInfo **)info;
    delete song;
    *ppsong = 0;
}

AYFLY_API void ay_setvolume(void *info, unsigned long chnl, float volume, unsigned long chip_num)
{
    ((AYSongInfo *)info)->player->SetVolume(chnl, volume, chip_num);

}
AYFLY_API float ay_getvolume(void *info, unsigned long chnl, unsigned long chip_num)
{
    return ((AYSongInfo *)info)->player->GetVolume(chnl, chip_num);
}

AYFLY_API void ay_chnlmute(void *info, unsigned long chnl, bool mute, unsigned long chip_num)
{
    ((AYSongInfo *)info)->player->ChnlMute(chnl, mute, chip_num);
}

AYFLY_API bool ay_chnlmuted(void *info, unsigned long chnl, unsigned long chip_num)
{
    return ((AYSongInfo *)info)->player->ChnlMuted(chnl, chip_num);
}

AYFLY_API void ay_setelapsedcallback(void *info, ELAPSED_CALLBACK callback, void *callback_arg)
{
    ((AYSongInfo *)info)->e_callback_arg = callback_arg;
    ((AYSongInfo *)info)->e_callback = callback;
}

AYFLY_API void ay_setstoppedcallback(void *info, STOPPED_CALLBACK callback, void *callback_arg)
{
    ((AYSongInfo *)info)->s_callback_arg = callback_arg;
    ((AYSongInfo *)info)->s_callback = callback;
}

AYFLY_API bool ay_songstarted(void *info)
{
    return ((AYSongInfo *)info)->player->Started();
}

AYFLY_API void ay_startsong(void *info)
{
    ((AYSongInfo *)info)->stopping = false;
    if(!ay_songstarted(info))
        ((AYSongInfo *)info)->player->Start();
}

AYFLY_API void ay_stopsong(void *info)
{
    ((AYSongInfo *)info)->stopping = false;
    if(ay_songstarted(info))
    {
        ((AYSongInfo *)info)->player->Stop();
    }
}

AYFLY_API unsigned long ay_getsonglength(void *info)
{
    return ((AYSongInfo *)info)->Length;
}

AYFLY_API unsigned long ay_getelapsedtime(void *info)
{
    return ((AYSongInfo *)info)->timeElapsed;
}

AYFLY_API unsigned long ay_getsongloop(void *info)
{
    return ((AYSongInfo *)info)->Loop;
}

AYFLY_API const unsigned char *ay_getregs(void *info, unsigned long chip_num)
{
    return ((AYSongInfo *)info)->player->GetAYRegs(chip_num);
}

AYFLY_API void ay_rendersongbuffer(void *info, unsigned char *buffer, unsigned long buffer_length, unsigned long chip_num)
{
    ay_stopsong(info);
    ((AYSongInfo *)info)->player->GetAYBuffer(buffer, buffer_length, chip_num);
}

AYFLY_API unsigned long ay_getz80freq(void *info)
{
    return ((AYSongInfo *)info)->z80_freq;
}
AYFLY_API void ay_setz80freq(void *info, unsigned long z80_freq)
{
    ((AYSongInfo *)info)->z80_freq = z80_freq;
    ((AYSongInfo *)info)->player->SetAYParameters();
}
AYFLY_API unsigned long ay_getayfreq(void *info, unsigned long chip_num)
{
    return ((AYSongInfo *)info)->ay_freq;
}
AYFLY_API void ay_setayfreq(void *info, unsigned long ay_freq)
{
    ((AYSongInfo *)info)->ay_freq = ay_freq;
    ((AYSongInfo *)info)->player->SetAYParameters();
}
AYFLY_API unsigned long ay_getintfreq(void *info)
{
    return ((AYSongInfo *)info)->int_freq;
}

AYFLY_API void ay_setintfreq(void *info, unsigned long int_freq)
{
    ((AYSongInfo *)info)->int_freq = int_freq;
    ((AYSongInfo *)info)->player->SetAYParameters();
}

AYFLY_API unsigned long ay_getsamplerate(void *info)
{
    return ((AYSongInfo *)info)->sr;
}

AYFLY_API void ay_setsamplerate(void *info, unsigned long sr)
{
    ((AYSongInfo *)info)->sr = sr;
    ((AYSongInfo *)info)->player->SetAYParameters();
}

AYFLY_API void ay_setsongplayer(void *info, void * /* class AbstractAudio */player)
{
    if(((AYSongInfo *)info)->player)
    {
        ay_stopsong(info);
        if(((AYSongInfo *)info)->own_player)
        {
            delete ((AYSongInfo *)info)->player;
            ((AYSongInfo *)info)->player = 0;
        }
    }
    ((AYSongInfo *)info)->player = (AbstractAudio *)player;
}

AYFLY_API void *ay_getsongplayer(void *info)
{
    return ((AYSongInfo *)info)->player;
}

AYFLY_API void ay_setchiptype(void *info, unsigned char chip_type)
{
    ((AYSongInfo *)info)->chip_type = chip_type;
    ((AYSongInfo *)info)->player->SetAYParameters();
}

AYFLY_API unsigned char ay_getchiptype(void *info)
{
    return ((AYSongInfo *)info)->chip_type;
}

AYFLY_API void ay_z80exec(void *info)
{
    return ay_sys_z80exec(*(AYSongInfo *)info);
}

AYSongInfo::~AYSongInfo()
{
    if(player && player->Started())
    {
        player->Stop();
    }
    if(cleanup_proc)
    {
        cleanup_proc(*this);
    }
    if(own_player)
    {
        if(player)
        {
            delete player;
            player = 0;
        }
    }
    ay_sys_shutdownz80(*this);
    if(module)
    {
        delete[] module;
        module = 0;
    }
    if(file_data)
    {
        delete[] file_data;
        file_data = 0;
    }

}

#ifndef __SYMBIAN32__
bool ay_format_supported(AY_TXT_TYPE filePath)
#else
bool ay_format_supported(const TFileName filePath)
#endif
{
    return ay_sys_format_supported(filePath);
}

#ifdef WINDOWS
AYFLY_API void ay_sethwnd(void *info, HWND hWnd)
{
    ((DXAudio *)((AYSongInfo *)info)->player)->SetHWND(hWnd);
}
#endif
