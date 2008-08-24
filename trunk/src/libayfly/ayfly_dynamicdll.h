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

//Dynamic dll functions for LoadLibrary run-time linking


#ifndef AYFLY_DYNAMICDLL_H_
#    define AYFLY_DYNAMICDLL_H_

#    ifndef UNICODE
#        define UNICODE
#    endif

#    ifndef _UNICODE
#        define _UNICODE
#    endif

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
#        include <commdlg.h>
#        include <tchar.h>
#    endif

typedef void (*ELAPSED_CALLBACK)(void *arg);

//import functions
typedef void * (*ay_initsong)(const wchar_t *FilePath, unsigned long sr);
typedef void * (*ay_initsongindirect)(unsigned char *module, unsigned long sr, wchar_t *type, unsigned long size);
typedef void * (*ay_getsonginfo)(const wchar_t *FilePath);
typedef void * (*ay_getsonginfoindirect)(unsigned char *module, wchar_t *type, unsigned long size);
typedef const wchar_t * (*ay_getsongname)(void *info);
typedef const wchar_t * (*ay_getsongauthor)(void *info);
typedef const wchar_t * (*ay_getsongpath)(void *info);
typedef void  (*ay_z80xec)(void *info);
typedef void  (*ay_seeksong)(void *info, long new_position);
typedef void  (*ay_resetsong)(void *info);
typedef void  (*ay_closesong)(void **info);
typedef bool  (*ay_songstarted)(void *info);
typedef void  (*ay_startsong)(void *info);
typedef void  (*ay_stopsong)(void *info);
typedef void  (*ay_setvolume)(void *info, unsigned long chnl, float volume);
typedef float  (*ay_getvolume)(void *info, unsigned long chnl);
typedef void  (*ay_chnlmute)(void *info, unsigned long chnl, bool mute);
typedef bool  (*ay_chnlmuted)(void *info, unsigned long chnl);
typedef void  (*ay_setcallback)(void *info, ELAPSED_CALLBACK callback, void *callback_arg);
typedef unsigned long  (*ay_getsonglength)(void *info);
typedef unsigned long  (*ay_getelapsedtime)(void *info);
typedef unsigned long  (*ay_getsongloop)(void *info);
typedef const unsigned char * (*ay_getregs)(void *info, unsigned long chip_num);
typedef void  (*ay_rendersongbuffer)(void *info, unsigned char *buffer, unsigned long buffer_length, unsigned long chip_num);
typedef unsigned long  (*ay_getz80freq)(void *info);
typedef void  (*ay_setz80freq)(void *info, unsigned long z80_freq);
typedef unsigned long  (*ay_getayfreq)(void *info);
typedef void  (*ay_setayfreq)(void *info, unsigned long ay_freq);
typedef unsigned long  (*ay_getintfreq)(void *info);
typedef void  (*ay_setintfreq)(void *info, unsigned long int_freq);
typedef void  (*ay_setsongplayer)(void *info, void * /* class AbstractAudio */ player);
typedef void * (*ay_getsongplayer)(void *info);
typedef void  (*ay_sethwnd)(void *info, HWND hWnd);


#endif /*AYFLY_H_*/
