#include "common.h"

SDLAudio::SDLAudio(unsigned long _sr)
: AbstractAudio(_sr)
{
    SDL_AudioSpec fmt;
    SDL_memset(&fmt, 0, sizeof (SDL_AudioSpec));
    fmt.callback = SDLAudio::Play;
    fmt.channels = 2;
    fmt.format = AUDIO_S16;
    fmt.samples = 1024 * 2 * 2;
    fmt.freq = sr;
    fmt.userdata = this;
    if(SDL_OpenAudio(&fmt, &fmt_out) < 0)
    {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return;
    }

    ay8910 = new ay(Z80_FREQ / 2, fmt_out.size >> 2); // 16 bit, 2 ch.

}

SDLAudio::~SDLAudio()
{
    SDL_CloseAudio();
    if(ay8910)
    {
        delete ay8910;
        ay8910 = 0;
    }
}

bool SDLAudio::Start()
{

    if(!started)
    {
        SDL_PauseAudio(0);
        started = true;
    }
    return started;

}

void SDLAudio::Stop()
{
    if(started)
    {
        SDL_LockAudio();
        started = false;
        SDL_PauseAudio(1);
        SDL_UnlockAudio();
    }

}

void SDLAudio::Play(void *udata, Uint8 *stream, int len)
{
    SDLAudio *audio = (SDLAudio *)udata;
    audio->ay8910->ayProcess(stream, len);
}
