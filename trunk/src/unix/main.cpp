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
