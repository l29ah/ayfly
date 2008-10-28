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

unsigned short ay_sys_getword(unsigned char *p)
{
    return *p | ((*(p + 1)) << 8);
}

void ay_sys_writeword(unsigned char *p, unsigned short val)
{
    *p = (unsigned char)(val & 0xff);
    *(p + 1) = (unsigned char) ((val >> 8) & 0xff);
}

void ay_sys_initz80module(AYSongInfo &info, unsigned long player_base, const unsigned char *player_ptr, unsigned long player_length, unsigned long player_init_proc, unsigned long player_play_proc);

#include "players/AYPlay.h"
#include "players/ASCPlay.h"
#include "players/PT2Play.h"
#include "players/PT3Play.h"
#include "players/STPPlay.h"
#include "players/STCPlay.h"
#include "players/PSCPlay.h"
#include "players/SQTPlay.h"
#include "players/PSGPlay.h"
#include "players/PT1Play.h"
#include "players/VTXPlay.h"

typedef void (*GETINFO_CALLBACK)(AYSongInfo &info);

struct _Players
{
#ifndef __SYMBIAN32__
    AY_TXT_TYPE ext;
#else
    const TText *ext;
#endif
    const unsigned char *player;
    unsigned long player_base;
    unsigned long length;
    unsigned long module_base;
    unsigned long emul_init_proc;
    PLAYER_INIT_PROC soft_init_proc;
    unsigned long emul_play_proc;
    PLAYER_PLAY_PROC soft_play_proc;
    PLAYER_CLEANUP_PROC soft_cleanup_proc;
    GETINFO_CALLBACK getInfo;
    PLAYER_DETECT_PROC detect;
};

static const _Players Players[] =
{
{ TXT(".asc"), 0, 0, 0, 0, 0, ASC_Init, 0, ASC_Play, ASC_Cleanup, ASC_GetInfo, 0 },
{ TXT(".pt2"), PT2Play_data, 0xc000, sizeof(PT2Play_data), 0x0000, 0xc000, 0, 0xc006, 0, 0, PT2_GetInfo, 0 },
{ TXT(".pt3"), 0, 0, 0, 0, 0, PT3_Init, 0, PT3_Play, PT3_Cleanup, PT3_GetInfo, 0 },
{ TXT(".stc"), 0, 0, 0, 0, 0, STC_Init, 0, STC_Play, STC_Cleanup, STC_GetInfo, STC_Detect },
{ TXT(".stp"), 0, 0, 0, 0, 0, STP_Init, 0, STP_Play, STP_Cleanup, STP_GetInfo, STP_Detect },
{ TXT(".psc"), 0, 0, 0, 0, 0, PSC_Init, 0, PSC_Play, PSC_Cleanup, PSC_GetInfo, 0 },
{ TXT(".sqt"), 0, 0, 0, 0, 0, SQT_Init, 0, SQT_Play, SQT_Cleanup, SQT_GetInfo, 0 },
{ TXT(".psg"), 0, 0, 0, 0, 0, PSG_Init, 0, PSG_Play, PSG_Cleanup, PSG_GetInfo, 0 },
{ TXT(".pt1"), 0, 0, 0, 0, 0, PT1_Init, 0, PT1_Play, PT1_Cleanup, PT1_GetInfo, 0 },
{ TXT(".vtx"), 0, 0, 0, 0, 0, VTX_Init, 0, VTX_Play, VTX_Cleanup, VTX_GetInfo, 0 },
{ TXT(".ay"), 0, 0, 0, 0, 0, AY_Init, 0, AY_Play, AY_Cleanup, AY_GetInfo, AY_Detect } };

#ifndef __SYMBIAN32__
bool ay_sys_format_supported(AY_TXT_TYPE filePath)
#else
bool ay_sys_format_supported(const TFileName filePath)
#endif
{
#ifndef __SYMBIAN32__
    AY_TXT_TYPE cfp = filePath;
    cfp.toLower();
    for(unsigned long player = 0; player < sizeof_array(Players); player++)
    {
        if(cfp.rcompare(Players[player].ext) == 0)
        {
            return true;
        }
    }
#else
    TFileName cfp = filePath;
    cfp.LowerCase();
    TParse parse;
    parse.Set(cfp, NULL, NULL);
    for (unsigned long player = 0; player < sizeof_array(Players); player++)
    {
        TPtrC ext = parse.Ext();
        TPtrC ext_cur = Players [player].ext;
        if (ext.Compare(ext_cur) == 0)
        {
            return true;
        }
    }
#endif
    return false;
}

#ifndef __SYMBIAN32__
unsigned char *osRead(AY_TXT_TYPE filePath, unsigned long *data_len)
#else
unsigned char *osRead(const TFileName filePath, unsigned long *data_len)
#endif
{
    unsigned char *fileData = 0;
#ifndef __SYMBIAN32__
    FILE *f;
#if !defined(WINDOWS) && defined(UNICODE)
    size_t len = filePath.length() * 6;
    char *mb_str = new char[len + 1];
    if(!mb_str)
    {
        *data_len = 0;
        delete[] fileData;
        return 0;
    }
    mbstate_t mbstate;
    ::memset((void*)&mbstate, 0, sizeof(mbstate));
    const wchar_t *wc_str = filePath.c_str();
    size_t cnt_conv = wcsrtombs(mb_str, &wc_str, len, &mbstate);
    mb_str[cnt_conv] = 0;
    f = fopen(mb_str, "rb");
    delete[] mb_str;
#else
#if defined(WINDOWS) && defined(UNICODE)
    f = _wfopen(filePath.c_str(), TXT("rb"));
#else
    f = fopen(filePath.c_str(), "rb");
#endif
#endif
    if(f)
    {
        fseek(f, 0, SEEK_END);
        *data_len = ftell(f);
        fseek(f, 0, SEEK_SET);

        unsigned long to_allocate = *data_len < 65536 ? 65536 : *data_len;
        fileData = new unsigned char[to_allocate];
        if(!fileData)
        {
            fclose(f);
            return 0;
        }
        memset(fileData, 0, to_allocate);
        fread((char *)fileData, 1, *data_len, f);
        if(ferror(f))
            *data_len = 0;
        fclose(f);
    }
    else
        *data_len = 0;

#else
    RFs fsSession;// = CCoeEnv::Static()->FsSession();
    fsSession.Connect();
    RFileReadStream readStream;
    TEntry entry;
    TInt err = readStream.Open(fsSession, filePath, EFileRead);
    if (err == KErrNone)
    {
        fsSession.Entry(filePath, entry);
        *data_len = (TUint)entry.iSize;
        unsigned long to_allocate = *data_len < 65536 ? 65536 : *data_len;
        fileData = new unsigned char[to_allocate];
        if(!fileData)
        {
            readStream.Close();
            return 0;
        }
        memset(fileData, 0, to_allocate);
        readStream.ReadL((TUint8 *)fileData, *data_len);
        readStream.Close();
    }
    else
    {
        *data_len = 0;
    }

#endif
    if(!*data_len)
    {
        if(fileData)
            delete[] fileData;
        fileData = 0;
    }
    return fileData;
}

bool ay_sys_initsong(AYSongInfo &info)
{
    unsigned long player = 0;
    unsigned char *fileData = info.file_data;
    unsigned long fileLength = info.file_len;
#define GET_WORD(x) {(x) = (*ptr++) << 8; (x) |= *ptr++;}
#define GET_PTR(x) {unsigned long tmp; GET_WORD(tmp); if(tmp >= 0x8000) tmp=-0x10000+tmp; (x)=ptr-2+tmp;}

    memset(info.module, 0, info.file_len);
    memcpy(info.module, info.file_data, info.file_len);
    for(player = 0; player < sizeof_array(Players); player++)
    {
        if(Players[player].detect != 0)
        {
            if(Players[player].detect(info.module, info.module_len))
                break;
        }
    }

    if((player >= sizeof_array(Players)) && (info.FilePath.compare(TXT(""))))
    {
#ifndef __SYMBIAN32__
        AY_TXT_TYPE cfp = info.FilePath;
        cfp.toLower();
        for(player = 0; player < sizeof_array(Players); player++)
        {
            if(cfp.rcompare(Players[player].ext) == 0)
            {
                break;
            }
        }
#else
        TFileName cfp = info.FilePath;
        cfp.LowerCase();
        TParse parse;
        parse.Set(cfp, NULL, NULL);
        for (player = 0; player < sizeof_array(Players); player++)
        {
            TPtrC ext = parse.Ext();
            TPtrC ext_cur = Players [player].ext;
            if (ext.Compare(ext_cur) == 0)
            {
                break;
            }
        }
#endif
    }
    if(player >= sizeof_array(Players))
        return false;

    info.player_num = player;
    memset(info.module, 0, info.file_len);
    memcpy(info.module, fileData, fileLength);
    info.init_proc = Players[player].soft_init_proc;
    info.play_proc = Players[player].soft_play_proc;
    info.cleanup_proc = Players[player].soft_cleanup_proc;
    info.bEmul = false;
    fwprintf(stderr, L"format = %s\n", Players [player].ext.c_str());
    return true;
}

void ay_sys_initz80module(AYSongInfo &info, unsigned long player_base, const unsigned char *player_ptr, unsigned long player_length, unsigned long player_init_proc, unsigned long player_play_proc)
{
    //fill z80 memory
    memset(info.module + 0x0000, 0xc9, 0x0100);
    memset(info.module + 0x0100, 0xff, 0x3f00);
    memset(info.module + 0x4000, 0x00, 0xc000);
    info.module[0x38] = 0xfb; /* ei */
    //copy player to 0xc000 of z80 memory
    memcpy(info.module + player_base, player_ptr, player_length);

    //copy module right after the player
    memcpy(info.module + player_base + player_length, info.file_data, info.file_len);
    //copy im1 loop to 0x0 of z80 memory
    memcpy(info.module, intnz, sizeof(intnz));

    info.module[2] = player_init_proc % 256;
    info.module[3] = player_init_proc / 256;
    info.module[9] = player_play_proc % 256;
    info.module[10] = player_play_proc / 256;
    z80ex_set_reg(info.z80ctx, regSP, 0xc000);
}

bool ay_sys_readfromfile(AYSongInfo &info)
{
    unsigned long data_len = 65536;
    info.timeElapsed = 0;
    info.Length = 0;
    info.bEmul = true;
    info.init_proc = 0;
    info.play_proc = 0;
    if(info.file_data)
    {
        delete[] info.file_data;
        info.file_data = 0;
    }
    if(info.module)
    {
        delete[] info.module;
        info.module = 0;
    }
    info.file_data = osRead(info.FilePath, &data_len);
    if(!info.file_data)
        return false;

    info.file_len = data_len;
    info.module_len = data_len;

    unsigned long to_allocate = info.file_len < 65536 ? 65536 : info.file_len;
    info.module = new unsigned char[to_allocate];
    if(!info.module)
    {
        delete[] info.file_data;
        info.file_data = 0;
        return false;
    }
    memset(info.module, 0, to_allocate);

    return true;
}

bool ay_sys_getsonginfoindirect(AYSongInfo &info)
{
    info.Length = 0;
    info.Loop = 0;
    info.Name = TXT("");
    info.Author = TXT("");
    if(info.player_num > -1)
    {
        if(Players[info.player_num].getInfo)
        {
            Players[info.player_num].getInfo(info);
            return true;
        }

    }
}
bool ay_sys_getsonginfo(AYSongInfo &info)
{
    if(!ay_sys_readfromfile(info))
        return false;
    return ay_sys_getsonginfoindirect(info);
}

void ay_sys_rewindsong(AYSongInfo &info, long new_position)
{
    if(info.player && info.player->Started())
        info.player->Stop();

    unsigned long timeCurrent = info.timeElapsed;
    info.timeElapsed = new_position;

    if(info.timeElapsed < timeCurrent)
    {
        timeCurrent = 0;
        ay_resetsong(&info);
    }

    info.timeElapsed = timeCurrent;
    while(info.timeElapsed != new_position)
    {
        info.play_proc(info);
        info.timeElapsed++;
    }
}

