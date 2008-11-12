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

void usage()
{
    fwprintf(stderr,L"\tusage: ayfly_converter [-s <metafile>] [ -r <sample_rate>]<input_file> <output_file>\n" );
    fwprintf(stderr, L"\t       if <input_file> = - then stdin is used.\n");
    fwprintf(stderr, L"\t       if <output_file> = - then stdout is used.\n");
    fwprintf(stderr, L"\t       default sample rate = 44100 Hz\n");

}

int main(int argc, char **argv)
{
    fwprintf(stderr,L"AY 891x file converter v. " AYFLY_VERSION_TEXT L", ");
    fwprintf(stderr, L"Deryabin Andrew, 2008. GNU GPL v2 license.\n");
    if(argc < 3)
    {
        usage();
        exit(1);
    }

    end = false;
    void *song = 0;

    FILE *fout = 0;
    FILE *metafile = 0;
    bool is_stdout = false;
    unsigned long sample_rate = 44100;

    int k;
    for(k = 1; k < argc; k++)
    {
        if(!strcmp(argv [k], "-s"))
        {
            if(k + 1 >= argc)
            {
                usage();
                exit(1);
            }
            metafile = fopen(argv [k + 1], "wb");
            k++;
        }
        else if(!strcmp(argv [k], "-r"))
        {
            if(k + 1 >= argc)
            {
                usage();
                exit(1);
            }
            sample_rate = atol(argv [k + 1]);
            k++;
        }
        else
            break;
    }
    
    if(k + 1 >= argc)
    {
        usage();
        exit(1);
    }

    if((strlen(argv [k + 1]) == 1) && (*argv [k + 1] == '-'))
    {
        fout = stdout;
        is_stdout = true;
    }
    else
    {
        fout = fopen(argv [k + 1], "wb");
    }
    
    if(fout == 0)
    {
        fwprintf(stderr, L"Can't open output file!\n");
        exit(1);
    }

    if((strlen(argv [k]) == 1) && (*argv [k] == '-'))
    {
        std::vector<unsigned char> module_vect;
        std::vector<unsigned char>::iterator iter;
        unsigned char buffer [8192];
        size_t rd = 0;
        while((rd = fread(buffer, 1, sizeof(buffer), stdin))> 0)
        {
            for(unsigned long i = 0; i < rd; i++)
            {
                module_vect.push_back(buffer [i]);
            }
        }
        unsigned long size = module_vect.size();
        if(size> 0)
        {
            unsigned char *module = new unsigned char [size];
            unsigned long i = 0;
            for (iter = module_vect.begin(); iter != module_vect.end(); iter++ )
            {
                module [i++] = *iter;
            }
            song = ay_initsongindirect(module, sample_rate, size);

            delete [] module;
        }
    }
    else
    {
        CayflyString song_name(argv [k]);
        unsigned char buffer [65536];
        memset(buffer, 0, sizeof(buffer));
        song = ay_initsong(song_name.c_str(), sample_rate);
    }
    if(!song)
    {
        fwprintf(stderr, L"Can't open song!\n");
        exit(1);
    }

    if(metafile)
    {
        CayflyString str;
        char mb_str [512];
        memset(mb_str, 0, sizeof(mb_str));
        str = ay_getsongname(song);
        wcstombs(mb_str, str.c_str(), sizeof(mb_str) - 1);
        fwrite("Name: ", 1, 6, metafile);
        fwrite(mb_str, 1, strlen(mb_str), metafile);
        fwrite("\n", 1, 1, metafile);
        memset(mb_str, 0, sizeof(mb_str));
        str = ay_getsongauthor(song);
        wcstombs(mb_str, str.c_str(), sizeof(mb_str) - 1);
        fwrite("Author: ", 1, 8, metafile);
        fwrite(mb_str, 1, strlen(mb_str), metafile);
        fwrite("\n", 1, 1, metafile);
        fclose(metafile);

    }

    ay_setelapsedcallback(song, elapsed_callback, 0);
    unsigned char buffer [8192];

    while(!end)
    {
        unsigned long written = ay_rendersongbuffer(song, buffer, sizeof(buffer));
        fwrite(buffer, 1, written, fout);
    }

    ay_closesong(&song);
    if(!is_stdout)
    fclose(fout);
    return 0;
}
