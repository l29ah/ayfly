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

#include "common.h"

int fd = 0;

/*int main(int argc, char **argv)
{
    AbstractAudio *player = 0;
    unsigned long i = 0;
    if((SDL_Init(SDL_INIT_AUDIO) == -1))
    {
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }
   
    char *file = 0;
    if(argc < 2)
    {
        printf("Usage: %s <file_to_play>\n", argv [0]);
        goto _exit;
    }
    else
        file = argv [1];

    initSpeccy();

    if(!readFile(file))
        goto _exit;

    player = new SDLAudio(AUDIO_FREQ);

    setPlayer(player);

    if(!player->Start())
        goto _exit;

    SDL_Event event;

    while(1)
    {
        SDL_Delay(50);
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT)
        {
            break;
        }

        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            break;
        }

    }

    player->Stop();
    delete player;

_exit:
    shutdownSpeccy();
    SDL_Quit();
    return 0;
}
*/
