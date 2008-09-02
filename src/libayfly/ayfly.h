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

#ifdef _UNICODE
#   ifndef UNICODE
#       define UNICODE
#   endif
#   define AY_CHAR wchar_t
#else
#   define ANSI
#   define AY_CHAR char
#endif

#    ifdef WIN32
#		ifndef _WINDOWS
#			define _WINDOWS
#       endif
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
#		 include <mmsystem.h>
#        include <commdlg.h>
#        include <tchar.h>
#    endif

#    include <stdio.h>
#    include <stdlib.h>
#    include <string.h>

#    ifndef __SYMBIAN32__
#    include <algorithm>
#    include <cctype>
#    include <wchar.h>
#    include <string>
#    include <fstream>
#    include <iostream>
#    else
#ifdef EKA2
#    include <e32cmn.h>
#endif
#    include <f32file.h>
#    include <s32std.h>
#    include <s32file.h>
#    include <coemain.h>
#    include <mda/common/audio.h>
#    include <mdaaudiooutputstream.h>
#    endif


#    include "z80ex/include/z80ex.h"
#    ifdef _MSC_VER
#        pragma warning(disable:4309)
#        ifdef AYFLY_EXPORTS
#           define AYFLY_API __declspec(dllexport)
#        else
#           ifndef AYFLY_STATIC
#               define AYFLY_API __declspec(dllimport)
#           else
#               define AYFLY_API
#           endif
#        endif
#    else
#        define AYFLY_API
#    endif
#    ifndef __SYMBIAN32__
#        define AUDIO_FREQ 44100
#        ifdef UNICODE
#        define TXT(x) L##x
#           define AY_TXT_TYPE std::wstring
#        else
#        define TXT(x) x
#           define AY_TXT_TYPE std::string
#        endif
#    else
#        define TXT(x) _S(x)
#        define AUDIO_FREQ 32000
#    endif
#    define Z80_FREQ 3546900
#    define INT_FREQ 50
#ifndef sizeof_array
#    define sizeof_array(x) sizeof(x) / sizeof(x [0])
#endif

struct AYSongInfo;

/*
 * Prototype for callback function, called when
 * song end reached. Example code:
 * void callback(void *songinfo)
 * {
 *      ay_stopsong(songinfo);
 *      wprintf("Song %s ended!\n", ay_getsongname(songinfo));
 *      exit(0);
 * }
 */
typedef void (*ELAPSED_CALLBACK)(void *arg);

/* System callback prototypes */
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
    unsigned char *module; /* z80 memory or raw song data */
    unsigned char *file_data; /* z80 memory or raw song data */
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
    unsigned char chip_type; /* chip type: AY = 0 or YM = 1 */
    ~AYSongInfo();
};


#ifndef __SYMBIAN32__
#ifndef WINDOWS
#include "unix/SDLAudio.h"
#else
#include "windows/WaveAudio.h"
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

#ifdef __cplusplus
extern "C" {
#endif

/* common functions */


/*
 * Initializes song with given file path @FilePath
 * and sample rate @sr. Example:
 *
 * int _tmain(int argc, wchar_t **argv)
 * {
 *      void *songinfo = ay_initsong(L"E:\\Authors\\Ksa\\E-MEGAMIX.stc", 44100);
 *      if(songinfo == 0)
 *      {
 *          printf("Can't open song!\n");
 *          exit(1);
 *      }
 *      ay_closesong(&songinfo);
 *      return 0;
 * }
 */

#ifndef __SYMBIAN32__
AYFLY_API void *ay_initsong(const AY_CHAR *FilePath, unsigned long sr);
#else
AYFLY_API void *ay_initsong(TFileName FilePath, unsigned long sr);
#endif

/*
 * Initializes song from memory ares pointed by @module of size @size,
 * with given sample rate @sr, given type @type. Example:
 *
 * usigng namespace std;
 * int _tmain(int argc, wchar_t **argv)
 * {
 *      ifstream f;
 *      f.open("E:\\Authors\\Ksa\\E-MEGAMIX.stc", ios_base::in | ios_base::binary);
 *      f.seekg(0, std::ios::end);
 *      size_t size = f.tellg();
 *      f.seekg(0, std::ios::beg);
 *      char *buffer = new char [size];
 *      void *songinfo = ay_initsongindirect(buffer, 44100, L".stc", size);
 *      if(songinfo == 0)
 *      {
 *          printf("Can't open song!\n");
 *          exit(1);
 *      }
 *      ay_closesong(&songinfo);
 *      return 0;
 * }
 */

#ifndef __SYMBIAN32__
AYFLY_API void *ay_initsongindirect(unsigned char *module, unsigned long sr, AY_CHAR *type, unsigned long size);
#else
AYFLY_API void *ay_initsongindirect(unsigned char *module, unsigned long sr, TFileName type, unsigned long size);
#endif

/*
 * Returns pointer to songinfo system structure for song in @FilePath
 * for further manipulation. Example:
 *
 * void *songinfo = ay_getsonginfo(L"E:\\Authors\\Ksa\\E-MEGAMIX.stc");
 * wchar_t *songname = ay_getsongname(songinfo);
 * unsigned long songlength = ay_getsonglength(songinfo);
 * wprintf("song name = %s, length = %lu\n", songname, songlength);
 * ay_closesong(&songinfo);
 *
 */
#ifndef __SYMBIAN32__
AYFLY_API void *ay_getsonginfo(const AY_CHAR *FilePath);
#else
AYFLY_API void *ay_getsonginfo(TFileName FilePath);
#endif
/*
 * Returns pointer to songinfo system structure for song in memory area @module
 * of size @size for further manipulation. Example:
 * ifstream f;
 * f.open("E:\\Authors\\Ksa\\E-MEGAMIX.stc", ios_base::in | ios_base::binary);
 * f.seekg(0, std::ios::end);
 * size_t size = f.tellg();
 * f.seekg(0, std::ios::beg);
 * char *buffer = new char [size];
 * void *songinfo = ay_getsonginfoindirect(buffer, L".stc", size);
 * wchar_t *songname = ay_getsongname(songinfo);
 * unsigned long songlength = ay_getsonglength(songinfo);
 * wprintf("song name = %s, length = %lu\n", songname, songlength);
 * ay_closesong(&songinfo);
 *
 */
#ifndef __SYMBIAN32__
AYFLY_API void *ay_getsonginfoindirect(unsigned char *module, AY_CHAR *type, unsigned long size);
#else
AYFLY_API void *ay_getsonginfoindirect(unsigned char *module, TFileName type, unsigned long size);
#endif

/*
 * Gets song name from initialized songinfo @info pointer.
 * Example:
 *
 * void *songinfo = ay_initsong("D:/123.ay", 32000);
 * wchar_t *songname = ay_getsongname(songinfo);
 * wprintf("song name = %s", songname);
 */

#ifndef __SYMBIAN32__
AYFLY_API const AY_CHAR *ay_getsongname(void *info);
#else
AYFLY_API TFileName ay_getsongname(void *info);
#endif

/*
 * Gets song author from initialized songinfo @info pointer.
 * Example:
 *
 * void *songinfo = ay_initsong("D:/123.ay", 32000);
 * wchar_t *songauthor = ay_getsongauthor(songinfo);
 * wprintf("song author = %s", songauthor);
 */

#ifndef __SYMBIAN32__
AYFLY_API const AY_CHAR *ay_getsongauthor(void *info);
#else
AYFLY_API TFileName ay_getsongauthor(void *info);
#endif

/*
 * Gets song file path from initialized songinfo @info pointer.
 * Example:
 *
 * void *songpath = ay_initsong("D:/123.ay", 32000);
 * wchar_t *songpath = ay_getsongpath(songinfo);
 * wprintf("song path = %s", songauthor); // = "D:/123.ay";
 */

#ifndef __SYMBIAN32__
AYFLY_API const AY_CHAR *ay_getsongpath(void *info);
#else
AYFLY_API TFileName ay_getsongpath(void *info);
#endif
/*
 * execute one quant of z80 code, or soft player code.
 * Used only in ay emulator and song positioning
 */

AYFLY_API void ay_z80xec(void *info);

/*
 * Set song pointer to new position @new_position
 * @new_position is a ralative offset from
 * elapsed song time. To get relative position from absolute value,
 * do following:
 * unsigned long absolute_pos = 1000; in 1/50 second
 * unsigned long current_pos = ay_getelapsedtime(songinfo);
 * unsigned long relative_pos = absolute_pos - current_pos;
 * ay_seksong(songinfo, relative_pos);
 */

AYFLY_API void ay_seeksong(void *info, long new_position);

/*
 * Resets song to zero position and clears AY registers
 */

AYFLY_API void ay_resetsong(void *info);

/*
 * Closes song and frees resources
 */

AYFLY_API void ay_closesong(void **info);

/*
 * Returns true, if song is playing.
 */

AYFLY_API bool ay_songstarted(void *info);

/*
 * Starts song
 */

AYFLY_API void ay_startsong(void *info);

/*
 * Stops song
 */

AYFLY_API void ay_stopsong(void *info);

/*
 * Sets AY chip @chip_num volume of channel @chnl to volume @volume.
 * @volume must be in range from 0 to 1.
 */

AYFLY_API void ay_setvolume(void *info, unsigned long chnl, float volume, unsigned long chip_num = 0);

/*
 * Returns AY chip @chip_num volume of channel @chnl in range from 0 to 1
 */

AYFLY_API float ay_getvolume(void *info, unsigned long chnl, unsigned long chip_num = 0);
/*
 * Mutes or enables AY chip @chip channel @chnl. If @mute is true - channel muted.
 */
AYFLY_API void ay_chnlmute(void *info, unsigned long chnl, bool mute, unsigned long chip_num = 0);

/*
 * Returns true if channel @chnl of AY chip @chip_num is muted
 */

AYFLY_API bool ay_chnlmuted(void *info, unsigned long chnl, unsigned long chip_num = 0);

/*
 * Sets callback function for song described by @info to function pointed by
 * @callback. When the function called, @callback_arg is passed as the only argument.
 * Function called when song @info ended, or reverted to the loop position
 */

AYFLY_API void ay_setcallback(void *info, ELAPSED_CALLBACK callback, void *callback_arg);

/*
 * Gets song length in 1/50 second
 */

AYFLY_API unsigned long ay_getsonglength(void *info);

/*
 * Gets song elapsed time in 1/50 second
 */

AYFLY_API unsigned long ay_getelapsedtime(void *info);

/*
 * Gets song loop position in 1/50 second
 */

AYFLY_API unsigned long ay_getsongloop(void *info);

/*
 * Gets pointer to array of 15 bytes - AY register buffer.
 * Buffer is only for reading
 */

AYFLY_API const unsigned char *ay_getregs(void *info, unsigned long chip_num = 0);

/*
 * User For render song AY chip @chip_num into buffer pointed by @buffer
 * with size @buffer_length bytes
 */

AYFLY_API void ay_rendersongbuffer(void *info, unsigned char *buffer, unsigned long buffer_length, unsigned long chip_num);

/*
 * Returns z80 frequency
 */

AYFLY_API unsigned long ay_getz80freq(void *info);

/*
 * Sets z80 frequency
 */


AYFLY_API void ay_setz80freq(void *info, unsigned long z80_freq);

/*
 * Gets AY frequency
 */

AYFLY_API unsigned long ay_getayfreq(void *info);

/*
 * Sets AY frequency to @ay_freq
 */

AYFLY_API void ay_setayfreq(void *info, unsigned long ay_freq);

/*
 * Gets Z80 interrupt frequency
 */

AYFLY_API unsigned long ay_getintfreq(void *info);

/*
 * Sets Z80 interrupt frequency to @int_freq
 */

AYFLY_API void ay_setintfreq(void *info, unsigned long int_freq);

/*
 * Gets song sample rate
 */

AYFLY_API unsigned long ay_getsamplerate(void *info);

/*
 * Sets song sample rate to @sr
 */

AYFLY_API void ay_setsamplerate(void *info, unsigned long sr);

/*
 * Sets song player. @player must point to class, derived from
 * class AbstractAudio
 */

AYFLY_API void ay_setsongplayer(void *info, void * /* class AbstractAudio */ player);

/*
 * Gets pointer to current song player
 */

AYFLY_API void *ay_getsongplayer(void *info);

/*
 * Sets chip type. 0 = AY, 1 = YM
 */

AYFLY_API void ay_setchiptype(void *info, unsigned char chip_type);

/*
 * Gets chip type. 0 = AY, 1 = YM
 */

AYFLY_API unsigned char ay_getchiptype(void *info);

/*
 * Sets window handle, used for directx init procedure
 */

#ifdef WINDOWS
AYFLY_API void ay_sethwnd(void *info, HWND hWnd);
#endif

#ifdef __cplusplus
}
#endif


#    define AYFLY_VERSION_MAJOR 0
#    define AYFLY_VERSION_MINOR 0
#    define AYFLY_VERSION_PATCH 16

#    define AYFLY_MAKE_VERSION ((AYFLY_VERSION_MAJOR << 16) | (AYFLY_VERSION_MINOR << 8) | AYFLY_VERSION_PATCH)
#    define AYFLY_VERSION AYFLY_VERSION_MAJOR.AYFLY_VERSION_MINOR.AYFLY_VERSION_PATCH
#    define xstr(s) str(s)
#    define str(s) TXT(#s)
#    define AYFLY_VERSION_TEXT xstr(AYFLY_VERSION)

#endif /*AYFLY_H_*/