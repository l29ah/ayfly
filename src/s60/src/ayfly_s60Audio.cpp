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

#include "s60.h"

#define MAXBUFFERSIZE 16384

#include <f32file.h>
#include <eikenv.h>

Cayfly_s60Audio::Cayfly_s60Audio() :
    AbstractAudio(AUDIO_FREQ), iDevSound(0), iVolume(7)
{
    iCodecType = KMMFFourCCCodePCM16;
    ay8910 = new ay(sr, Z80_FREQ / 2, MAXBUFFERSIZE >> 2); // 16 bit, 2 ch.
    iSoundData = new TUint8[MAXBUFFERSIZE];
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
    KillSound();

    iDevSound = CMMFDevSound::NewL();
    iDevSound->SetVolume(iVolume);

    TMMFState aMode = EMMFStatePlaying;

    TRAPD(err, iDevSound->InitializeL(*this, iCodecType, aMode));
    if(err)
    {
        TBuf<10> errBuf;
        errBuf.AppendNum(err);
        CEikonEnv::Static()->InfoWinL(_L("Play error"), errBuf);
        KillSound();
        return;
    }
    iPrioritySettings.iPref = EMdaPriorityPreferenceTime;
    iPrioritySettings.iPriority = EMdaPriorityNormal;
    iDevSound->SetPrioritySettings(iPrioritySettings);

    // set sample rate and channels
    TMMFCapabilities conf;
    conf = iDevSound->Config();

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

    }
    conf.iChannels = EMMFStereo;
    iDevSound->SetConfigL(conf);

    iDevSound->PlayInitL();
}

void Cayfly_s60Audio::StopPlay()
{
    KillSound();
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
    TDes8& bufData = ((CMMFDataBuffer*)aBuffer)->Data();
    ay8910->ayProcess((unsigned char *)iSoundData, reqSize);
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
    if(aError == KErrNone)
    {
        // priority and preference settings
        /*iPrioritySettings.iPref = EMdaPriorityPreferenceQuality;
         iPrioritySettings.iPriority = EMdaPriorityNormal;
         iDevSound->SetPrioritySettings(iPrioritySettings);

         // set sample rate and channels
         TMMFCapabilities conf;
         conf = iDevSound->Config();
         conf.iRate = EMMFSampleRate32000Hz;
         conf.iChannels = EMMFMono;
         iDevSound->SetConfigL(conf);*/
    }

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
