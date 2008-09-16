#ifndef S60AUDIO_H_
#define S60AUDIO_H_

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

class Cayfly_s60Sound;

class CCommandHandler: public CActive
{
public:
    IMPORT_C static CCommandHandler* NewL();
    IMPORT_C ~CCommandHandler();
    void Start(Cayfly_s60Sound *aSound);
    void DoCancel();
    IMPORT_C CCommandHandler();
    IMPORT_C void RunL();
private:
    Cayfly_s60Sound *iSound;
};

class Cayfly_s60Sound: public CBase, MMdaAudioOutputStreamCallback
{
public:
    enum
    {
        EStopped = 0, EStarting, EPlaying, EStopping
    };
public:
    static Cayfly_s60Sound* NewL();
    virtual ~Cayfly_s60Sound();

    void StartPlay();
    void StopPlay();
    void SetDeviceVolume(TInt aVolume);
    TInt GetDeviceVolume();

    virtual void MaoscOpenComplete(TInt aError);
    virtual void MaoscBufferCopied(TInt aError, const TDesC8 &aBuffer);
    virtual void MaoscPlayComplete(TInt aError);

    bool StartL();
    void StopL();

    void PrivateWaitRequestOK();
    void PrivateStart();
    void PrivateStop();
    void PrivateSetVolume();
    TInt State();
    void Exit();
    void SetSongInfo(AYSongInfo *info);
    unsigned char *iBuffer1;
    unsigned char *iBuffer2;
    TPtr8 iDesc1;
    TPtr8 iDesc2;
    RThread iPlayerThread;
    TRequestStatus *iRequestPtr;
    TBool iKilling;
    TInt iBufferToMix;
    TInt iMixStep;
    CIdle *iIdle;
    TBool iIdleActive;
    TBool iStartOnNext;
    CMdaAudioOutputStream *iStream;
    TMdaAudioDataSettings iSettings;
    TInt iState;
    TInt iVolume;
    CCommandHandler *iHandler;
    bool stereo;
private:
    void ConstructL();
    Cayfly_s60Sound();
    AYSongInfo *songinfo;
    static TInt MixLoop(TAny *t);
    CPeriodic *iPeriodic;
    static TInt StopTCallback(TAny *aPtr);
};

//control class
class Cayfly_s60Audio : public AbstractAudio
{
public:
    Cayfly_s60Audio(AYSongInfo *info);
    Cayfly_s60Audio();
    virtual ~Cayfly_s60Audio();
    virtual bool Start();
    virtual void Stop();
    void SetDeviceVolume(TInt aVolume);
    TInt GetDeviceVolume();
    bool Started() {return sound->State() == Cayfly_s60Sound::EPlaying ? true : false;};
    void SetSongInfo(AYSongInfo *info);
private:
    Cayfly_s60Sound *sound;
};

#endif /*S60AUDIO_H_*/
