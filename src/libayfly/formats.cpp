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

#ifdef __SYMBIAN32__
#pragma pack(1)
#endif
#include "ayfly.h"
#include "players/ASCPlay.h"
#include "players/PT2Play.h"
#include "players/PT3Play.h"
#include "players/STPPlay.h"
#include "players/STCPlay.h"
#include "players/PSCPlay.h"
#include "players/SQTPlay.h"
#include "players/PSGPlay.h"

typedef void (*GETINFO_CALLBACK)(AYSongInfo &info);

enum _FileTypes
{
    FILE_TYPE_AY, FILE_TYPE_TRACKER
};

struct _Players
{
#ifndef __SYMBIAN32__
    AY_TXT_TYPE ext;
#else
    const TFileName ext;
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
};

static _Players Players[] =
{
{ TXT(".asc"), 0, 0, 0, 0, 0, ASC_Init, 0, ASC_Play, ASC_Cleanup, ASC_GetInfo },
{ TXT(".pt2"), PT2Play_data, 0xc000, sizeof(PT2Play_data), 0x0000, 0xc000, 0, 0xc006, 0, 0, PT2_GetInfo },
{ TXT(".pt3"), PT3Play_data, 0xc000, sizeof(PT3Play_data), 0x0000, 0xc000, 0, 0xc005, 0, 0, PT3_GetInfo },
{ TXT(".stc"), STCPlay_data, 0xc000, sizeof(STCPlay_data), 0x0000, 0xc000, 0, 0xc006, 0, 0, STC_GetInfo },
{ TXT(".stp"), STPPlay_data, 0xc000, sizeof(STPPlay_data), 0x0000, 0xc000, 0, 0xc006, 0, 0, STP_GetInfo },
{ TXT(".psc"), 0, 0, 0, 0, 0, PSC_Init, 0, PSC_Play, PSC_Cleanup, PSC_GetInfo },
{ TXT(".sqt"), 0, 0, 0, 0, 0, SQT_Init, 0, SQT_Play, SQT_Cleanup, SQT_GetInfo },
{ TXT(".psg"), 0, 0, 0, 0, 0, PSG_Init, 0, PSG_Play, PSG_Cleanup, PSG_GetInfo } };

/*
 * parts of ay read code and memory init are from aylet player:
 * Copyright (C) 2001-2005 Russell Marks and Ian Collier.
 */

static unsigned char intz[] =
{ 0xf3, /* di */
0xcd, 0, 0, /* call init */
0xed, 0x5e, /* loop: im 2 */
0xfb, /* ei */
0x76, /* halt */
0x18, 0xfa /* jr loop */
};

static unsigned char intnz[] =
{ 0xf3, /* di */
0xcd, 0, 0, /* call init */
0x0, 0x0, /* loop: im 1 */
0x00, /* ei */
0x00, /* halt */
0xcd, 0, 0, /* call interrupt */
0x18, 0xfb /* jr loop */
};

//for .ay format
struct ayTrack
{
    unsigned char *name, *data;
    unsigned char *data_points, *data_memblocks;
    unsigned long fadestart, fadelen;
};

struct ayData
{
    unsigned char *filedata;
    unsigned long filelen;
    struct ayTrack *tracks;

    unsigned long filever, playerver;
    unsigned char *author, *misc;
    unsigned long num_tracks;
    unsigned long first_track;
    ~ayData()
    {
    }
    ;
};

static ayData aydata;

void ay_sys_initayfmt(AYSongInfo &info, unsigned char track);

#ifndef __SYMBIAN32__
unsigned char *osRead(AY_TXT_TYPE filePath, unsigned long *data_len)
#else
unsigned char *osRead(const TFileName filePath, unsigned long *data_len)
#endif
{
    unsigned char *fileData = 0;
#ifndef __SYMBIAN32__
    std::ifstream f;
#ifndef WINDOWS
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
    f.open(mb_str, std::ios_base::in | std::ios_base::binary);
    delete[] mb_str;
#else
    f.open(filePath.c_str(), std::ios_base::in | std::ios_base::binary);
#endif
    if(f)
    {
        f.seekg(0, std::ios::end);
        *data_len = f.tellg();
        f.seekg(0, std::ios::beg);

        unsigned long to_allocate = *data_len < 65536 ? 65536 : *data_len;
        fileData = new unsigned char[to_allocate];
        if(!fileData)
        {
            f.close();
            return 0;
        }
        memset(fileData, 0, to_allocate);
        f.read((char *)fileData, *data_len);
        if(f.bad())
            *data_len = 0;
        f.close();
    }
    else
        *data_len = 0;

#else
    RFs fsSession = CCoeEnv::Static()->FsSession();
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
    _FileTypes fileType = FILE_TYPE_TRACKER;
    unsigned long player = 0;
    unsigned char *fileData = info.file_data;
    unsigned long fileLength = info.file_len;
#define GET_WORD(x) {(x) = (*ptr++) << 8; (x) |= *ptr++;}
#define GET_PTR(x) {unsigned long tmp; GET_WORD(tmp); if(tmp >= 0x8000) tmp=-0x10000+tmp; (x)=ptr-2+tmp;}
#ifndef __SYMBIAN32__
    AY_TXT_TYPE cfp = info.FilePath;
    std::transform(cfp.begin(), cfp.end(), cfp.begin(), (int(*)(int))std::tolower);
    if(cfp.rfind(TXT(".ay")) != std::string::npos)
    {
        fileType = FILE_TYPE_AY;
    }
    else
    {
        for(player = 0; player < sizeof_array(Players); player++)
        {
            if(cfp.rfind(Players[player].ext) != std::string::npos)
            {
                break;
            }
        }
    }
#else
    TFileName cfp = info.FilePath;
    cfp.LowerCase();
    TParse parse;
    parse.Set(cfp, NULL, NULL);
    if (parse.Ext().Match(_L(".ay")) != KErrNotFound)
    {
        fileType = FILE_TYPE_AY;
    }
    else
    {
        for (player = 0; player < sizeof_array(Players); player++)
        {
            TPtrC ext = parse.Ext();
            TPtrC ext_cur = Players [player].ext;
            if (ext.Compare(ext_cur) == 0)
            {
                break;
            }
        }
    }
#endif
    if((fileType == FILE_TYPE_TRACKER) && (player == sizeof_array(Players)))
        return false;

    if(fileType == FILE_TYPE_AY)
    {
        unsigned char *ptr = (unsigned char *)fileData;
        unsigned char *ptr2;
        if(*ptr == 'Z' && *(ptr + 1) == 'X' && *(ptr + 2) == 'A' && *(ptr + 3) == 'Y' && *(ptr + 4) == 'E' && *(ptr + 5) == 'M' && *(ptr + 6) == 'U' && *(ptr + 7) == 'L')
        {
            ptr += 8;
            aydata.filever = *ptr++;
            aydata.playerver = *ptr++;
            ptr += 2;
            GET_PTR(aydata.author);
            GET_PTR(aydata.misc);
            aydata.num_tracks = 1 + *ptr++;
            aydata.first_track = *ptr++;
            GET_PTR(ptr2);
            ptr = ptr2;
            if(aydata.tracks = new ayTrack[aydata.num_tracks])
            {
                for(unsigned long i = 0; i < aydata.num_tracks; i++)
                {
                    GET_PTR(aydata.tracks [i].name);
                    GET_PTR(aydata.tracks [i].data);
                }
                for(unsigned long i = 0; i < aydata.num_tracks; i++)
                {
                    ptr = aydata.tracks[i].data + 10;
                    GET_PTR(aydata.tracks [i].data_points);
                    GET_PTR(aydata.tracks [i].data_memblocks);

                    ptr = aydata.tracks[i].data + 4;
                    GET_WORD(aydata.tracks [i].fadestart);
                    GET_WORD(aydata.tracks [i].fadelen);
                }
                aydata.filelen = fileLength;
                aydata.filedata = fileData;
                ay_sys_initayfmt(info, 0);
                delete[] aydata.tracks;
                aydata.tracks = 0;
                return true;
            }
        }

    }
    else if(fileType == FILE_TYPE_TRACKER)
    {
        if(Players[player].player == 0) //soft player
        {
            memset(info.module, 0, 65536);
            memcpy(info.module, fileData, fileLength);
            info.init_proc = Players[player].soft_init_proc;
            info.play_proc = Players[player].soft_play_proc;
            info.cleanup_proc = Players[player].soft_cleanup_proc;
            info.bEmul = false;
            return true;
        }
        //fill z80 memory
        memset(info.module + 0x0000, 0xc9, 0x0100);
        memset(info.module + 0x0100, 0xff, 0x3f00);
        memset(info.module + 0x4000, 0x00, 0xc000);
        info.module[0x38] = 0xfb; /* ei */

        //copy player to 0xc000 of z80 memory
        memcpy(info.module + Players[player].player_base, Players[player].player, Players[player].length);

        if(Players[player].module_base)
        {
            //copy module at given address
            memcpy(info.module + Players[player].module_base, fileData, fileLength);
        }
        else
        {
            //copy module right after the player
            memcpy(info.module + Players[player].player_base + Players[player].length, fileData, fileLength);
        }
        //copy im1 loop to 0x0 of z80 memory
        memcpy(info.module, intnz, sizeof(intnz));

        info.module[2] = Players[player].emul_init_proc % 256;
        info.module[3] = Players[player].emul_init_proc / 256;
        info.module[9] = Players[player].emul_play_proc % 256;
        info.module[10] = Players[player].emul_play_proc / 256;
        z80ex_set_reg(info.z80ctx, regSP, 0xc000);
        return true;

    }
    return false;
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

    info.module = new unsigned char[info.file_len];
    if(!info.module)
    {
        delete[] info.file_data;
        info.file_data = 0;
        return false;
    }
    memset(info.module, 0, info.file_len);

    return true;
}

void ay_sys_initayfmt(AYSongInfo &info, unsigned char track)
{

    unsigned long init, ay_1st_block, ourinit, interrupt;
    unsigned char *ptr;
    unsigned long addr, len, ofs;

#undef GET_WORD
#define GET_WORD(x) (((*(x))<<8)|(*(x+1)))

    init = GET_WORD(aydata.tracks[track].data_points+2);
    interrupt = GET_WORD(aydata.tracks[track].data_points+4);
    ay_1st_block = GET_WORD(aydata.tracks[track].data_memblocks);

    memset(info.module + 0x0000, 0xc9, 0x0100);
    memset(info.module + 0x0100, 0xff, 0x3f00);
    memset(info.module + 0x4000, 0x00, 0xc000);
    info.module[0x38] = 0xfb; /* ei */

    /* call first AY block if no init */
    ourinit = (init ? init : ay_1st_block);

    if(!interrupt)
        memcpy(info.module, intz, sizeof(intz));
    else
    {
        memcpy(info.module, intnz, sizeof(intnz));
        info.module[9] = interrupt % 256;
        info.module[10] = interrupt / 256;
    }

    info.module[2] = ourinit % 256;
    info.module[3] = ourinit / 256;

    /* now put the memory blocks in place */
    ptr = aydata.tracks[track].data_memblocks;
    while((addr = GET_WORD(ptr)) != 0)
    {
        len = GET_WORD(ptr + 2);
        ofs = GET_WORD(ptr + 4);
        if(ofs >= 0x8000)
            ofs = -0x10000 + ofs;

        /* range check */
        if(ptr - 4 - aydata.filedata + ofs >= aydata.filelen || ptr - 4 - aydata.filedata + ofs < 0)
        {
            ptr += 6;
            continue;
        }

        /* fix any broken length */
        if(ptr + 4 + ofs + len >= aydata.filedata + aydata.filelen)
            len = aydata.filedata + aydata.filelen - (ptr + 4 + ofs);
        if(addr + len > 0x10000)
            len = 0x10000 - addr;

        memcpy(info.module + addr, ptr + 4 + ofs, len);
        ptr += 6;
    }

    /*ctx->R1.br.A = ctx->R2.br.A = ctx->R1.br.B = ctx->R2.br.B = ctx->R1.br.D = ctx->R2.br.D = ctx->R1.br.H = ctx->R2.br.H = aydata.tracks [track].data [8];
     ctx->R1.br.F = ctx->R2.br.F = ctx->R1.br.C = ctx->R2.br.C = ctx->R1.br.E = ctx->R2.br.E = ctx->R1.br.L = ctx->R2.br.L = aydata.tracks [track].data [9];
     ctx->R1.wr.SP = aydata.tracks [track].data_points [0] * 256 + aydata.tracks [track].data_points [1];
     ctx->I = 3;*/
    z80ex_set_reg(info.z80ctx, regSP, aydata.tracks[track].data_points[0] * 256 + aydata.tracks[track].data_points[1]);
    z80ex_set_reg(info.z80ctx, regI, 3);
    Z80EX_WORD reg = aydata.tracks[track].data[8] * 256 + aydata.tracks[track].data[9];
    z80ex_set_reg(info.z80ctx, regAF, reg);
    z80ex_set_reg(info.z80ctx, regAF_, reg);
    z80ex_set_reg(info.z80ctx, regBC, reg);
    z80ex_set_reg(info.z80ctx, regBC_, reg);
    z80ex_set_reg(info.z80ctx, regDE, reg);
    z80ex_set_reg(info.z80ctx, regDE_, reg);
    z80ex_set_reg(info.z80ctx, regHL, reg);
    z80ex_set_reg(info.z80ctx, regHL_, reg);

}

bool ay_sys_getsonginfoindirect(AYSongInfo &info)
{
    ayData aydata_loc;
    info.Length = 0;
    info.Loop = 0;
    info.Name = TXT("");
    info.Author = TXT("");
#undef GET_WORD
#define GET_WORD(x) {(x) = (*ptr++) << 8; (x) |= *ptr++;}
#define GET_PTR(x) {unsigned long tmp; GET_WORD(tmp); if(tmp >= 0x8000) tmp=-0x10000+tmp; (x)=ptr-2+tmp;}
    bool bRet = false;
#ifndef __SYMBIAN32__
    AY_TXT_TYPE cfp = info.FilePath;
    std::transform(cfp.begin(), cfp.end(), cfp.begin(), (int(*)(int))std::tolower);
    if(cfp.rfind(TXT(".ay")) != std::string::npos)
#else
    TFileName cfp = info.FilePath;
    cfp.LowerCase();
    TParse parse;
    parse.Set(cfp, NULL, NULL);

    if (parse.Ext() == _L(".ay"))
#endif
    {

        unsigned char *ptr = info.file_data;
        unsigned char *ptr2;
        if(*ptr == 'Z' && *(ptr + 1) == 'X' && *(ptr + 2) == 'A' && *(ptr + 3) == 'Y' && *(ptr + 4) == 'E' && *(ptr + 5) == 'M' && *(ptr + 6) == 'U' && *(ptr + 7) == 'L')
        {
            ptr += 8;
            aydata_loc.filever = *ptr++;
            aydata_loc.playerver = *ptr++;
            ptr += 2;
            GET_PTR(aydata_loc.author);
            GET_PTR(aydata_loc.misc);
            aydata_loc.num_tracks = 1 + *ptr++;
            aydata_loc.first_track = *ptr++;
            GET_PTR(ptr2);
            ptr = ptr2;
            if(aydata_loc.tracks = new ayTrack[aydata_loc.num_tracks])
            {
                for(unsigned long i = 0; i < aydata_loc.num_tracks; i++)
                {
                    GET_PTR(aydata_loc.tracks [i].name);
                    GET_PTR(aydata_loc.tracks [i].data);
                }
                for(unsigned long i = 0; i < aydata_loc.num_tracks; i++)
                {
                    ptr = aydata_loc.tracks[i].data + 10;
                    GET_PTR(aydata_loc.tracks [i].data_points);
                    GET_PTR(aydata_loc.tracks [i].data_memblocks);

                    ptr = aydata_loc.tracks[i].data + 4;
                    GET_WORD(aydata_loc.tracks [i].fadestart);
                    GET_WORD(aydata_loc.tracks [i].fadelen);
                }
                if(aydata_loc.num_tracks)
                {
                    info.Length = aydata_loc.tracks[0].fadestart;
                }

                delete[] aydata_loc.tracks;
                bRet = true;
            }

        }

    }
    else
    {
        for(unsigned int i = 0; i < sizeof_array(Players); i++)
        {
#ifndef __SYMBIAN32__
            if(cfp.rfind(Players[i].ext) != std::string::npos)
#else
            TPtrC ext = parse.Ext();
            TPtrC ext_cur = Players [i].ext;
            if (ext.Compare(ext_cur) == 0)
#endif
            {
                if(Players[i].getInfo)
                    Players[i].getInfo(info);
                bRet = true;
                break;
            }
        }

    }
    return bRet;
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
        /*if (!ay_sys_readfromfile(info))
         return;*/
        ay_resetsong(&info);
    }

    info.timeElapsed = timeCurrent;
    if(info.bEmul)
    {
        while(info.timeElapsed != new_position)
        {
            z80ex_step(info.z80ctx);
            if(z80ex_get_reg(info.z80ctx, regPC) == 8)
                info.timeElapsed++;
        }

    }
    else if(info.play_proc)
    {
        while(info.timeElapsed != new_position)
        {
            info.play_proc(info);
        }

    }

}
