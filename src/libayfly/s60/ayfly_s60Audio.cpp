/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew                      *
 *   andrew@it-optima.ru                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "ayfly.h"

#include <f32file.h>
#include <eikenv.h>

_LIT(KThreadName,"ayflyplaybackthread");

Cayfly_s60Audio* Cayfly_s60Audio::NewL(AYSongInfo *info)
{
    Cayfly_s60Audio* a = new (ELeave) Cayfly_s60Audio(info);
    a->ConstructL();
    return a;
}

Cayfly_s60Audio::Cayfly_s60Audio(AYSongInfo *info) :
    AbstractAudio(AUDIO_FREQ, info), iVolume(7), iDesc1(0,0,0),
    iDesc2(0,0,0)
{

}

Cayfly_s60Audio::~Cayfly_s60Audio()
{
    KillSound();
    if(ay8910)
    {
        delete ay8910;
        ay8910 = 0;
    }

    if(iSoundData)
    {
        delete iSoundData;
        iSoundData = 0;
    }

}

void Cayfly_s60Audio::StartPlay()
{
    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 1"));
    KillSound();
    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 2"));

    iDevSound = CMMFDevSound::NewL();
    iDevSound->SetVolume(iVolume);
    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 3"));

    TMMFState aMode = EMMFStatePlaying;

    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 4"));
    TRAPD(err, iDevSound->InitializeL(*this, iCodecType, aMode));
    if(err)
    {
        TBuf<10> errBuf;
        errBuf.AppendNum(err);
        CEikonEnv::Static()->InfoWinL(_L("Play error"), errBuf);
        KillSound();
        return;
    }
    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 55"));

    // set sample rate and channels
    TMMFCapabilities conf;
    conf = iDevSound->Config();

    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 23"));

    switch(sr)
    {
        case 8000:
            conf.iRate = EMMFSampleRate8000Hz;
            break;
        case 11025:
            conf.iRate = EMMFSampleRate11025Hz;
            break;
        case 22050:
            conf.iRate = EMMFSampleRate22050Hz;
            break;
        case 32000:
            conf.iRate = EMMFSampleRate32000Hz;
            break;
        case 44100:
            conf.iRate = EMMFSampleRate44100Hz;
            break;
        case 48000:
            conf.iRate = EMMFSampleRate48000Hz;
            break;
        case 96000:
            conf.iRate = EMMFSampleRate96000Hz;
            break;
        default:
            break;
    }

    conf.iChannels = EMMFStereo;
    iDevSound->SetConfigL(conf);

    iDevSound->PlayInitL();
    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 10"));
    started = true;
}

void Cayfly_s60Audio::StopPlay()
{
    KillSound();
    started = false;
}

void Cayfly_s60Audio::SetDeviceVolume(TInt aVolume)
{
    if(aVolume > 50)
        aVolume = 50;
    if(aVolume < 0)
        aVolume = 0;
    iVolume = aVolume;
    if(iDevSound)
        iDevSound->SetVolume(iVolume);
}

TInt Cayfly_s60Audio::GetDeviceVolume()
{
    return iVolume;
}

void Cayfly_s60Audio::KillSound()
{
    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 11"));
    if(iDevSound)
    {
        iDevSound->Stop();
        delete iDevSound;
        iDevSound = NULL;
    }
}

void Cayfly_s60Audio::BufferToBeFilled(CMMFBuffer*aBuffer)
{
    int reqSize = aBuffer->RequestSize();
    TDes8& bufData = ((CMMFDataBuffer*) aBuffer)->Data();
    ay8910->ayProcess((unsigned char *) iSoundData, reqSize);
    bufData.Copy(iSoundData, reqSize);
    //bufData.FillZ();


    /*for(unsigned long i = 0 ; i < reqSize; i++)
     {
     bufData [i] = gen_buffer [i];
     }*/
    iDevSound->PlayData();
}

void Cayfly_s60Audio::InitializeComplete(TInt aError)
{
    /*CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 12"));
     if(aError == KErrNone)
     {
     CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 13"));


     CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 22"));

     // set sample rate and channels
     TMMFCapabilities conf;
     conf = iDevSound->Config();

     CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 23"));

     switch(sr)
     {
     case 8000:
     conf.iRate = EMMFSampleRate8000Hz;
     break;
     case 11025:
     conf.iRate = EMMFSampleRate11025Hz;
     break;
     case 22050:
     conf.iRate = EMMFSampleRate22050Hz;
     break;
     case 32000:
     conf.iRate = EMMFSampleRate32000Hz;
     break;
     case 44100:
     conf.iRate = EMMFSampleRate44100Hz;
     break;
     case 48000:
     conf.iRate = EMMFSampleRate48000Hz;
     break;
     case 96000:
     conf.iRate = EMMFSampleRate96000Hz;
     break;
     default:
     break;
     }

     conf.iChannels = EMMFStereo;
     iDevSound->SetConfigL(conf);
     }*/

}

void Cayfly_s60Audio::ToneFinished(TInt aError)
{
    DisplayError(_L("ToneFinished"), aError);
}

void Cayfly_s60Audio::PlayError(TInt aError)
{
    if(aError == KErrUnderflow)
    {
        iDevSound->Stop();
        User::InfoPrint(_L("Play Finished"));
        return;
    }
    DisplayError(_L("PlayError"), aError);
}

// CMMFDevSound object calls this function when the buffer,
// aBuffer gets filled while recording or converting.
void Cayfly_s60Audio::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
{
}

void Cayfly_s60Audio::RecordError(TInt aError)
{
    DisplayError(_L("RecordError"), aError);
}

void Cayfly_s60Audio::ConvertError(TInt aError)
{
    DisplayError(_L("ConvertError"), aError);
}

void Cayfly_s60Audio::DeviceMessage(TUid aMessageType, const TDesC8& aMsg)
{
    HBufC* info;
    HBufC* tmpBuf = HBufC::NewL(20);
    tmpBuf->Des().AppendNum(aMessageType.iUid);
    info = HBufC::NewL(tmpBuf->Length());
    info->Des().Append(*tmpBuf);
    delete tmpBuf;
    tmpBuf = HBufC::NewL(aMsg.Length() + 1);
    tmpBuf->Des().Copy(aMsg);
    tmpBuf->Des().Insert(0, _L(" "));
    info->ReAlloc(info->Length() + tmpBuf->Length());
    info->Des().Append(*tmpBuf);
    delete tmpBuf;
    CEikonEnv::InfoWinL(_L("DeviceMessage"), *info);
    delete info;
}

void Cayfly_s60Audio::DisplayError(const TDesC& aTitle, TInt aError)
{
    if(aError == KErrNone)
        return;

    _LIT(KErrMsgCode, "error: %d");
    TBuf<40> errBuf;
    errBuf.Format(KErrMsgCode, aError);
    CEikonEnv::InfoWinL(aTitle, errBuf);
}

bool Cayfly_s60Audio::Start()
{
    StartPlay();
    return true;
}

void Cayfly_s60Audio::Stop()
{
    StopPlay();
}

CCommandHandler::~CCommandHandler()
{
}

void CCommandHandler::DoCancel()
{
}


CCommandHandler::CCommandHandler() :
    CActive(CActive::EPriorityIdle)
{
}

CCommandHandler* CCommandHandler::NewL()
{
    CCommandHandler *a = new (ELeave) CCommandHandler;
    return a;
}

void CCommandHandler::Start(CScenetoneSound *aSound)
{
    iSound = aSound;
    iSound->iRequestPtr = &iStatus;

    iStatus = KRequestPending;
    SetActive();

    iSound->iKilling = EFalse;
}

void CCommandHandler::RunL(void)
{
    /* We got triggered, check command */
    switch(iStatus.Int())
    {
        case AYFLY_COMMAND_START_PLAYBACK:
            // assumes: filename is ok
            iSound->PrivateStart();
            break;
        case AYFLY_COMMAND_STOP_PLAYBACK:
            iSound->PrivateStop();
            break;
        case AYFLY_COMMAND_SET_VOLUME:
            iSound->PrivateSetVolume();
            break;
        case AYFLY_COMMAND_EXIT:
            iSound->PrivateStop();
            iSound->iKilling = ETrue;
            break;
        case AYFLY_COMMAND_WAIT_KILL:
            if(iSound->State() == CScenetoneSound::EStopped)
            {
                Deque();
                CActiveScheduler::Stop();

                delete iSound->iStream;
                iSound->iStream = NULL;

                return;
            }
        default:
            break;
    }

    iSound->iRequestPtr = &iStatus;
    iStatus = KRequestPending;
    SetActive();

    if(iSound->iKilling)
    {
        /* wait a bit, then loop the AO again */
        User::After(10000);
        User::RequestComplete(iSound->iRequestPtr, AYFLY_COMMAND_WAIT_KILL);
    }
}

TInt serverthreadfunction(TAny *aThis)
{
    Cayfly_s60Audio *a = (Cayfly_s60Audio*)aThis;

    /* We will be using LIBC (possibly) from multiple threads.. -> use Multi-Thread mode of ESTLIB */
//  SpawnPosixServerThread();

    CTrapCleanup *ctrap = CTrapCleanup::New();

    CActiveScheduler *scheduler = new CActiveScheduler();
    CActiveScheduler::Install(scheduler);

    a->iHandler          = CCommandHandler::NewL();
    CActiveScheduler::Add(a->iHandler);

    a->iHandler->Start(a);
    CActiveScheduler::Start();

    // Delete objects created in this thread
    delete a->iHandler;
    delete scheduler;
    delete ctrap;

    return 0;
}

void Cayfly_s60Audio::ConstructL()
{
    iVolume = 7;
    ay8910 = new ay(info, MAXBUFFERSIZE >> 2); // 16 bit, 2 ch.

    iBuffer1 = new (ELeave) unsigned char [MIX_BUFFER_LENGTH];
    iDesc1.Set(iBuffer1, MIX_BUFFER_LENGTH, MIX_BUFFER_LENGTH);
    iBuffer2 = new (ELeave) unsigned char [MIX_BUFFER_LENGTH];
    iDesc2.Set(iBuffer2, MIX_BUFFER_LENGTH, MIX_BUFFER_LENGTH);

    iStream = NULL;
    iState  = EStopped;

    /*********************************************************************************
       Priority scheme:

       1) set owning process to high (-> more than foreground)
       2) set UI thread to be Normal
       3) set playback thread to be RealTime
     *********************************************************************************/

    RThread curthread;

    /* Spawn new thread for actual playback and command control, shares the heap with main thread */
    iPlayerThread.Create(KThreadName, serverthreadfunction, AYFLY_SERVER_STACKSIZE, NULL, (TAny*)this);
    iPlayerThread.SetProcessPriority(EPriorityHigh);
    iPlayerThread.SetPriority(EPriorityRealTime);
    curthread.SetPriority(EPriorityLess);

    iPlayerThread.Resume();                    /* start the streaming thread */
}
