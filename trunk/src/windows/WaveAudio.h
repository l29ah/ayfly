#ifndef WAVEAUDIO_H_
#define WAVEAUDIO_H_

#include "AbstractAudio.h"

class WaveAudio : public AbstractAudio
{
public:
	WaveAudio(unsigned long _sr);
	virtual ~WaveAudio();
	virtual bool Start(void);
	virtual void Stop();
};

#endif /*WAVEAUDIO_H_*/
