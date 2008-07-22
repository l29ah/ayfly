#include "common.h"

WaveAudio::WaveAudio(unsigned long _sr)
:AbstractAudio(_sr)
{
}

WaveAudio::~WaveAudio()
{
}

bool WaveAudio::Start()
{
	return false;
}

void WaveAudio::Stop()
{
	return;
}
