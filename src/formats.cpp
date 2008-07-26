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

#include "common.h"
#include "players/PT2Play.h"
#include "players/PT3Play.h"
#include "players/STPPlay.h"
#include "players/PSCPlay.h"
#include "players/STCPlay.h"
//#include "players/SQTPlay.h" not working
//#include "players/ASCPlay.h" not working

enum _FileTypes
{
    FILE_TYPE_AY, FILE_TYPE_TRACKER
};

struct _Players
{
    TXT_TYPE ext;
    const char *player;
    unsigned long player_base;
    unsigned long length;
    unsigned long module_base;
    unsigned long init_proc;
    unsigned long play_proc;
    GETTIME_CALLBACK getTime;
};

unsigned long STCGetTime(const char *fileData, unsigned long &loop);
unsigned long STPGetTime(const char *fileData, unsigned long &loop);
unsigned long PT3GetTime(const char *fileData, unsigned long &loop);
unsigned long PT2GetTime(const char *fileData, unsigned long &loop);

_Players Players[] =
{
{ TXT(".pt2"), PT2Play_data, 0xc000, sizeof(PT2Play_data), 0x0000, 0xc000, 0xc006, PT2GetTime },
{ TXT(".pt3"), PT3Play_data, 0xc000, sizeof(PT3Play_data), 0x0000, 0xc000, 0xc005, PT3GetTime },
{ TXT(".stp"), STPPlay_data, 0xc000, sizeof(STPPlay_data), 0x0000, 0xc000, 0xc006, STPGetTime },
{ TXT(".psc"), PSCPlay_data, 0xc000, sizeof(PSCPlay_data), 0x0000, 0xc000, 0xc006, 0 },
{ TXT(".stc"), STCPlay_data, 0xc000, sizeof(STCPlay_data), 0x0000, 0xc000, 0xc006, STCGetTime },
//		{TEXT(".sqt"), SQTPlay_data, 0xc000, sizeof(SQTPlay_data), 0x0000, 0xc000, 0xc030}, not working
        //		{TEXT(".asc"), ASCPlay_data, 0xc000, sizeof(ASCPlay_data), 0x0000, 0xc000 + 11, 0xc000 + 14} not working
};

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
0xed, 0x56, /* loop: im 1 */
0xfb, /* ei */
0x76, /* halt */
0xcd, 0, 0, /* call interrupt */
0x18, 0xf7 /* jr loop */
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
};

static ayData aydata;

void initMemoryAY(unsigned char track);

#ifndef __SYMBIAN32__
char *osRead(const TXT_TYPE &filePath, unsigned long *data_len)
#else
char *osRead(const TDesC &filePath, unsigned long *data_len)
#endif
{
    char *fileData = new char[*data_len];
    if (!fileData)
        return 0;
    memset(fileData, 0, *data_len);
#ifndef __SYMBIAN32__
    /*#ifndef WINDOWS
     SDL_RWops *f = SDL_RWFromFile(filePath, "rb");
     if(f)
     {
     f->read(f, fileData, *data_len, 1);
     *data_len = f->seek(f, 0, SEEK_CUR);
     f->close(f);
     }
     else
     *data_len = 0;
     #else
     HANDLE hFile = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
     if(hFile != INVALID_HANDLE_VALUE)
     {
     DWORD dwRead = 0;
     if(ReadFile(hFile, fileData, *data_len, &dwRead, NULL) != FALSE)
     *data_len = dwRead;
     else
     *data_len = 0;
     CloseHandle(hFile);
     }
     else
     *data_len = 0;
     #endif*/
    wxFile f(filePath);
    *data_len = 0;
    if (f.IsOpened())
    {
        wxFileOffset nSize = f.Length();
        if (nSize != wxInvalidOffset)
        {
            if (f.Read(fileData, (size_t) nSize) == nSize)
            {
                *data_len = nSize;

            }
        }
        f.Close();
    }

#else
    RFs fsSession = CCoeEnv::Static()->FsSession();
    //User::LeaveIfError(fsSession.Connect());

    RFileReadStream readStream;
    TEntry entry;
    TInt err = readStream.Open(fsSession, filePath, EFileRead);

    if(err == KErrNone)
    {
        fsSession.Entry(filePath, entry);
        *data_len = (TUint)entry.iSize;
        readStream.ReadL((TUint8 *)fileData, *data_len);
        //gConsole->Printf(_L("File Read. Length=%d...\n"), *data_len);
        readStream.Close();
    }
    else
    {
        //gConsole->Printf(_L("File not opened!\n"));
        *data_len = 0;
    }
    //fsSession.Close();
#endif
    if (!*data_len)
    {
        delete[] fileData;
        fileData = 0;
    }
    return fileData;
}

bool parseData(char *fileData, unsigned long fileLength, _FileTypes fileType, unsigned long player = 0)
{
#define GET_WORD(x) {(x) = (*ptr++) << 8; (x) |= *ptr++;}
#define GET_PTR(x) {unsigned long tmp; GET_WORD(tmp); if(tmp >= 0x8000) tmp=-0x10000+tmp; (x)=ptr-2+tmp;}
    if (fileType == FILE_TYPE_AY)
    {
        unsigned char *ptr = (unsigned char *) fileData;
        unsigned char *ptr2;
        if (!memcmp(ptr, "ZXAYEMUL", 8))
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
            if ((aydata.tracks = (ayTrack *) malloc(aydata.num_tracks * sizeof(ayTrack))))
            {
                for (unsigned long i = 0; i < aydata.num_tracks; i++)
                {
                    GET_PTR(aydata.tracks [i].name);
                    GET_PTR(aydata.tracks [i].data);
                }
                for (unsigned long i = 0; i < aydata.num_tracks; i++)
                {
                    ptr = aydata.tracks[i].data + 10;
                    GET_PTR(aydata.tracks [i].data_points);
                    GET_PTR(aydata.tracks [i].data_memblocks);

                    ptr = aydata.tracks[i].data + 4;
                    GET_WORD(aydata.tracks [i].fadestart);
                    GET_WORD(aydata.tracks [i].fadelen);
                }
                aydata.filelen = fileLength;
                aydata.filedata = (unsigned char *) fileData;
                initMemoryAY(0);
                return true;
            }
        }

    }
    else if (fileType == FILE_TYPE_TRACKER)
    {
        //fill z80 memory
        memset(z80Memory + 0x0000, 0xc9, 0x0100);
        memset(z80Memory + 0x0100, 0xff, 0x3f00);
        memset(z80Memory + 0x4000, 0x00, 0xc000);
        z80Memory[0x38] = 0xfb; /* ei */

        //copy player to 0xc000 of z80 memory
        memcpy(z80Memory + Players[player].player_base, Players[player].player, Players[player].length);

        if (Players[player].module_base)
        {
            //copy module at given address
            memcpy(z80Memory + Players[player].module_base, fileData, fileLength);
        }
        else
        {
            //copy module right after the player
            memcpy(z80Memory + Players[player].player_base + Players[player].length, fileData, fileLength);
        }
        //copy im1 loop to 0x0 of z80 memory
        memcpy(z80Memory, intnz, sizeof(intnz));

        z80Memory[2] = Players[player].init_proc % 256;
        z80Memory[3] = Players[player].init_proc / 256;
        z80Memory[9] = Players[player].play_proc % 256;
        z80Memory[10] = Players[player].play_proc / 256;
        z80ex_set_reg(ctx, regSP, 0xc000);
        return true;

    }
    return false;
}

#ifndef __SYMBIAN32__
bool readFile(const TXT_TYPE &filePath)
{
    unsigned long data_len = 65536;

    bool bRet = false;
    char *fileData = 0;

    wxString cfp = filePath;
    cfp = cfp.MakeLower();

    if (cfp.rfind(TXT(".ay")) != wxString::npos)
    {
        char *fileData = osRead(filePath, &data_len);
        if (fileData)
        {
            bRet = parseData(fileData, data_len, FILE_TYPE_AY);
        }
    }
    else
    {
        for (unsigned int i = 0; i < sizeof_array(Players); i++)
        {
            if (cfp.rfind(Players[i].ext) != wxString::npos)
            {
                char *fileData = osRead(filePath, &data_len);
                if (fileData)
                {
                    bRet = parseData(fileData, data_len, FILE_TYPE_TRACKER, i);
                }
                break;
            }
        }
    }
    if (fileData)
        delete fileData;
    if (!bRet)
        printf("Error reading file!\n");

    return bRet;
}
#else
bool readFile(const TDesC &filePath)
{
    unsigned long data_len = 65536;
    bool bRet = false;
    char *fileData = 0;
    TParse parse;
    parse.Set(filePath, NULL, NULL);
    //gConsole->Printf(_L("Reading file %S...\n"), &filePath);
    if (parse.Ext() == _L(".ay"))
    {
        char *fileData = osRead(filePath, &data_len);
        if (fileData)
        {
            bRet = parseData(fileData, data_len, FILE_TYPE_AY);
        }
    }
    else
    {
        for (unsigned int i = 0; i < sizeof_array(Players); i++)
        {
            TPtrC ext = parse.Ext();
            TPtrC ext_cur = Players [i].ext;
            //gConsole->Printf(_L("Current ext = %S, file ext = %S\n"), &ext_cur, &ext);
            if (ext.Compare(ext_cur) == 0)
            {
                char *fileData = osRead(filePath, &data_len);
                if (fileData)
                {
                    bRet = parseData(fileData, data_len, FILE_TYPE_TRACKER, i);
                }
                break;
            }
        }
    }
    if (fileData)
    delete fileData;

    //if(!bRet)
    //gConsole->Printf(_L("Error reading file %S...\n"), &filePath);

    return bRet;

}
#endif

void initMemoryAY(unsigned char track)
{

    unsigned long init, ay_1st_block, ourinit, interrupt;
    unsigned char *ptr;
    unsigned long addr, len, ofs;

#undef GET_WORD
#define GET_WORD(x) (((*(x))<<8)|(*(x+1)))

    init = GET_WORD(aydata.tracks[track].data_points+2);
    interrupt = GET_WORD(aydata.tracks[track].data_points+4);
    ay_1st_block = GET_WORD(aydata.tracks[track].data_memblocks);

    memset(z80Memory + 0x0000, 0xc9, 0x0100);
    memset(z80Memory + 0x0100, 0xff, 0x3f00);
    memset(z80Memory + 0x4000, 0x00, 0xc000);
    z80Memory[0x38] = 0xfb; /* ei */

    /* call first AY block if no init */
    ourinit = (init ? init : ay_1st_block);

    if (!interrupt)
        memcpy(z80Memory, intz, sizeof(intz));
    else
    {
        memcpy(z80Memory, intnz, sizeof(intnz));
        z80Memory[9] = interrupt % 256;
        z80Memory[10] = interrupt / 256;
    }

    z80Memory[2] = ourinit % 256;
    z80Memory[3] = ourinit / 256;

    /* now put the memory blocks in place */
    ptr = aydata.tracks[track].data_memblocks;
    while ((addr = GET_WORD(ptr)) != 0)
    {
        len = GET_WORD(ptr + 2);
        ofs = GET_WORD(ptr + 4);
        if (ofs >= 0x8000)
            ofs = -0x10000 + ofs;

        /* range check */
        if (ptr - 4 - aydata.filedata + ofs >= aydata.filelen || ptr - 4 - aydata.filedata + ofs < 0)
        {
            ptr += 6;
            continue;
        }

        /* fix any broken length */
        if (ptr + 4 + ofs + len >= aydata.filedata + aydata.filelen)
            len = aydata.filedata + aydata.filelen - (ptr + 4 + ofs);
        if (addr + len > 0x10000)
            len = 0x10000 - addr;

        memcpy(z80Memory + addr, ptr + 4 + ofs, len);
        ptr += 6;
    }

    /*ctx->R1.br.A = ctx->R2.br.A = ctx->R1.br.B = ctx->R2.br.B = ctx->R1.br.D = ctx->R2.br.D = ctx->R1.br.H = ctx->R2.br.H = aydata.tracks [track].data [8];
     ctx->R1.br.F = ctx->R2.br.F = ctx->R1.br.C = ctx->R2.br.C = ctx->R1.br.E = ctx->R2.br.E = ctx->R1.br.L = ctx->R2.br.L = aydata.tracks [track].data [9];
     ctx->R1.wr.SP = aydata.tracks [track].data_points [0] * 256 + aydata.tracks [track].data_points [1];
     ctx->I = 3;*/
    z80ex_set_reg(ctx, regSP, aydata.tracks[track].data_points[0] * 256 + aydata.tracks[track].data_points[1]);
    z80ex_set_reg(ctx, regI, 3);
    Z80EX_WORD reg = aydata.tracks[track].data[8] * 256 + aydata.tracks[track].data[9];
    z80ex_set_reg(ctx, regAF, reg);
    z80ex_set_reg(ctx, regAF_, reg);
    z80ex_set_reg(ctx, regBC, reg);
    z80ex_set_reg(ctx, regBC_, reg);
    z80ex_set_reg(ctx, regDE, reg);
    z80ex_set_reg(ctx, regDE_, reg);
    z80ex_set_reg(ctx, regHL, reg);
    z80ex_set_reg(ctx, regHL_, reg);

}

bool getSongInfo(SongInfo *info)
{
    ayData aydata_loc;
    info->Length = 0;
    info->Loop = 0;
#undef GET_WORD
#define GET_WORD(x) {(x) = (*ptr++) << 8; (x) |= *ptr++;}
#define GET_PTR(x) {unsigned long tmp; GET_WORD(tmp); if(tmp >= 0x8000) tmp=-0x10000+tmp; (x)=ptr-2+tmp;}
    unsigned long data_len = 65536;
#ifndef __SYMBIAN32__
    wxString cfp = info->FilePath;
    cfp = cfp.MakeLower();
    if (cfp.rfind(TXT(".ay")) != wxString::npos)
#else
    TParse parse;
    parse.Set(info->FilePath, NULL, NULL);
    if (parse.Ext() == _L(".ay"))
#endif
    {
        char *fileData = osRead(info->FilePath, &data_len);
        if (fileData)
        {
            unsigned char *ptr = (unsigned char *) fileData;
            unsigned char *ptr2;
            if (!memcmp(ptr, "ZXAYEMUL", 8))
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
                if ((aydata_loc.tracks = (ayTrack *) malloc(aydata_loc.num_tracks * sizeof(ayTrack))))
                {
                    for (unsigned long i = 0; i < aydata_loc.num_tracks; i++)
                    {
                        GET_PTR(aydata_loc.tracks [i].name);
                        GET_PTR(aydata_loc.tracks [i].data);
                    }
                    for (unsigned long i = 0; i < aydata_loc.num_tracks; i++)
                    {
                        ptr = aydata_loc.tracks[i].data + 10;
                        GET_PTR(aydata_loc.tracks [i].data_points);
                        GET_PTR(aydata_loc.tracks [i].data_memblocks);

                        ptr = aydata_loc.tracks[i].data + 4;
                        GET_WORD(aydata_loc.tracks [i].fadestart);
                        GET_WORD(aydata_loc.tracks [i].fadelen);
                    }
                    if (aydata_loc.num_tracks)
                    {
                        info->Length = aydata_loc.tracks[0].fadestart;
                    }

                    free(aydata_loc.tracks);
                }

            }
            delete fileData;
        }

    }
    else
    {
        for (unsigned int i = 0; i < sizeof_array(Players); i++)
        {
#ifndef __SYMBIAN32__
            wxString cfp = info->FilePath;
            cfp = cfp.MakeLower();
            if (cfp.rfind(Players[i].ext) != wxString::npos)
#else
            TPtrC ext = parse.Ext();
            TPtrC ext_cur = Players [i].ext;
            if (ext.Compare(ext_cur) == 0)
#endif
            {
                char *fileData = osRead(info->FilePath, &data_len);
                if (fileData)
                {
                    if (Players[i].getTime)
                        info->Length = Players[i].getTime(fileData, info->Loop);
                    delete fileData;
                }
                break;
            }
        }

    }
    return true;
}

unsigned long STCGetTime(const char *fileData, unsigned long &loop)
{
    unsigned long tm = 0;
    long j, j1, j2, i;
    unsigned char stDelay = fileData[0];
    unsigned short stPosPt = *(unsigned short *) &fileData[1];
    //unsigned long stOrnPt = *(unsigned short *)fileData [3];
    unsigned short stPatPt = *(unsigned short *) &fileData[5];
    unsigned char a;

    j = -1;
    do
    {
        j++;
        j2 = stPosPt + j * 2 + 1;
        j2 = fileData[j2];
        i = -1;
        do
        {
            i++;
            j1 = stPatPt + 7 * i;
        } while (fileData[j1] != j2);
        j1 = *(unsigned short *) &fileData[j1 + 1];
        a = 1;
        while (*(unsigned char *) &fileData[j1] != 255)
        {
            unsigned char val = *(unsigned char *) &fileData[j1];
            if ((val >= 0 && val <= 0x5f) || (val == 0x80) || (val == 0x81))
            {
                tm += a;
            }
            else if (val >= 0xa1 && val <= 0xe0)
            {
                a = val - 0xa0;
            }
            else if (val >= 0x83 && val <= 0x8e)
            {
                j1++;
            }
            j1++;
        }
    } while (j != fileData[stPosPt]);
    tm *= stDelay;
    return tm;
}

unsigned long STPGetTime(const char *fileData, unsigned long &loop)
{
    unsigned long tm = 0;
    unsigned char a = 1;
    unsigned long i, j1;
    unsigned char stDelay = fileData[0];
    unsigned short stPosPt = *(unsigned short *) &fileData[1];
    //unsigned long stOrnPt = *(unsigned short *)fileData [5];
    unsigned long stPatPt = *(unsigned short *) &fileData[3];

    for (i = 0; i < (unsigned char) fileData[stPosPt]; i++)
    {
        j1 = *(unsigned short *) &fileData[stPatPt + fileData[stPosPt + 2 + i * 2]];
        while (*(unsigned char *) &fileData[j1] != 0)
        {
            unsigned char val = *(unsigned char *) &fileData[j1];
            if ((val >= 1 && val <= 0x60) || (val >= 0xd0 && val <= 0xef))
            {
                tm += a;
            }
            else if (val >= 0x80 && val <= 0xbf)
            {
                a = val - 0x7f;
            }
            else if ((val >= 0xc0 && val <= 0xcf) || val == 0xf0)
            {
                j1++;
            }
            j1++;
        }
    }
    tm *= stDelay;
    return tm;
}

unsigned long PT3GetTime(const char *fileData, unsigned long &loop)
{
    unsigned short a1, a2, a3, a11, a22, a33;
    unsigned long j1, j2, j3;
    long c1, c2, c3, c4, c5, c8;
    long i, j, tm = 0;
    unsigned char b;
    unsigned char ptDelay = fileData[100];
    unsigned char ptNumPos = fileData[101];
    unsigned short ptLoopPos = fileData[102];
    unsigned short ptPatPt = *(unsigned short *) &fileData[103];
    const unsigned char *ptPosList = (unsigned char *) &fileData[201];

    b = ptDelay;
    a11 = a22 = a33 = 1;
    for (i = 0; i < ptNumPos; i++)
    {
        if (i == ptLoopPos)
        {
            loop = tm;
        }
        j1 = *(unsigned short *) &fileData[ptPatPt + ptPosList[i] * 2];
        j2 = *(unsigned short *) &fileData[ptPatPt + ptPosList[i] * 2 + 2];
        j3 = *(unsigned short *) &fileData[ptPatPt + ptPosList[i] * 2 + 4];
        a1 = a2 = a3 = 1;
        do
        {
            a1--;
            if (a1 == 0)
            {
                if (fileData[j1] == 0)
                    break;
                j = c1 = c2 = c3 = c4 = c5 = c8 = 0;
                do
                {
                    unsigned char val = fileData[j1];
                    if (val == 0xd0 || val == 0xc0 || (val >= 0x50 && val <= 0xaf))
                    {
                        a1 = a11;
                        j1++;
                        break;
                    }
                    else if (val == 0x10 || (val >= 0xf0 && val <= 0xff))
                    {
                        j1++;
                    }
                    else if (val >= 0xb2 && val <= 0xbf)
                    {
                        j1 += 2;
                    }
                    else if (val == 0xb1)
                    {
                        j1++;
                        a11 = fileData[j1];
                    }
                    else if (val >= 0x11 && val <= 0x1f)
                    {
                        j1 += 3;
                    }
                    else
                    {
                        switch (val)
                        {
                            case 1:
                                j++;
                                c1 = j;
                                break;
                            case 2:
                                j++;
                                c2 = j;
                                break;
                            case 3:
                                j++;
                                c3 = j;
                                break;
                            case 4:
                                j++;
                                c4 = j;
                                break;
                            case 5:
                                j++;
                                c5 = j;
                                break;
                            case 8:
                                j++;
                                c8 = j;
                                break;
                            case 9:
                                j++;
                                break;
                            default:
                                break;
                        }
                    }
                    j1++;
                } while (true);

                while (j > 0)
                {
                    if (j == c1 || j == c8)
                    {
                        j1 += 3;
                    }
                    else if (j == c2)
                    {
                        j1 += 5;
                    }
                    else if (j == c3 || j == c4)
                    {
                        j1++;
                    }
                    else if (j == c5)
                    {
                        j1 += 2;
                    }
                    else
                    {
                        b = fileData[j1];
                        j1++;
                    }
                    j--;
                }
                a2--;
                if (a2 == 0)
                {
                    j = c1 = c2 = c3 = c4 = c5 = c8 = 0;
                    do
                    {
                        unsigned char val = fileData[j2];
                        if (val == 0xd0 || val == 0xc0 || (val >= 0x50 && val <= 0xaf))
                        {
                            a2 = a22;
                            j2++;
                            break;
                        }
                        else if (val == 0x10 || (val >= 0xf0 && val <= 0xff))
                        {
                            j2++;
                        }
                        else if ((val >= 0xb2) & (val <= 0xbf))
                        {
                            j2 += 2;
                        }
                        else if (val == 0xb1)
                        {
                            j2++;
                            a22 = fileData[j2];
                        }
                        else if (val >= 0x11 && val <= 0x1f)
                        {
                            j2 += 3;
                        }
                        else
                        {
                            switch (val)
                            {
                                case 1:
                                    j++;
                                    c1 = j;
                                    break;
                                case 2:
                                    j++;
                                    c2 = j;
                                    break;
                                case 3:
                                    j++;
                                    c3 = j;
                                    break;
                                case 4:
                                    j++;
                                    c4 = j;
                                    break;
                                case 5:
                                    j++;
                                    c5 = j;
                                    break;
                                case 8:
                                    j++;
                                    c8 = j;
                                    break;
                                case 9:
                                    j++;
                                    break;
                                default:
                                    break;
                            }
                        }
                        j2++;
                    } while (true);
                    while (j > 0)
                    {
                        if (j == c1 || j == c8)
                        {
                            j2 += 3;
                        }
                        else if (j == c2)
                        {
                            j2 += 5;
                        }
                        else if (j == c3 || j == c4)
                        {
                            j2++;
                        }
                        else if (j == c5)
                        {
                            j2 += 2;
                        }
                        else
                        {
                            b = fileData[j2];
                            j2++;
                        }
                        j--;
                    }
                }
                a3--;
                if (a3 == 0)
                {
                    j = c1 = c2 = c3 = c4 = c5 = c8 = 0;
                    do
                    {
                        unsigned char val = fileData[j3];
                        if (val == 0xd0 || val == 0xc0 || (val >= 0x50 && val <= 0xaf))
                        {
                            a3 = a33;
                            j3++;
                            break;
                        }
                        else if (val == 0x10 || (val >= 0xf0 && val <= 0xff))
                        {
                            j3++;
                        }
                        else if ((val >= 0xb2) & (val <= 0xbf))
                        {
                            j3 += 2;
                        }
                        else if (val == 0xb1)
                        {
                            j3++;
                            a33 = fileData[j3];
                        }
                        else if (val >= 0x11 && val <= 0x1f)
                        {
                            j3 += 3;
                        }
                        else
                        {
                            switch (val)
                            {
                                case 1:
                                    j++;
                                    c1 = j;
                                    break;
                                case 2:
                                    j++;
                                    c2 = j;
                                    break;
                                case 3:
                                    j++;
                                    c3 = j;
                                    break;
                                case 4:
                                    j++;
                                    c4 = j;
                                    break;
                                case 5:
                                    j++;
                                    c5 = j;
                                    break;
                                case 8:
                                    j++;
                                    c8 = j;
                                    break;
                                case 9:
                                    j++;
                                    break;
                                default:
                                    break;
                            }
                        }
                        j3++;
                    } while (true);
                    while (j > 0)
                    {
                        if (j == c1 || j == c8)
                        {
                            j3 += 3;
                        }
                        else if (j == c2)
                        {
                            j3 += 5;
                        }
                        else if (j == c3 || j == c4)
                        {
                            j3++;
                        }
                        else if (j == c5)
                        {
                            j3 += 2;
                        }
                        else
                        {
                            b = fileData[j3];
                            j3++;
                        }
                        j--;
                    }
                }
            }
            tm += b;
        } while (true);

    }
    return tm;
}

unsigned long PT2GetTime(const char *fileData, unsigned long &loop)
{
    short a1, a2, a3, a11, a22, a33;
    unsigned long j1, j2, j3;
    long i, tm = 0;
    unsigned char b;
    unsigned char ptDelay = fileData[0];
    unsigned char ptNumPos = fileData[1];
    unsigned short ptLoopPos = fileData [2];
    unsigned short ptPatPt = *(unsigned short *) &fileData[99];
    const unsigned char *ptPosList = (unsigned char *) &fileData[131];

    b = ptDelay;
    a1 = a2 = a3 = a11 = a22 = a33 = 0;
    for (i = 0; i < ptNumPos; i++)
    {
        if (i == ptLoopPos)
        {
            loop = tm;
        }
        j1 = *(unsigned short *) &fileData[ptPatPt + ptPosList[i] * 6];
        j2 = *(unsigned short *) &fileData[ptPatPt + ptPosList[i] * 6 + 2];
        j3 = *(unsigned short *) &fileData[ptPatPt + ptPosList[i] * 6 + 4];
        do
        {
            a1--;
            if (a1 < 0)
            {
                if (fileData[j1] == 0)
                    break;
                do
                {
                    unsigned char val = (unsigned char) fileData[j1];
                    if (val == 0x70 || (val >= 0x80 && val <= 0xe0))
                    {
                        a1 = a11;
                        j1++;
                        break;
                    }
                    else if (val >= 0x71 && val <= 0x7e)
                    {
                        j1 += 2;
                    }
                    else if (val >= 0x20 && val <= 0x5f)
                    {
                        a11 = fileData[j1] - 0x20;
                    }
                    else if (val == 0xf)
                    {
                        j1++;
                        b = fileData[j1];
                    }
                    else if ((val >= 1 && val <= 0xb) || val == 0xe)
                    {
                        j1++;
                    }
                    else if (val == 0xd)
                    {
                        j1 += 3;
                    }
                    j1++;
                } while (true);
            }

            a2--;
            if (a2 < 0)
            {
                do
                {
                    unsigned char val = (unsigned char) fileData[j2];
                    if (val == 0x70 || (val >= 0x80 && val <= 0xe0))
                    {
                        a2 = a22;
                        j2++;
                        break;
                    }
                    else if (val >= 0x71 && val <= 0x7e)
                    {
                        j2 += 2;
                    }
                    else if (val >= 0x20 && val <= 0x5f)
                    {
                        a22 = fileData[j2] - 0x20;
                    }
                    else if (val == 0xf)
                    {
                        j2++;
                        b = fileData[j2];
                    }
                    else if ((val >= 1 && val <= 0xb) || val == 0xe)
                    {
                        j2++;
                    }
                    else if (val == 0xd)
                    {
                        j2 += 3;
                    }
                    j2++;
                } while (true);
            }
            a3--;
            if (a3 < 0)
            {
                do
                {
                    unsigned char val = (unsigned char) fileData[j3];
                    if (val == 0x70 || (val >= 0x80 && val <= 0xe0))
                    {
                        a3 = a33;
                        j3++;
                        break;
                    }
                    else if (val >= 0x71 && val <= 0x7e)
                    {
                        j3 += 2;
                    }
                    else if (val >= 0x20 && val <= 0x5f)
                    {
                        a33 = fileData[j3] - 0x20;
                    }
                    else if (val == 0xf)
                    {
                        j3++;
                        b = fileData[j3];
                    }
                    else if ((val >= 1 && val <= 0xb) || val == 0xe)
                    {
                        j3++;
                    }
                    else if (val == 0xd)
                    {
                        j3 += 3;
                    }
                    j3++;
                } while (true);
            }
            tm += b;
        } while (true);
    }
    return tm;
}
