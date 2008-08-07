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

#ifndef COMMON_H_
#    define COMMON_H_

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
#        include "windows.h"
#        include "commdlg.h"
#        include "tchar.h"
#    endif

#    include "stdio.h"
#    include "stdlib.h"
#    include "math.h"
#    include "string.h"

extern "C"
{
#    include "z80ex/include/z80ex.h"
}
#    ifdef _MSC_VER
#        pragma warning(disable:4309)
#    endif

#    ifndef __SYMBIAN32__
#        define AUDIO_FREQ 44100
#        define TXT(x) wxT(x)
#        define TXT_TYPE wxString
#    else
#        define TXT(x) _L(x)
#        define TXT_TYPE TFileName
#        pragma pack(1)
#        define AUDIO_FREQ 32000
#    endif
#    define Z80_FREQ 3546900
#    define INTR_FREQ 50
#    define Z80_TO_INTR (Z80_FREQ / INTR_FREQ)
#    define Z80_TO_AUDIO (Z80_FREQ/AUDIO_FREQ)
#    define AY_CLOCK 1773400
#ifndef sizeof_array
#    define sizeof_array(x) sizeof(x) / sizeof(x [0])
#endif

typedef void (*ELAPSED_CALLBACK)(void *arg);
typedef void (*PLAYER_INIT_PROC)(unsigned char *module);
typedef void (*PLAYER_PLAY_PROC)(unsigned char *module, ELAPSED_CALLBACK callback, void *arg);

struct SongInfo
{
    TXT_TYPE Author; /* Song author */
    TXT_TYPE Name; /* Song name */
    TXT_TYPE FilePath;
    unsigned long Length; /* Song length in seconds */
    unsigned long Loop; /* Loop start position */
    bool bEmul; /* player is in z80 asm? */
    PLAYER_INIT_PROC soft_init_proc; /* init for soft player */
    PLAYER_PLAY_PROC soft_play_proc; /* play for soft player */
};

#include "ay.h"
#include "AbstractAudio.h"

#ifndef WINDOWS
#include "unix/SDLAudio.h"
#else
#include "windows/DXAudio.h"
#endif


//loader functions
bool readFile(SongInfo &info);
bool getSongInfo(SongInfo &info);
void rewindSong(SongInfo &info, long new_position);
unsigned long timeElapsed;
unsigned long maxElapsed;
PLAYER_INIT_PROC soft_init_proc;
PLAYER_PLAY_PROC soft_play_proc;

//emulator functions
void initSpeccy();
void resetSpeccy();
void shutdownSpeccy();
void setPlayer(AbstractAudio *_player);
void execInstruction(ELAPSED_CALLBACK callback, void *arg);
unsigned char *z80Memory;
Z80EX_CONTEXT *ctx;
AbstractAudio *player;


#    define AYFLY_VERSION_MAJOR 0
#    define AYFLY_VERSION_MINOR 0
#    define AYFLY_VERSION_PATCH 15

#    define AYFLY_MAKE_VERSION ((AYFLY_VERSION_MAJOR << 16) | (AYFLY_VERSION_MINOR << 8) | AYFLY_VERSION_PATCH)
#    define AYFLY_VERSION AYFLY_VERSION_MAJOR.AYFLY_VERSION_MINOR.AYFLY_VERSION_PATCH
#    define xstr(s) str(s)
#    define str(s) TXT(#s)
#    define AYFLY_VERSION_TEXT xstr(AYFLY_VERSION)

#endif /*COMMON_H_*/
