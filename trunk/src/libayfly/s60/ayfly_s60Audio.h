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

class Cayfly_s60Audio;

class CCommandHandler: public CActive
{
public:
    IMPORT_C CCommandHandler* NewL();
    IMPORT_C ~CCommandHandler();
    void Start(Cayfly_s60Audio *aSound);
    void DoCancel();
    IMPORT_C CCommandHandler();
    IMPORT_C void RunL();
private:
    Cayfly_s60Audio *iSound;
};

class Cayfly_s60Audio: public AbstractAudio, MMdaAudioOutputStreamCallback
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

    virtual void MaoscOpenComplete(TInt aError);
    virtual void MaoscBufferCopied(TInt aError, const TDesC8 &aBuffer);
    virtual void MaoscPlayComplete(TInt aError);

    bool Start();
    void Stop();

    void PrivateWaitRequestOK();
    void PrivateStart();
    void PrivateStop();
    TInt State();
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
    Cayfly_s60Audio(AYSongInfo *info);
    AYSongInfo *songinfo;
    static TInt MixLoop(TAny *t);
};

#endif /*S60AUDIO_H_*/
