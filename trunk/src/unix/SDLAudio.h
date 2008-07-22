#ifndef SDLSOUND_H_
#define SDLSOUND_H_

class AbstractAudio;

class SDLAudio : public AbstractAudio
{
public:
	SDLAudio(unsigned long _sr);
	virtual ~SDLAudio();
	bool Start();
	virtual void Stop();
private:
	static void Play(void *udata, Uint8 *stream, int len);
	SDL_AudioSpec fmt_out;
};

#endif /*SDLSOUND_H_*/
