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

/*
 * parts of this code are taken from scenetone project.
 */

#include "ayfly.h"


_LIT(KThreadName, "ayflyplaybackthread");

/* preferred order of sample rate selection */
static const TInt sampleRateConversionTable[] =
{ 44100, TMdaAudioDataSettings::ESampleRate44100Hz, 32000, TMdaAudioDataSettings::ESampleRate32000Hz, 22050, TMdaAudioDataSettings::ESampleRate22050Hz, 16000, TMdaAudioDataSettings::ESampleRate16000Hz, 11025, TMdaAudioDataSettings::ESampleRate11025Hz, 48000, TMdaAudioDataSettings::ESampleRate48000Hz, 8000, TMdaAudioDataSettings::ESampleRate8000Hz };

Cayfly_s60Audio* Cayfly_s60Audio::NewL(AYSongInfo *info)
{
    Cayfly_s60Audio* a = new (ELeave) Cayfly_s60Audio(info);
    a->ConstructL();
    return a;
}

Cayfly_s60Audio::Cayfly_s60Audio(AYSongInfo *info) :
    AbstractAudio(AUDIO_FREQ, info), iDesc1(0, 0, 0), iDesc2(0, 0, 0)
{
    iVolume = 7;

}

Cayfly_s60Audio::~Cayfly_s60Audio()
{

    if(ay8910)
    {
        delete ay8910;
        ay8910 = 0;
    }

    delete iBuffer1;
    delete iBuffer2;

}

void Cayfly_s60Audio::MaoscOpenComplete(TInt aError)
{
    if(aError != KErrNone)
    {
        User::Panic(_L("STREAMOPEN FAILED"), aError);
        iState = EStopped;
        return;
    }

    TInt mix_freq = 0;

    /* Try out which sample rates are supported, first check stereo */
    for(TUint i = 0; i < sizeof(sampleRateConversionTable) / sizeof(TInt); i += 2)
    {
        TRAPD(err, iStream->SetAudioPropertiesL(sampleRateConversionTable[i + 1], TMdaAudioDataSettings::EChannelsStereo));
        if(err == KErrNone)
        {
            mix_freq = sampleRateConversionTable[i];
            stereo = true;
            break;
        }
    }

    /* Then mono */
    if(mix_freq == 0)
    {
        for(TUint i = 0; i < sizeof(sampleRateConversionTable) / sizeof(TInt); i += 2)
        {
            TRAPD(err, iStream->SetAudioPropertiesL(sampleRateConversionTable[i + 1], TMdaAudioDataSettings::EChannelsMono));
            if(err == KErrNone)
            {
                mix_freq = sampleRateConversionTable[i];
                stereo = false;
                break;
            }
        }
    }

    iState = EPlaying;

    // Mix 2 buffers ready
    if(stereo)
    {
        ay8910->ayProcess(iBuffer1, MIX_BUFFER_LENGTH);
        ay8910->ayProcess(iBuffer2, MIX_BUFFER_LENGTH);
    }
    else
    {
        ay8910->ayProcessMono(iBuffer1, MIX_BUFFER_LENGTH);
        ay8910->ayProcessMono(iBuffer2, MIX_BUFFER_LENGTH);
    }

    iStream->SetVolume(iVolume);
    iStream->SetBalanceL();

    // Write both buffers
    iStream->WriteL(iDesc1);
    iStream->WriteL(iDesc2);

    iMixStep = MIX_BUFFER_TIMES;
    iIdleActive = EFalse;
}

TInt Cayfly_s60Audio::MixLoop(TAny *t)
{
    Cayfly_s60Audio *s = (Cayfly_s60Audio*)t;
    TInt samplesLeft = ((MIX_BUFFER_TIMES - s->iMixStep) * (MIX_BUFFER_LENGTH / MIX_BUFFER_TIMES));

    /* If we are stopping or already stopped, exit idle loop */
    if(s->State() != Cayfly_s60Audio::EPlaying)
        return EFalse;

    if(s->iStartOnNext)
    {
        /* OK. We are late. Mix the current buffer to the end, write it and start on the next one */
        if(s->iBufferToMix == 0)
        {
            if(s->stereo)
                s->ay8910->ayProcess((unsigned char*)(s->iBuffer1 + ((MIX_BUFFER_LENGTH / MIX_BUFFER_TIMES) * s->iMixStep)), samplesLeft);
            else
                s->ay8910->ayProcessMono((unsigned char*)(s->iBuffer1 + ((MIX_BUFFER_LENGTH / MIX_BUFFER_TIMES) * s->iMixStep)), samplesLeft);
        }
        else
        {
            if(s->stereo)
                s->ay8910->ayProcess((unsigned char*)(s->iBuffer2 + ((MIX_BUFFER_LENGTH / MIX_BUFFER_TIMES) * s->iMixStep)), samplesLeft);
            else
                s->ay8910->ayProcessMono((unsigned char*)(s->iBuffer2 + ((MIX_BUFFER_LENGTH / MIX_BUFFER_TIMES) * s->iMixStep)), samplesLeft);
        }

        /* Initialize mixing on the other buffer */
        s->iMixStep = 0;
        s->iBufferToMix = 1 - s->iBufferToMix;
        s->iStartOnNext = EFalse;

        return ETrue;
    }
    else
    {
        /* Normal mixing step, callback has not been called, select buffer first */
        char *mix_buffer = (char*)s->iBuffer1;

        /* Select buffer */
        if(s->iBufferToMix == 1)
        {
            mix_buffer = (char*)s->iBuffer2;
        }

        if(s->stereo)
            s->ay8910->ayProcess((unsigned char*)(mix_buffer + ((MIX_BUFFER_LENGTH / MIX_BUFFER_TIMES) * s->iMixStep)), MIX_BUFFER_LENGTH / MIX_BUFFER_TIMES);
        else
            s->ay8910->ayProcessMono((unsigned char*)(mix_buffer + ((MIX_BUFFER_LENGTH / MIX_BUFFER_TIMES) * s->iMixStep)), MIX_BUFFER_LENGTH / MIX_BUFFER_TIMES);

        s->iMixStep++;

        if(s->iMixStep == MIX_BUFFER_TIMES)
        {
            /* Buffer is complete, write it and complete the AO */
            if(s->iBufferToMix == 0)
                s->iStream->WriteL(s->iDesc1);
            else
                s->iStream->WriteL(s->iDesc2);

            s->iIdleActive = EFalse;
            return EFalse;
        }
    }

    /* Continue mixing */
    return ETrue;
}

void Cayfly_s60Audio::MaoscBufferCopied(TInt aError, const TDesC8 &aBuffer)
{
    if(aError != KErrNone)
    {
        iState = EStopped;
        return;
    }

    if(iState == EPlaying)
    {
        if(!iIdleActive)
        {
            /* If background mixing is not being done (active object doing the mixing), lets start mixing to next buffer (other is still being copied) */
            iBufferToMix = 0;
            if(aBuffer.Ptr() == iBuffer2)
            {
                iBufferToMix = 1;
            }

            iMixStep = 0;

            /* Trig AO to do the mix in N steps for the next buffer */
            iStartOnNext = EFalse;
            iIdle = CIdle::NewL(CActive::EPriorityIdle);
            iIdle->Start(TCallBack(MixLoop, this));
            iIdleActive = ETrue;
        }
        else
        {
            /* Previous buffer was NOT completed in time.. flag AO to finish the previous one and start on next */
            iStartOnNext = ETrue;
        }
    }
}

void Cayfly_s60Audio::MaoscPlayComplete(TInt aError)
{
    if( aError == KErrCancel )
    {
        iState = EStopped;
    }
}

void Cayfly_s60Audio::SetDeviceVolume(TInt aVolume)
{
    if(aVolume > 50)
        aVolume = 50;
    if(aVolume < 0)
        aVolume = 0;
    iVolume = aVolume;
    if(iStream)
        iStream->SetVolume(iVolume);
}

TInt Cayfly_s60Audio::GetDeviceVolume()
{
    return iVolume;
}

bool Cayfly_s60Audio::Start()
{
    PrivateWaitRequestOK();
    iPlayerThread.RequestComplete(iRequestPtr, AYFLY_COMMAND_START_PLAYBACK);
    started = true;
    return true;
}

void Cayfly_s60Audio::Stop()
{
    PrivateWaitRequestOK();
    iPlayerThread.RequestComplete(iRequestPtr, AYFLY_COMMAND_STOP_PLAYBACK);
    started = false;
}

void Cayfly_s60Audio::PrivateStart()
{
    /* StartL should not be called unless playback is stopped */
    if(iState != EStopped) return;

    delete iStream;

    iState = EStarting;
    iStream = CMdaAudioOutputStream::NewL(*this);
    iStream->Open(&iSettings);
}

void Cayfly_s60Audio::PrivateStop()
{
    if(iState == EPlaying)
    {
        iState = EStopping;
        iStream->Stop();
    }
}

TInt Cayfly_s60Audio::State()
{
    return iState;
}

void Cayfly_s60Audio::PrivateWaitRequestOK()
{
    /* Dummy loop.. but works :-) */
    while((iRequestPtr == NULL) || (*iRequestPtr != KRequestPending))
    {
        User::After(10000);
    }
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

void CCommandHandler::Start(Cayfly_s60Audio *aSound)
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
            iSound->PrivateStart();
            break;
        case AYFLY_COMMAND_STOP_PLAYBACK:
            iSound->PrivateStop();
            break;
        case AYFLY_COMMAND_SET_VOLUME:
            //iSound->PrivateSetVolume();
            break;
        case AYFLY_COMMAND_EXIT:
            iSound->PrivateStop();
            iSound->iKilling = ETrue;
            break;
        case AYFLY_COMMAND_WAIT_KILL:
            if(iSound->State() == Cayfly_s60Audio::EStopped)
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

    CTrapCleanup *ctrap = CTrapCleanup::New();

    CActiveScheduler *scheduler = new CActiveScheduler();
    CActiveScheduler::Install(scheduler);

    a->iHandler = CCommandHandler::NewL();
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
    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Hello 1!"));
    ay8910 = new ay(songinfo); // 16 bit, 2 ch.

    iBuffer1 = new (ELeave) unsigned char[MIX_BUFFER_LENGTH];
    iDesc1.Set(iBuffer1, MIX_BUFFER_LENGTH, MIX_BUFFER_LENGTH);
    iBuffer2 = new (ELeave) unsigned char[MIX_BUFFER_LENGTH];
    iDesc2.Set(iBuffer2, MIX_BUFFER_LENGTH, MIX_BUFFER_LENGTH);

    iStream = NULL;
    iState = EStopped;
    stereo = true;

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

    iPlayerThread.Resume(); /* start the streaming thread */
}
