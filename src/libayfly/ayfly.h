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

#ifndef AYFLY_H_
#    define AYFLY_H_

#    ifndef UNICODE
#        define UNICODE
#    endif

#    ifndef _UNICODE
#        define _UNICODE
#    endif

#    ifdef _WINDOWS
#		ifndef WINDOWS
#			define WINDOWS
#		endif
#    endif

#    ifdef WINDOWS
#		ifndef _WINDOWS
#			define _WINDOWS
#		endif
#        define WIN32_LEAN_AND_MEAN
#        include <windows.h>
#        include <commdlg.h>
#        include <tchar.h>
#    endif

#    include <stdio.h>
#    include <stdlib.h>
#    include <string.h>
#    include <math.h>


#    ifndef __SYMBIAN32__
#    include <algorithm>
#    include <cctype>
#    include <wchar.h>
#    include <string>
#    include <fstream>
#    include <iostream>
#    else
#    include "f32file.h"
#    include "s32std.h"
#    include "s32file.h"
#    include "e32cmn.h"
#    include "coemain.h"
#    endif


extern "C"
{
#    include "z80ex/include/z80ex.h"
}
#    ifdef _MSC_VER
#        pragma warning(disable:4309)
#    endif
#    ifndef __SYMBIAN32__
#        define AUDIO_FREQ 44100
#        define TXT(x) L##x
#        define AY_TXT_TYPE std::wstring
#    else
#        define TXT(x) _L(x)
#        pragma pack(1)
#        define AUDIO_FREQ 32000
#    endif
#    define Z80_FREQ 3546900
#    define INT_FREQ 50
#ifndef sizeof_array
#    define sizeof_array(x) sizeof(x) / sizeof(x [0])
#endif

struct AYSongInfo;

typedef void (*ELAPSED_CALLBACK)(void *arg);
typedef void (*PLAYER_INIT_PROC)(AYSongInfo &info);
typedef void (*PLAYER_PLAY_PROC)(AYSongInfo &info);
typedef void (*PLAYER_CLEANUP_PROC)(AYSongInfo &info);

#include "ay.h"
#include "AbstractAudio.h"

struct AYSongInfo
{
#ifndef __SYMBIAN32__
    AY_TXT_TYPE Author; /* Song author */
    AY_TXT_TYPE Name; /* Song name */
    AY_TXT_TYPE FilePath; /* Song file path */
#else
    TFileName Author;
    TFileName Name;
    TFileName FilePath;
#endif
    unsigned long Length; /* Song length in seconds */
    unsigned long Loop; /* Loop start position */
    bool bEmul; /* player is in z80 asm? */
    PLAYER_INIT_PROC init_proc; /* init for soft player */
    PLAYER_PLAY_PROC play_proc; /* play for soft player */
    PLAYER_CLEANUP_PROC cleanup_proc; /* play for soft player */
    void *data; /* used for players */
    unsigned char module [65536]; /* z80 memory or raw song data */
    unsigned char file_data [65536]; /* z80 memory or raw song data */
    unsigned char z80IO [65536]; /* z80 ports */
    unsigned long file_len; /* file length */
    AbstractAudio *player; /* player for this song */
    Z80EX_CONTEXT *z80ctx; /* z80 execution context */
    unsigned long timeElapsed; /* playing time in tacts */
    ELAPSED_CALLBACK callback; /* song end callback function */
    void *callback_arg; /* argument for callback */
    unsigned short ay_reg; /* current AY register */
    unsigned long z80_freq; /* z80 cpu frequency */
    unsigned long ay_freq; /* AY chip frequency */
    unsigned long int_freq; /* interrupts frequency */
    unsigned long sr; /* sample rate */
};


#ifndef __SYMBIAN32__
#ifndef WINDOWS
#include "unix/SDLAudio.h"
#else
#include "windows/DXAudio.h"
#endif
#else
#include "s60/ayfly_s60Audio.h"
#endif


//system functions
bool ay_sys_readfromfile(AYSongInfo &info);
bool ay_sys_getsonginfo(AYSongInfo &info);
bool ay_sys_getsonginfoindirect(AYSongInfo &info);
void ay_sys_rewindsong(AYSongInfo &info, long new_position);
bool ay_sys_initz80(AYSongInfo &info);
void ay_sys_z80exec(AYSongInfo &info);
void ay_sys_resetz80(AYSongInfo &info);
void ay_sys_shutdownz80(AYSongInfo &info);
bool ay_sys_initsong(AYSongInfo &info);

//common functions
#ifndef __SYMBIAN32__
void *ay_initsong(const wchar_t *FilePath, unsigned long sr);
#else
void *ay_initsong(TFileName FilePath, unsigned long sr);
#endif
#ifndef __SYMBIAN32__
void *ay_initsongindirect(unsigned char *module, unsigned long sr, wchar_t *type, unsigned long size);
#else
void *ay_initsongindirect(unsigned char *module, unsigned long sr, TFileName type, unsigned long size);
#endif
#ifndef __SYMBIAN32__
void *ay_getsonginfo(const wchar_t *FilePath);
#else
void *ay_getsonginfo(TFileName FilePath);
#endif
#ifndef __SYMBIAN32__
void *ay_getsonginfoindirect(unsigned char *module, wchar_t *type, unsigned long size);
#else
void *ay_getsonginfoindirect(unsigned char *module, TFileName type, unsigned long size);
#endif
#ifndef __SYMBIAN32__
const wchar_t *ay_getsongname(void *info);
#else
TFileName ay_getsongname(void *info);
#endif
#ifndef __SYMBIAN32__
const wchar_t *ay_getsongauthor(void *info);
#else
TFileName ay_getsongauthor(void *info);
#endif
#ifndef __SYMBIAN32__
const wchar_t *ay_getsongpath(void *info);
#else
TFileName ay_getsongpath(void *info);
#endif
void ay_z80xec(void *info);
void ay_seeksong(void *info, long new_position);
void ay_resetsong(void *info);
void ay_closesong(void **info);
bool ay_songstarted(void *info);
void ay_startsong(void *info);
void ay_stopsong(void *info);
void ay_setvolume(void *info, unsigned long chnl, double volume);
double ay_getvolume(void *info, unsigned long chnl);
void ay_chnlmute(void *info, unsigned long chnl, bool mute);
double ay_chnlmuted(void *info, unsigned long chnl);
void ay_setcallback(void *info, ELAPSED_CALLBACK callback, void *callback_arg);
unsigned long ay_getsonglength(void *info);
unsigned long ay_getelapsedtime(void *info);
unsigned long ay_getsongloop(void *info);
const unsigned char *ay_getregs(void *info, unsigned long chip_num);
void ay_rendersongbuffer(void *info, unsigned char *buffer, unsigned long buffer_length, unsigned long chip_num);
unsigned long ay_getz80freq(void *info);
void ay_setz80freq(void *info, unsigned long z80_freq);
unsigned long ay_getayfreq(void *info);
void ay_setayfreq(void *info, unsigned long ay_freq);
unsigned long ay_getintfreq(void *info);
void ay_setintfreq(void *info, unsigned long int_freq);
void ay_setsongplayer(void *info, void * /* class AbstractAudio */ player);
void *ay_getsongplayer(void *info);


#    define AYFLY_VERSION_MAJOR 0
#    define AYFLY_VERSION_MINOR 0
#    define AYFLY_VERSION_PATCH 15

#    define AYFLY_MAKE_VERSION ((AYFLY_VERSION_MAJOR << 16) | (AYFLY_VERSION_MINOR << 8) | AYFLY_VERSION_PATCH)
#    define AYFLY_VERSION AYFLY_VERSION_MAJOR.AYFLY_VERSION_MINOR.AYFLY_VERSION_PATCH
#    define xstr(s) str(s)
#    define str(s) TXT(#s)
#    define AYFLY_VERSION_TEXT xstr(AYFLY_VERSION)

#endif /*AYFLY_H_*/
