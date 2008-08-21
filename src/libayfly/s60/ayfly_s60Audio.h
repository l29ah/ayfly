#ifndef S60AUDIO_H_
#define S60AUDIO_H_

#include <Mda\Common\Audio.h>
#include <MdaAudioOutputStream.h>
#include <e32std.h>

#define AYFLY_SERVER_STACKSIZE        65536

#define AYFLY_COMMAND_NONE            0
#define AYFLY_COMMAND_START_PLAYBACK  1
#define AYFLY_COMMAND_STOP_PLAYBACK   2
#define AYFLY_COMMAND_SET_VOLUME      3
#define AYFLY_COMMAND_EXIT            4
#define AYFLY_COMMAND_WAIT_KILL       5

#define MIX_BUFFER_TIMES                                     8                // mix in 8 smaller passes
#define MIX_BUFFER_SAMPLES_IN_ONE_STEP                     2048
#define MIX_BUFFER_LENGTH               MIX_BUFFER_SAMPLES_IN_ONE_STEP*MIX_BUFFER_TIMES*4

class Cayfly_s60Audio;

class CCommandHandler: public CActive
{
public:
    static CCommandHandler* NewL();
    ~CCommandHandler();
    void Start(Cayfly_s60Audio *aSound);
    void DoCancel();

    CCommandHandler();
    void RunL();
private:
    Cayfly_s60Audio *iSound;
};

class Cayfly_s60Audio: public AbstractAudio, CBase, MMdaAudioOutputStreamCallback
{
public:
    enum
    {
        EStopped = 0, EStarting, EPlaying, EStopping
    };
public:
    static Cayfly_s60Audio* NewL(AYSongInfo *info);
    virtual ~Cayfly_s60Audio();

    void StartPlay();
    void StopPlay();
    void SetDeviceVolume(TInt aVolume);
    TInt GetDeviceVolume();

    bool Start();
    void Stop();
    TPtr8 iDesc1;
    TPtr8 iDesc2;
    RThread iPlayerThread;
    TInt iBufferToMix;
    CIdle *iIdle;
    TBool iIdleActive;
    TBool iStartOnNext;
    CMdaAudioOutputStream *iStream;
    TMdaAudioDataSettings iSettings;
    TInt iState;
    TInt iVolume;
    CCommandHandler *iHandler;
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

    void ConstructL();
    Cayfly_s60Audio(AYSongInfo *info);

};

#endif /*S60AUDIO_H_*/
