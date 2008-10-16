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
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool end;

bool elapsed_callback(void *)
{
    printf("!!\n");
    end = true;
    return end;
}

int main(int argc, char **argv)
{
    wprintf(L"AY 891x file converter v. " AYFLY_VERSION_TEXT L", ");
    wprintf(L"Deryabin Andrew, 2008. GNU GPL v2 license.\n");
    if(argc < 2)
    {
        wprintf(L"\tusage: ayfly_converter ay_file > raw_pcm_file\n");
        exit(1);
    }
    
    end = false;
    wchar_t song_name [65536];
    swprintf(song_name, sizeof(song_name) - 1, L"%S", argv [1]);
    wprintf(L"Converting song %s...\n", song_name);    
    void *song = ay_initsong(song_name, 44100);    
    if(!song)
    {
        wprintf(L"Can't open song!\n");
        exit(1);
    }
    
    ay_setelapsedcallback(song, elapsed_callback, 0);
    unsigned char buffer [8192];
    
    while(!end)
    {
        ay_rendersongbuffer(song, buffer, sizeof(buffer));
        write(0, buffer, sizeof(buffer));
    }
    
    ay_closesong(&song);
            
}
