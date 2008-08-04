#ifndef __SYMBIAN32__
#pragma pack(push, 1)
#endif
struct PSC_File
{
    signed char PSC_MusicName[69];
    unsigned short PSC_UnknownPointer;
    unsigned short PSC_PatternsPointer;
    unsigned char PSC_Delay;
    unsigned short PSC_OrnamentsPointer;
    unsigned short PSC_SamplesPointers[32];
};
#ifndef __SYMBIAN32__
#pragma pack(pop)
#endif

struct PSC_Channel_Parameters
{
    unsigned short Address_In_Pattern, OrnamentPointer, SamplePointer, Ton;
    short Current_Ton_Sliding, Ton_Accumulator, Addition_To_Ton;
    char Initial_Volume, Note_Skip_Counter;
    unsigned char Note, Volume, Amplitude, Volume_Counter, Volume_Counter1, Volume_Counter_Init, Noise_Accumulator, Position_In_Sample, Loop_Sample_Position, Position_In_Ornament, Loop_Ornament_Position;
    bool Enabled, Ornament_Enabled, Envelope_Enabled, Gliss, Ton_Slide_Enabled, Break_Sample_Loop, Break_Ornament_Loop, Volume_Inc;
};

struct PSC_Parameters
{
    unsigned char Delay, DelayCounter, Lines_Counter, Noise_Base;
    unsigned short Positions_Pointer;
};

PSC_Parameters PSC;
PSC_Channel_Parameters PSC_A, PSC_B, PSC_C;

void PSC_Init(unsigned char *module)
{
    PSC_File *header = (PSC_File *)module;
    PSC.DelayCounter = 1;
    PSC.Delay = header->PSC_Delay;
    PSC.Positions_Pointer = header->PSC_PatternsPointer;
    PSC.Lines_Counter = 1;
    PSC.Noise_Base = 0;

    PSC_A.SamplePointer = header->PSC_SamplesPointers[0] + 0x4c;
    PSC_B.SamplePointer = PSC_A.SamplePointer;
    PSC_C.SamplePointer = PSC_A.SamplePointer;
    PSC_A.OrnamentPointer = (*(unsigned short *)&module[header->PSC_OrnamentsPointer]) + header->PSC_OrnamentsPointer;
    PSC_B.OrnamentPointer = PSC_A.OrnamentPointer;
    PSC_C.OrnamentPointer = PSC_A.OrnamentPointer;

    PSC_A.Break_Ornament_Loop = false;
    PSC_A.Ornament_Enabled = false;
    PSC_A.Enabled = false;
    PSC_A.Break_Sample_Loop = false;
    PSC_A.Ton_Slide_Enabled = false;
    PSC_A.Note_Skip_Counter = 1;
    PSC_A.Ton = 0;

    PSC_B.Break_Ornament_Loop = false;
    PSC_B.Ornament_Enabled = false;
    PSC_B.Enabled = false;
    PSC_B.Break_Sample_Loop = false;
    PSC_B.Ton_Slide_Enabled = false;
    PSC_B.Note_Skip_Counter = 1;
    PSC_B.Ton = 0;

    PSC_C.Break_Ornament_Loop = false;
    PSC_C.Ornament_Enabled = false;
    PSC_C.Enabled = false;
    PSC_C.Break_Sample_Loop = false;
    PSC_C.Ton_Slide_Enabled = false;
    PSC_C.Note_Skip_Counter = 1;
    PSC_C.Ton = 0;

    player->ResetAy();

}

void PSC_PatternInterpreter(unsigned char *module, PSC_Channel_Parameters &chan)
{
    PSC_File *header = (PSC_File *)module;
    bool quit;
    bool b1b, b2b, b3b, b4b, b5b, b6b, b7b;
    quit = b1b = b2b = b3b = b4b = b5b = b6b = b7b = false;
    do
    {
        unsigned char val = module[chan.Address_In_Pattern];
        if(val >= 0xc0)
        {
            chan.Note_Skip_Counter = val - 0xbf;
            quit = true;
        }
        else if(val >= 0xa0 && val <= 0xbf)
        {
            chan.OrnamentPointer = (*(unsigned short *)&module[header->PSC_OrnamentsPointer + (val - 0xa0) * 2]) + header->PSC_OrnamentsPointer;
        }
        else if(val >= 0x7e && val <= 0x9f)
        {
            if(val >= 0x80)
                chan.SamplePointer = header->PSC_SamplesPointers[val - 0x80] + 0x4c;
        }
        else if(val == 0x6b)
        {
            chan.Address_In_Pattern++;
            chan.Addition_To_Ton = module[chan.Address_In_Pattern];
            b5b = true;
        }
        else if(val == 0x6c)
        {
            chan.Address_In_Pattern++;
            chan.Addition_To_Ton = -(signed char)(module[chan.Address_In_Pattern]);
            b5b = true;
        }
        else if(val == 0x6d)
        {
            b4b = true;
            chan.Address_In_Pattern++;
            chan.Addition_To_Ton = module[chan.Address_In_Pattern];
        }
        else if(val == 0x6e)
        {
            chan.Address_In_Pattern++;
            PSC.Delay = module[chan.Address_In_Pattern];
        }
        else if(val == 0x6f)
        {
            b1b = true;
            chan.Address_In_Pattern++;
        }
        else if(val == 0x70)
        {
            b3b = true;
            chan.Address_In_Pattern++;
            chan.Volume_Counter1 = module[chan.Address_In_Pattern];
        }
        else if(val == 0x71)
        {
            chan.Break_Ornament_Loop = true;
            chan.Address_In_Pattern++;
        }
        else if(val == 0x7a)
        {
            chan.Address_In_Pattern++;
            if(&chan == &PSC_B)
            {
                player->WriteAy(AY_ENV_SHAPE, module[chan.Address_In_Pattern] & 15);
                player->WriteAy(AY_ENV_FINE, module[chan.Address_In_Pattern + 1]);
                player->WriteAy(AY_ENV_COARSE, module[chan.Address_In_Pattern + 2]);
                chan.Address_In_Pattern += 2;
            }
        }
        else if(val == 0x7b)
        {
            chan.Address_In_Pattern++;
            if(&chan == &PSC_B)
                PSC.Noise_Base = module [chan.Address_In_Pattern];
        }
        else if(val == 0x7c)
        {
            b1b = b3b = b4b = b5b = b6b = b7b = false;
            b2b = true;
        }
        else if(val == 0x7d)
        {
            chan.Break_Sample_Loop = true;
        }
        else if(val >= 0x58 && val <= 0x66)
        {
            chan.Initial_Volume = module[chan.Address_In_Pattern] - 0x57;
            chan.Envelope_Enabled = false;
            b6b = true;
        }
        else if(val == 0x57)
        {
            chan.Initial_Volume = 0xf;
            chan.Envelope_Enabled = true;
            b6b = true;
        }
        else if(val <= 0x56)
        {
            chan.Note = val;
            b6b = true;
            b7b = true;
        }
        else
            chan.Address_In_Pattern++;
        chan.Address_In_Pattern++;
    }
    while(!quit);

    if(b7b)
    {
        chan.Break_Ornament_Loop = false;
        chan.Ornament_Enabled = true;
        chan.Enabled = true;
        chan.Break_Sample_Loop = false;
        chan.Ton_Slide_Enabled = false;
        chan.Ton_Accumulator = 0;
        chan.Current_Ton_Sliding = 0;
        chan.Noise_Accumulator = 0;
        chan.Volume_Counter = 0;
        chan.Position_In_Sample = 0;
        chan.Position_In_Ornament = 0;
    }
    if(b6b)
        chan.Volume = chan.Initial_Volume;
    if(b5b)
    {
        chan.Gliss = false;
        chan.Ton_Slide_Enabled = true;
    }
    if(b4b)
    {
        chan.Current_Ton_Sliding = chan.Ton - ASM_Table[chan.Note];
        chan.Gliss = true;
        if(chan.Current_Ton_Sliding >= 0)
            chan.Addition_To_Ton = -chan.Addition_To_Ton;
        chan.Ton_Slide_Enabled = true;
    }
    if(b3b)
    {
        chan.Volume_Counter = chan.Volume_Counter1;
        chan.Volume_Inc = true;
        if(chan.Volume_Counter & 0x40 != 0)
        {
            chan.Volume_Counter = -(signed char)(chan.Volume_Counter | 128);
            chan.Volume_Inc = false;
        }
        chan.Volume_Counter_Init = chan.Volume_Counter;
    }
    if(b2b)
    {
        chan.Break_Ornament_Loop = false;
        chan.Ornament_Enabled = false;
        chan.Enabled = false;
        chan.Break_Sample_Loop = false;
        chan.Ton_Slide_Enabled = false;
    }
    if(b1b)
        chan.Ornament_Enabled = false;
}

void PSC_GetRegisters(unsigned char *module, PSC_Channel_Parameters &chan, unsigned char &TempMixer)
{
    unsigned char j, b;
    if(chan.Enabled)
    {
        j = chan.Note;
        if(chan.Ornament_Enabled)
        {
            b = module[chan.OrnamentPointer + chan.Position_In_Ornament * 2];
            chan.Noise_Accumulator += b;
            j += module[chan.OrnamentPointer + chan.Position_In_Ornament * 2 + 1];
            if((signed char)j < 0)
                j += 0x56;
            if(j > 0x55)
                j -= 0x56;
            if(j > 0x55)
                j = 0x55;
            if(b & 128 == 0)
                chan.Loop_Ornament_Position = chan.Position_In_Ornament;
            if(b & 64 == 0)
            {
                if(!chan.Break_Ornament_Loop)
                    chan.Position_In_Ornament = chan.Loop_Ornament_Position;
                else
                {
                    chan.Break_Ornament_Loop = false;
                    if(b & 32 == 0)
                        chan.Ornament_Enabled = false;
                    chan.Position_In_Ornament++;
                }
            }
            else
            {
                if(b & 32 == 0)
                    chan.Ornament_Enabled = false;
                chan.Position_In_Ornament++;
            }
        }
        chan.Note = j;
        chan.Ton = *(unsigned short *)&module[chan.SamplePointer + chan.Position_In_Sample * 6];
        chan.Ton_Accumulator += chan.Ton;
        chan.Ton = ASM_Table[j] + chan.Ton_Accumulator;
        if(chan.Ton_Slide_Enabled)
        {
            chan.Current_Ton_Sliding += chan.Addition_To_Ton;
            if(chan.Gliss && (((chan.Current_Ton_Sliding < 0) && (chan.Addition_To_Ton <= 0)) || ((chan.Current_Ton_Sliding >= 0) && (chan.Addition_To_Ton >= 0))))
                chan.Ton_Slide_Enabled = false;
            chan.Ton += chan.Current_Ton_Sliding;
        }
        chan.Ton &= 0xfff;
        b = module[chan.SamplePointer + chan.Position_In_Sample * 6 + 4];
        TempMixer |= (b & 9) << 3;
        j = 0;
        if(b & 2 != 0)
            j++;
        if(b & 4 != 0)
            j--;
        if(chan.Volume_Counter > 0)
        {
            chan.Volume_Counter--;
            if(chan.Volume_Counter == 0)
            {
                if(chan.Volume_Inc)
                    j++;
                else
                    j--;
                chan.Volume_Counter = chan.Volume_Counter_Init;
            }
        }
        chan.Volume += j;
        if((signed char)chan.Volume < 0)
            chan.Volume = 0;
        else if(chan.Volume > 15)
            chan.Volume = 15;
        chan.Amplitude = ((chan.Volume + 1) * (module[chan.SamplePointer + chan.Position_In_Sample * 6 + 3] & 15)) >> 4;
        if(chan.Envelope_Enabled && (b & 16 == 0))
            chan.Amplitude = chan.Amplitude | 16;
        if((chan.Amplitude & 16 != 0) & (b & 8 != 0))
        {
            unsigned short env = player->ReadAy(AY_ENV_FINE) | (player->ReadAy(AY_ENV_COARSE) << 8);
            env += (signed char)(module[chan.SamplePointer + chan.Position_In_Sample * 6 + 2]);
            player->WriteAy(AY_ENV_FINE, env & 0xff);
            player->WriteAy(AY_ENV_COARSE, (env >> 8) & 0xff);
        }
        else
        {
            chan.Noise_Accumulator += module[chan.SamplePointer + chan.Position_In_Sample * 6 + 2];
            if(b & 8 == 0)
                player->WriteAy(AY_NOISE_PERIOD, chan.Noise_Accumulator & 31);
        }
        if(b & 128 == 0)
            chan.Loop_Sample_Position = chan.Position_In_Sample;
        if(b & 64 == 0)
        {
            if(!chan.Break_Sample_Loop)
                chan.Position_In_Sample = chan.Loop_Sample_Position;
            else
            {
                chan.Break_Sample_Loop = false;
                if(b & 32 == 0)
                    chan.Enabled = false;
                chan.Position_In_Sample++;
            }
        }
        else
        {
            if(b & 32 == 0)
                chan.Enabled = false;
            chan.Position_In_Sample++;
        }
    }
    else
        chan.Amplitude = 0;
    TempMixer = TempMixer >> 1;
}

void PSC_Play(unsigned char *module, ELAPSED_CALLBACK callback, void *arg)
{
    PSC_File *header = (PSC_File *)module;
    unsigned char TempMixer;
    if(timeElapsed >= maxElapsed)
    {
        if(callback)
            callback(arg);
    }

    if(--PSC.DelayCounter <= 0)
    {
        if(--PSC.Lines_Counter <= 0)
        {
            if(module[PSC.Positions_Pointer + 1] == 255)
                PSC.Positions_Pointer = *(unsigned short *)&module[PSC.Positions_Pointer + 2];
            PSC.Lines_Counter = module[PSC.Positions_Pointer + 1];
            PSC_A.Address_In_Pattern = *(unsigned short *)&module[PSC.Positions_Pointer + 2];
            PSC_B.Address_In_Pattern = *(unsigned short *)&module[PSC.Positions_Pointer + 4];
            PSC_C.Address_In_Pattern = *(unsigned short *)&module[PSC.Positions_Pointer + 6];
            PSC.Positions_Pointer += 8;
            PSC_A.Note_Skip_Counter = 1;
            PSC_B.Note_Skip_Counter = 1;
            PSC_B.Note_Skip_Counter = 1;
        }
        if(--PSC_A.Note_Skip_Counter == 0)
            PSC_PatternInterpreter(module, PSC_A);
        if(--PSC_B.Note_Skip_Counter == 0)
            PSC_PatternInterpreter(module, PSC_B);
        if(--PSC_C.Note_Skip_Counter == 0)
            PSC_PatternInterpreter(module, PSC_C);
        PSC_A.Noise_Accumulator += PSC.Noise_Base;
        PSC_B.Noise_Accumulator += PSC.Noise_Base;
        PSC_C.Noise_Accumulator += PSC.Noise_Base;
        PSC.DelayCounter = PSC.Delay;
    }
    TempMixer = 0;
    PSC_GetRegisters(module, PSC_A, TempMixer);
    PSC_GetRegisters(module, PSC_B, TempMixer);
    PSC_GetRegisters(module, PSC_C, TempMixer);

    player->WriteAy(AY_MIXER, TempMixer);
    player->WriteAy(AY_CHNL_A_FINE, PSC_A.Ton & 0xff);
    player->WriteAy(AY_CHNL_A_COARSE, (PSC_A.Ton >> 8) & 0xf);
    player->WriteAy(AY_CHNL_B_FINE, PSC_B.Ton & 0xff);
    player->WriteAy(AY_CHNL_B_COARSE, (PSC_B.Ton >> 8) & 0xf);
    player->WriteAy(AY_CHNL_C_FINE, PSC_C.Ton & 0xff);
    player->WriteAy(AY_CHNL_C_COARSE, (PSC_C.Ton >> 8) & 0xf);
    player->WriteAy(AY_CHNL_A_VOL, PSC_A.Amplitude);
    player->WriteAy(AY_CHNL_B_VOL, PSC_B.Amplitude);
    player->WriteAy(AY_CHNL_C_VOL, PSC_C.Amplitude);

    timeElapsed++;
}

void PSC_GetInfo(const unsigned char *module, SongInfo &info)
{
    info.Length = 4 * 60 * 50;
}

