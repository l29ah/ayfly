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

#include "common.h"

#define BUFFERSIZE 16384

Cayfly_s60Audio::Cayfly_s60Audio(unsigned long _sr)
        : AbstractAudio(_sr)
{
    bOpened = false;
    sem.CreateLocal(0);
    //console->Write(_L("S60Audio::S60Audio\n"));
}

Cayfly_s60Audio::~Cayfly_s60Audio()
{
    iThread.Kill(KErrCancel);
    iThread.Close();
    if (ay8910)
    {
        delete ay8910;
        ay8910 = 0;
    }
    bOpened = false;
    shutdownSpeccy();
}

Cayfly_s60Audio* Cayfly_s60Audio::NewL(unsigned long _sr)
{
    Cayfly_s60Audio* self = new(ELeave) Cayfly_s60Audio(_sr);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void Cayfly_s60Audio::ConstructL()
{
    //console->Write(_L("S60Audio::ConstructL\n"));
    iSoundData [0] = new TUint8[BUFFERSIZE];
    iSoundBuf [0] = new TPtr8(iSoundData [0], BUFFERSIZE, BUFFERSIZE);
    iSoundData [1] = new TUint8[BUFFERSIZE];
    iSoundBuf [1] = new TPtr8(iSoundData [1], BUFFERSIZE, BUFFERSIZE);
    iStream = CMdaAudioOutputStream::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
    ay8910 = new ay(Z80_FREQ / 2, BUFFERSIZE >> 1); // 16 bit, 2 ch.
    buffer_num = 0;
    TInt res = iThread.Create(_L("GenAudioThread"), ThreadEntryPoint,
                              KDefaultStackSize, NULL, (TAny *)this);

    if (res == KErrNone)
    {
        iThread.SetPriority(EPriorityMore);
        iThread.Resume();
    }
}

void Cayfly_s60Audio::Open()
{
    //printf("S60Audio::Open\n");
    iStream->Open(&iSettings);

}

void Cayfly_s60Audio::MaoscOpenComplete(TInt aError)
{
    //printf("S60Audio::MaoscOpenComplete\n");

    if (aError == KErrNone)
    {
        //printf("!!\n");
        // set stream properties to 16bit,44.1KHz stereo
        iStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate32000Hz, TMdaAudioDataSettings::EChannelsMono);

        // note that MaxVolume() is different in the emulator and the real device!
        iStream->SetVolume(5);
        //iStream->SetPriority(EMdaPriorityNomral, EMdaPriorityPreferenceTimeAndQuality);

        bOpened = true;
        // Fill first buffer and write it to the stream
        UpdateBuffer();
    }
}

void Cayfly_s60Audio::UpdateBuffer()
{
    /*static long a = 0;
    static long b = 1;
    //printf("S60Audio::UpdateBuffer\n");

    for(long i = 0; i < BUFFERSIZE; i += 2)
    {
     unsigned long val = b * 3000;
     iSoundData [i] = (val & 0xff);
     iSoundData [i + 1] = (val & 0xff00) >> 8;

     if(++a > 200)
     {
      a = 0;
      b ^= 1;
     }
    }

    //printf("writing %d bytes, a = %u, b = %u\n", BUFFERSIZE, a, b);*/
    // calculate the contents of the buffer
    //ay8910->ayProcess((unsigned char *)iSoundData, BUFFERSIZE);


    // call WriteL with a descriptor pointing at iSoundData
    iStream->WriteL(*iSoundBuf [buffer_num]);
}

void Cayfly_s60Audio::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
{
    if (aError == KErrNone)
    {
        sem.Signal();
        UpdateBuffer();
    }
}

void Cayfly_s60Audio::MaoscPlayComplete(TInt aError)
{
    // we only want to restart in case of an underflow
    // if aError!=KErrUnderflow the stream probably was stopped manually
    if (aError == KErrUnderflow)
    {
        UpdateBuffer();
    }
}

bool Cayfly_s60Audio::Start()
{
    //printf("S60Audio::Start\n");

    if (!bOpened)
        Open();
    else
        UpdateBuffer();
    return true;

}

void Cayfly_s60Audio::Stop()
{
    iStream->Stop();
}

TInt Cayfly_s60Audio::ThreadEntryPoint(TAny* aParameters)
{
    Cayfly_s60Audio *self = (Cayfly_s60Audio *)aParameters;
    return self->DoGenerate();
}

TInt Cayfly_s60Audio::DoGenerate()
{
    static long a = 0;
    static long b = 1;
    static long c = 200;
    static long cc = -1;
    //printf("S60Audio::UpdateBuffer\n");

    while (1)
    {
        unsigned long genbuf_bum = 1 - buffer_num;
        sem.Wait();
        for (long i = 0; i < BUFFERSIZE; i += 2)
        {
            unsigned long val = b * 3000;
            iSoundData [genbuf_bum] [i] = (val & 0xff);
            iSoundData [genbuf_bum] [i + 1] = (val & 0xff00) >> 8;

            if (++a > c)
            {
                a = 0;
                b ^= 1;
                c += cc;
                if (c <= 100)
                    cc = 1;
                else if (c >= 200)
                    cc = -1;
            }
        }
        //ay8910->ayProcess((unsigned char *)iSoundData [genbuf_bum], BUFFERSIZE);
        buffer_num = 1 - buffer_num;
    }



    //printf("writing %d bytes, a = %u, b = %u\n", BUFFERSIZE, a, b);
    return 0;
}
