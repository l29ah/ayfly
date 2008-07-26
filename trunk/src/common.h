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

typedef void (*ELAPSED_CALLBACK)(void *arg);
typedef unsigned long (*GETTIME_CALLBACK)(const char *fileData, unsigned long &loop);

#    include "stdio.h"
#    include "stdlib.h"
#    include "math.h"
#    include "string.h"
#    include "Filter3.h"
#ifndef __SYMBIAN32__
#    include "ay.h"
#else
#    include "ay_s60.h"
#endif
#    include "AbstractAudio.h"

extern "C"
{
#    include "z80ex/include/z80ex.h"
#    include "z80ex/include/z80ex_dasm.h"
}
#    ifdef _MSC_VER
#        pragma warning(disable:4309)
#    endif

#    ifndef __SYMBIAN32__
#        include "wx/wx.h"
#        include "wx/file.h"
#        include "wx/filedlg.h"
#        include "wx/toolbar.h"
#        include "wx/dnd.h"
#        include "wx/slider.h"
#        include "wx/spinbutt.h"
#        include "wx/tglbtn.h"
#        include "wx/listctrl.h"
#        include "wx/dialog.h"
#        ifndef WINDOWS
#            include "SDL.h"
#            include "SDL_rwops.h"
#            include "unix/SDLAudio.h"
#        else
#            include "ole2.h"
#            include "shlobj.h"
#            include "shellapi.h"
#            include "mmsystem.h"
#            ifndef _WIN32_WCE
#                ifdef _MSC_VER
#                    include "strsafe.h"
#                endif
#                include "dsound.h"
#                include "windows/DXAudio.h"
#                include "windows/MainDropTarget.h"
#            else
#                include "aygshell.h"
#                include "windows/WaveAudio.h"
#            endif
#            define WM_SWITCH_LANG (WM_USER + 1)
#            define WM_DROP_FILE (WM_USER + 2)
#        endif
#    else
#        include "eikstart.h"
#        include "f32file.h"
#        include "s32std.h"
#        include "s32file.h"
#        include "e32cons.h"
#        include "ayfly_s60.hrh"
#        include "ayfly_s60.pan"
#        include "ayfly_s60Document.h"
#        include "ayfly_s60Application.h"
#        include "ayfly_s60AppUi.h"
#        include "ayfly_s60AppView.h"
#        include "ayfly_s60Audio.h"
#        include "ayfly_s60_0xEA0B66F6.rsg"
#        include "avkon.hrh"
#        include "coemain.h"
#        include "aknmessagequerydialog.h"
#        include "aknnotewrappers.h"
#        include "stringloader.h"
#        include "hlplch.h"
#        include "caknfileselectiondialog.h"
#        include "pathinfo.h"
#    endif

#    ifndef __SYMBIAN32__
#    define AUDIO_FREQ 44100
#    else
#    define AUDIO_FREQ 32000
#    endif
#    define Z80_FREQ 3500000
#    define INTR_FREQ 50
#    define Z80_TO_INTR (Z80_FREQ / INTR_FREQ)
#    define Z80_TO_AUDIO (Z80_FREQ/AUDIO_FREQ)

#    define sizeof_array(x) sizeof(x) / sizeof(x [0])

#    ifndef __SYMBIAN32__
#        define TXT(x) wxT(x)
#        define TXT_TYPE wxString
#    else
#        define TXT(x) _L(x)
#        define TXT_TYPE TPtrC
#    endif

//loader functions
#    ifndef __SYMBIAN32__
extern bool readFile(const TXT_TYPE &filePath);
#    else
extern bool readFile(const TDesC &filePath);
extern CConsoleBase *gConsole;
#    endif

extern unsigned long timeElapsed;
extern unsigned long maxElapsed;


struct SongInfo
{
    TXT_TYPE Author; /* Song author */
    TXT_TYPE Name; /* Song name */
    TXT_TYPE FilePath;
    unsigned long Length; /* Song length in seconds */
    unsigned long Loop;
};


#    ifndef __SYMBIAN32__
struct bindings
{
    wxString name; /* button name */
    wxString str_id; /* string id for action */
    int id; /* int id for action */
    int key; /* key mapping */
    int modifier; /* modifier */
};
#    endif


#    ifndef __SYMBIAN32__
#        include "gui/AyflyApp.h"
#        include "gui/AyflyFrame.h"
#        include "gui/DnDFiles.h"
#        include "gui/AyflyBindingsDlg.h"
#    endif


//emulator functions
extern void initSpeccy();
extern void resetSpeccy();
extern void shutdownSpeccy();
extern void setPlayer(AbstractAudio *_player);
extern void execInstruction(ELAPSED_CALLBACK callback, void *arg);
extern bool getSongInfo(SongInfo *info);
extern unsigned char *z80Memory;
extern Z80EX_CONTEXT *ctx;
extern AbstractAudio *player;


#    define AYFLY_VERSION_MAJOR 0
#    define AYFLY_VERSION_MINOR 0
#    define AYFLY_VERSION_PATCH 12

#    define AYFLY_MAKE_VERSION ((AYFLY_VERSION_MAJOR << 16) | (AYFLY_VERSION_MINOR << 8) | AYFLY_VERSION_PATCH)
#    define AYFLY_VERSION AYFLY_VERSION_MAJOR.AYFLY_VERSION_MINOR.AYFLY_VERSION_PATCH
#    define xstr(s) str(s)
#    define str(s) TXT(#s)
#    define AYFLY_VERSION_TEXT xstr(AYFLY_VERSION)


#endif /*COMMON_H_*/
