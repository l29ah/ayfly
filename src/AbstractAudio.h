#include "ay.h"

#ifndef AUDIO_H_
#define AUDIO_H_

class ay;

class AbstractAudio
{
public:
    AbstractAudio(unsigned long _sr);
    virtual ~AbstractAudio();
    virtual bool Start(void) = 0;
    virtual void Stop() = 0;
    virtual void WriteAy(unsigned char reg, unsigned char val)
    {
        ay8910->ayWrite(reg, val);
    }
    ;
    inline virtual bool ChnlToggle(unsigned long chnl)
    {
        return ay8910 ? ay8910->chnlToggle(chnl) : true;
    }
    ;
    inline virtual bool ChnlMuted(unsigned long chnl)
    {
        return ay8910 ? ay8910->chnlMuted(chnl) : true;
    }
    ;
    bool Started()
    {
        return started;
    }
    ;
    inline double GetVolume(unsigned long chnl)
    {
        return ay8910 ? ay8910->GetVolume(chnl) : 0;
    }
    ;
    inline void SetVolume(unsigned long chnl, double new_volume)
    {
        if (ay8910)
        {
            ay8910->SetVolume(chnl, new_volume);
        }
    }
    ;
    virtual void SetCallback(ELAPSED_CALLBACK _callback, void *_arg);
protected:
    unsigned long sr;
    ay *ay8910;
    bool started;
};

#endif /*AUDIO_H_*/
