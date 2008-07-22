#include "common.h"

AbstractAudio::AbstractAudio(unsigned long _sr)
{
	sr = _sr;
	started = false;
	ay8910 = 0;
}

AbstractAudio::~AbstractAudio()
{

}

void AbstractAudio::SetCallback(ELAPSED_CALLBACK _callback, void *_arg)
{
    if(ay8910)
        ay8910->SetCallback(_callback, _arg);
}
