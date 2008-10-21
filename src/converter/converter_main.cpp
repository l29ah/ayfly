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
    fwprintf(stderr, L"AY 891x file converter v. " AYFLY_VERSION_TEXT L", ");
    fwprintf(stderr, L"Deryabin Andrew, 2008. GNU GPL v2 license.\n");
    if(argc < 2)
    {
        fwprintf(stderr, L"\tusage: ayfly_converter ay_file > raw_pcm_file\n");
        exit(1);
    }

    end = false;
    wchar_t song_name [65536];
    mbstate_t mbstate;
    ::memset((void*)&mbstate, 0, sizeof(mbstate));
    const char *strc = argv [1];
    wchar_t *wstrc = song_name;
    size_t lenc = 0;
    size_t len = strlen(argv [1]);
    while(lenc < len)
    {
        size_t conv_res = mbrtowc(wstrc, strc, 1, &mbstate);
        switch(conv_res)
        {
            case 0:
            break;
            default:
            lenc++;
            strc++;
            wstrc++;
            break;
        }
    }
    song_name [lenc] = 0;
    fprintf(stderr, "Converting song %s...\n", argv [1]);
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
        fwrite(buffer, 1, sizeof(buffer), stdout);
    }

    ay_closesong(&song);

}
