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
#include <vector>

bool end;

bool elapsed_callback(void *)
{
    end = true;
    return end;
}

int main(int argc, char **argv)
{
    fwprintf(stderr,L"AY 891x file converter v. " AYFLY_VERSION_TEXT L", ");
    fwprintf(stderr, L"Deryabin Andrew, 2008. GNU GPL v2 license.\n");
    if(argc < 2)
    {
        fwprintf(stderr, L"\tusage: ayfly_converter ay_file > raw_pcm_file\n");
        exit(1);
    }

    end = false;
    void *song = 0;    
    if((strlen(argv [1]) == 1) && (*argv [1] == '-')) // stdin
    {
        std::vector<unsigned char> module_vect;
        std::vector<unsigned char>::iterator iter;
        unsigned char buffer [8192];
        size_t rd = 0;
        fwprintf(stderr, L"Reading from stdin..\n");
        while((rd = fread(buffer, 1, sizeof(buffer), stdin))> 0)
        {
            for(unsigned long i = 0; i < rd; i++)
            {
                module_vect.push_back(buffer [i]);
            }
        }
        unsigned long size = module_vect.size();
        fwprintf(stderr, L"Read %u bytes..\n", size);
        if(size > 0)
        {                        
            unsigned char *module = new unsigned char [size];
            unsigned long i = 0;
            for (iter = module_vect.begin(); iter != module_vect.end(); iter++ )
            {
                module [i++] = *iter;
            }
            song = ay_initsongindirect(module, 44100, size);

            delete [] module;
        }
    }
    else
    {
        wchar_t song_name [65536];
        mbstate_t mbstate;
        ::memset((void*)&mbstate, 0, sizeof(mbstate));
        fprintf(stderr, "Reading from file..\n");
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
        unsigned char buffer [65536];
        memset(buffer, 0, sizeof(buffer));
        song = ay_initsong(song_name, 44100);
    }
    if(!song)
    {
        fwprintf(stderr, L"Can't open song!\n");
        exit(1);
    }

    ay_setelapsedcallback(song, elapsed_callback, 0);
    unsigned char buffer [8192];

    while(!end)
    {
        unsigned long written = ay_rendersongbuffer(song, buffer, sizeof(buffer));
        fwrite(buffer, 1, written, stdout);
    }

    ay_closesong(&song);

}
