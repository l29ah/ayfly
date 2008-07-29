#ifndef S60AUDIO_H_
#define S60AUDIO_H_

#include "mmf/server/sounddevice.h"
#include "mmf/common/mmfutilities.h"

class Cayfly_s60Audio : public AbstractAudio, MDevSoundObserver
{
public:
    Cayfly_s60Audio();
    virtual ~Cayfly_s60Audio();

    void StartPlay();
    void StopPlay();

    void SetVolume(TInt aVolume);
    TInt GetVolume();

    bool Start();
	void Stop();

private:
    void KillSound();

    // from MDevSoundObserver
    void BufferToBeFilled(CMMFBuffer *aBuffer);
    void InitializeComplete(TInt aError);
    void ToneFinished(TInt aError);
    void PlayError(TInt aError);
    void BufferToBeEmptied(CMMFBuffer *aBuffer);
    void RecordError(TInt aError);
    void ConvertError(TInt aError);
    void DeviceMessage(TUid aMessageType, const TDesC8 &aMsg);

    void DisplayError(const TDesC& aTitle, TInt aError);

private:
    TMMFPrioritySettings  iPrioritySettings;
    CMMFDevSound* iDevSound;

    TFourCC   iCodecType;
    TInt      iVolume;
    TUint8* iSoundData; // sound buffer
};

#endif /*S60AUDIO_H_*/
