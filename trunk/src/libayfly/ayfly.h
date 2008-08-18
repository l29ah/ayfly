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
#        include <windows.h>
#        include <commdlg.h>
#        include <tchar.h>
#    endif

#    include <stdio.h>
#    include <stdlib.h>
#    include <math.h>


#    ifndef __SYMBIAN32__
#    include <algorithm>
#    include <cctype>
#    include <wchar.h>
#    include <string>
#    include <fstream>
#    include <iostream>
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
#ifndef __SYMBIAN32__
    AY_TXT_TYPE Author; /* Song author */
    AY_TXT_TYPE Name; /* Song name */
    AY_TXT_TYPE FilePath;
#else
    TFileName Author;
    TFileName Name;
    TFileName FilePath;
#endif
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
bool ay_readfromfile(SongInfo &info);
bool ay_getsonginfo(SongInfo &info);
void ay_rewindsong(SongInfo &info, long new_position);
extern unsigned long timeElapsed;
extern unsigned long maxElapsed;
extern PLAYER_INIT_PROC soft_init_proc;
extern PLAYER_PLAY_PROC soft_play_proc;

//emulator functions
void ay_initz80();
void ay_resetz80();
void ay_shutdownz80();
void setPlayer(AbstractAudio *_player);
void execInstruction(ELAPSED_CALLBACK callback, void *arg);
extern unsigned char *z80Memory;
extern Z80EX_CONTEXT *ctx;
extern AbstractAudio *player;


#    define AYFLY_VERSION_MAJOR 0
#    define AYFLY_VERSION_MINOR 0
#    define AYFLY_VERSION_PATCH 15

#    define AYFLY_MAKE_VERSION ((AYFLY_VERSION_MAJOR << 16) | (AYFLY_VERSION_MINOR << 8) | AYFLY_VERSION_PATCH)
#    define AYFLY_VERSION AYFLY_VERSION_MAJOR.AYFLY_VERSION_MINOR.AYFLY_VERSION_PATCH
#    define xstr(s) str(s)
#    define str(s) TXT(#s)
#    define AYFLY_VERSION_TEXT xstr(AYFLY_VERSION)

#endif /*COMMON_H_*/
