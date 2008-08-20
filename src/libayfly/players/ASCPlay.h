/* This player module was ported from:
 AY-3-8910/12 Emulator
 Version 3.0 for Windows 95
 Author Sergey Vladimirovich Bulba
 (c)1999-2004 S.V.Bulba
 */
unsigned short ASM_Table[] =
{ 0xedc, 0xe07, 0xd3e, 0xc80, 0xbcc, 0xb22, 0xa82, 0x9ec, 0x95c, 0x8d6, 0x858, 0x7e0, 0x76e, 0x704, 0x69f, 0x640, 0x5e6, 0x591, 0x541, 0x4f6, 0x4ae, 0x46b, 0x42c, 0x3f0, 0x3b7, 0x382, 0x34f, 0x320, 0x2f3, 0x2c8, 0x2a1, 0x27b, 0x257, 0x236, 0x216, 0x1f8, 0x1dc, 0x1c1, 0x1a8, 0x190, 0x179, 0x164, 0x150, 0x13d, 0x12c, 0x11b, 0x10b, 0xfc, 0xee, 0xe0, 0xd4, 0xc8, 0xbd, 0xb2, 0xa8, 0x9f, 0x96, 0x8d, 0x85, 0x7e, 0x77, 0x70, 0x6a, 0x64, 0x5e, 0x59, 0x54, 0x50, 0x4b, 0x47, 0x43, 0x3f, 0x3c, 0x38, 0x35, 0x32, 0x2f, 0x2d, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c };

#ifndef __SYMBIAN32__
#pragma pack(push, 1)
#endif
struct ASC1_File
{
    unsigned char ASC1_Delay, ASC1_LoopingPosition;
    unsigned short ASC1_PatternsPointers, ASC1_SamplesPointers, ASC1_OrnamentsPointers;
    unsigned char ASC1_Number_Of_Positions;
    unsigned char ASC1_Positions[65535 - 8];
};
#ifndef __SYMBIAN32__
#pragma pack(pop)
#endif

struct ASC_Channel_Parameters
{
    unsigned short Initial_Point_In_Sample, Point_In_Sample, Loop_Point_In_Sample, Initial_Point_In_Ornament, Point_In_Ornament, Loop_Point_In_Ornament, Address_In_Pattern, Ton, Ton_Deviation;
    unsigned char Note, Addition_To_Note, Number_Of_Notes_To_Skip, Initial_Noise, Current_Noise, Volume, Ton_Sliding_Counter, Amplitude, Amplitude_Delay, Amplitude_Delay_Counter;
    short Current_Ton_Sliding, Substruction_for_Ton_Sliding;
    signed char Note_Skip_Counter, Addition_To_Amplitude;
    bool Envelope_Enabled, Sound_Enabled, Sample_Finished, Break_Sample_Loop, Break_Ornament_Loop;
};

struct ASC_Parameters
{
    unsigned char Delay, DelayCounter, CurrentPosition;
};



struct ASC_SongInfo
{
    ASC_Parameters ASC;
    ASC_Channel_Parameters ASC_A, ASC_B, ASC_C;
};

#define ASC_A(x) ((ASC_SongInfo *)x.data)->ASC_A
#define ASC_B(x) ((ASC_SongInfo *)x.data)->ASC_B
#define ASC_C(x) ((ASC_SongInfo *)x.data)->ASC_C
#define ASC(x) ((ASC_SongInfo *)x.data)->ASC

void ASC_Init(AYSongInfo &info)
{
    unsigned char *module = info.file_data;
    ASC1_File *header = (ASC1_File *)module;
    AbstractAudio *player = info.player;
    unsigned short ascPatPt = header->ASC1_PatternsPointers;
    if(info.data)
    {
        delete (ASC_SongInfo *)info.data;
        info.data = 0;
    }
    info.data = (void *)new ASC_SongInfo;
    if(!info.data)
        return;
    memset(&ASC_A(info), 0, sizeof(ASC_Channel_Parameters));
    memset(&ASC_B(info), 0, sizeof(ASC_Channel_Parameters));
    memset(&ASC_C(info), 0, sizeof(ASC_Channel_Parameters));
    ASC(info).CurrentPosition = 0;
    ASC(info).DelayCounter = 1;
    ASC(info).Delay = header->ASC1_Delay;
    ASC_A(info).Address_In_Pattern = (*(unsigned short *)&module[ascPatPt + 6 * module[9]]) + ascPatPt;
    ASC_B(info).Address_In_Pattern = (*(unsigned short *)&module[ascPatPt + 6 * module[9] + 2]) + ascPatPt;
    ASC_C(info).Address_In_Pattern = (*(unsigned short *)&module[ascPatPt + 6 * module[9] + 4]) + ascPatPt;
    player->ResetAy();

}

void ASC_PatternInterpreter(AYSongInfo &info, ASC_Channel_Parameters &chan)
{
    unsigned char *module = info.file_data;
    ASC1_File *header = (ASC1_File *)module;
    AbstractAudio *player = info.player;
    short delta_ton;
    bool Initialization_Of_Ornament_Disabled, Initialization_Of_Sample_Disabled;

    Initialization_Of_Ornament_Disabled = Initialization_Of_Sample_Disabled = false;

    chan.Ton_Sliding_Counter = 0;
    chan.Amplitude_Delay_Counter = 0;
    while(true)
    {
        unsigned char val = module[chan.Address_In_Pattern];
        if(val <= 0x55)
        {
            chan.Note = val;
            chan.Address_In_Pattern++;
            chan.Current_Noise = chan.Initial_Noise;
            if((signed char)(chan.Ton_Sliding_Counter) <= 0)
                chan.Current_Ton_Sliding = 0;
            if(!Initialization_Of_Sample_Disabled)
            {
                chan.Addition_To_Amplitude = 0;
                chan.Ton_Deviation = 0;
                chan.Point_In_Sample = chan.Initial_Point_In_Sample;
                chan.Sound_Enabled = true;
                chan.Sample_Finished = false;
                chan.Break_Sample_Loop = false;
            }
            if(!Initialization_Of_Ornament_Disabled)
            {
                chan.Point_In_Ornament = chan.Initial_Point_In_Ornament;
                chan.Addition_To_Note = 0;
            }
            if(chan.Envelope_Enabled)
            {
                player->WriteAy(AY_ENV_FINE, module[chan.Address_In_Pattern]);
                chan.Address_In_Pattern++;
            }
            break;
        }
        else if((val >= 0x56) && (val <= 0x5d))
        {
            chan.Address_In_Pattern++;
            break;
        }
        else if(val == 0x5e)
        {
            chan.Break_Sample_Loop = true;
            chan.Address_In_Pattern++;
            break;
        }
        else if(val == 0x5f)
        {
            chan.Sound_Enabled = false;
            chan.Address_In_Pattern++;
            break;
        }
        else if((val >= 0x60) && (val <= 0x9f))
        {
            chan.Number_Of_Notes_To_Skip = val - 0x60;
        }
        else if((val >= 0xa0) && (val <= 0xbf))
        {
            chan.Initial_Point_In_Sample = (*(unsigned short *)&module[(module[chan.Address_In_Pattern] - 0xa0) * 2 + header->ASC1_SamplesPointers]) + header->ASC1_SamplesPointers;
        }
        else if((val >= 0xc0) && (val <= 0xdf))
        {
            chan.Initial_Point_In_Ornament = (*(unsigned short *)&module[(module[chan.Address_In_Pattern] - 0xc0) * 2 + header->ASC1_OrnamentsPointers]) + header->ASC1_OrnamentsPointers;
        }
        else if(val == 0xe0)
        {
            chan.Volume = 15;
            chan.Envelope_Enabled = true;
        }
        else if((val >= 0xe1) && (val <= 0xef))
        {
            chan.Volume = val - 0xe0;
            chan.Envelope_Enabled = false;
        }
        else if(val == 0xf0)
        {
            chan.Address_In_Pattern++;
            chan.Initial_Noise = module[chan.Address_In_Pattern];
        }
        else if(val == 0xf1)
        {
            Initialization_Of_Sample_Disabled = true;
        }
        else if(val == 0xf2)
        {
            Initialization_Of_Ornament_Disabled = true;
        }
        else if(val == 0xf3)
        {
            Initialization_Of_Sample_Disabled = true;
            Initialization_Of_Ornament_Disabled = true;
        }
        else if(val == 0xf4)
        {
            chan.Address_In_Pattern++;
            ASC(info).Delay = module[chan.Address_In_Pattern];
        }
        else if(val == 0xf5)
        {
            chan.Address_In_Pattern++;
            chan.Substruction_for_Ton_Sliding = -(signed char)(module[chan.Address_In_Pattern]) * 16;
            chan.Ton_Sliding_Counter = 255;
        }
        else if(val == 0xf6)
        {
            chan.Address_In_Pattern++;
            chan.Substruction_for_Ton_Sliding = (signed char)(module[chan.Address_In_Pattern]) * 16;
            chan.Ton_Sliding_Counter = 255;
        }
        else if(val == 0xf7)
        {
            chan.Address_In_Pattern++;
            Initialization_Of_Sample_Disabled = true;
            if(module[chan.Address_In_Pattern + 1] < 0x56)
                delta_ton = ASM_Table[chan.Note] + (chan.Current_Ton_Sliding / 16) - ASM_Table[module[chan.Address_In_Pattern + 1]];
            else
                delta_ton = chan.Current_Ton_Sliding / 16;
            delta_ton = delta_ton << 4;
            chan.Substruction_for_Ton_Sliding = -delta_ton / (signed char)(module[chan.Address_In_Pattern]);
            chan.Current_Ton_Sliding = delta_ton - delta_ton % (signed char)(module[chan.Address_In_Pattern]);
            chan.Ton_Sliding_Counter = (signed char)(module[chan.Address_In_Pattern]);
        }
        else if(val == 0xf8)
        {
            player->WriteAy(AY_ENV_SHAPE, 8);
        }
        else if(val == 0xf9)
        {
            chan.Address_In_Pattern++;
            if(module[chan.Address_In_Pattern + 1] < 0x56)
            {
                delta_ton = ASM_Table[chan.Note] - ASM_Table[module[chan.Address_In_Pattern + 1]];
            }
            else
                delta_ton = chan.Current_Ton_Sliding / 16;
            delta_ton = delta_ton << 4;
            chan.Substruction_for_Ton_Sliding = -delta_ton / (signed char)(module[chan.Address_In_Pattern]);
            chan.Current_Ton_Sliding = delta_ton - delta_ton % (signed char)(module[chan.Address_In_Pattern]);
            chan.Ton_Sliding_Counter = (signed char)(module[chan.Address_In_Pattern]);

        }
        else if(val == 0xfa)
        {
            player->WriteAy(AY_ENV_SHAPE, 10);
        }
        else if(val == 0xfb)
        {
            chan.Address_In_Pattern++;
            if((module[chan.Address_In_Pattern] & 32) == 0)
            {
                chan.Amplitude_Delay = module[chan.Address_In_Pattern] << 3;
                chan.Amplitude_Delay_Counter = chan.Amplitude_Delay;
            }
            else
            {
                chan.Amplitude_Delay = ((module[chan.Address_In_Pattern] << 3) ^ 0xf8) + 9;
                chan.Amplitude_Delay_Counter = chan.Amplitude_Delay;
            }
        }
        else if(val == 0xfc)
        {
            player->WriteAy(AY_ENV_SHAPE, 12);
        }
        else if(val == 0xfe)
        {
            player->WriteAy(AY_ENV_SHAPE, 14);
        }
        chan.Address_In_Pattern++;
    }
    chan.Note_Skip_Counter = chan.Number_Of_Notes_To_Skip;
}

void ASC_GetRegisters(AYSongInfo &info, ASC_Channel_Parameters &chan, unsigned char &TempMixer)
{
    unsigned char *module = info.file_data;
    AbstractAudio *player = info.player;
    signed char j;
    bool Sample_Says_OK_for_Envelope;
    if(chan.Sample_Finished || !chan.Sound_Enabled)
        chan.Amplitude = 0;
    else
    {
        if(chan.Amplitude_Delay_Counter != 0)
        {
            if(chan.Amplitude_Delay_Counter >= 16)
            {
                chan.Amplitude_Delay_Counter -= 8;
                if(chan.Addition_To_Amplitude < -15)
                    chan.Addition_To_Amplitude++;
                else if(chan.Addition_To_Amplitude > 15)
                    chan.Addition_To_Amplitude--;
            }
            else
            {
                if((chan.Amplitude_Delay_Counter & 1) != 0)
                {
                    if(chan.Addition_To_Amplitude > -15)
                        chan.Addition_To_Amplitude--;
                }
                else if(chan.Addition_To_Amplitude < 15)
                    chan.Addition_To_Amplitude++;
                chan.Amplitude_Delay_Counter = chan.Amplitude_Delay;
            }
        }
        if((module[chan.Point_In_Sample] & 128) != 0)
            chan.Loop_Point_In_Sample = chan.Point_In_Sample;
        if((module[chan.Point_In_Sample] & 96) == 32)
            chan.Sample_Finished = true;
        chan.Ton_Deviation += (signed char)(module[chan.Point_In_Sample + 1]);
        TempMixer |= (module[chan.Point_In_Sample + 2] & 9) << 3;
        if((module[chan.Point_In_Sample + 2] & 6) == 2)
            Sample_Says_OK_for_Envelope = true;
        else
            Sample_Says_OK_for_Envelope = false;
        if((module[chan.Point_In_Sample + 2] & 6) == 4)
        {
            if(chan.Addition_To_Amplitude > -15)
                chan.Addition_To_Amplitude--;
        }
        if((module[chan.Point_In_Sample + 2] & 6) == 6)
        {
            if(chan.Addition_To_Amplitude < 15)
                chan.Addition_To_Amplitude++;
        }
        chan.Amplitude = chan.Addition_To_Amplitude + (module[chan.Point_In_Sample + 2] >> 4);
        if((signed char)(chan.Amplitude) < 0)
            chan.Amplitude = 0;
        else if(chan.Amplitude > 15)
            chan.Amplitude = 15;
        chan.Amplitude = (chan.Amplitude * (chan.Volume + 1)) >> 4;
        if(Sample_Says_OK_for_Envelope && ((TempMixer & 64) != 0))
            player->WriteAy(AY_ENV_FINE, player->ReadAy(AY_ENV_FINE) + ((signed char)(module[chan.Point_In_Sample] << 3) / 8));
        else
            chan.Current_Noise += (signed char)(module[chan.Point_In_Sample] << 3) / 8;
        chan.Point_In_Sample += 3;
        if((module[chan.Point_In_Sample - 3] & 64) != 0)
        {
            if(!chan.Break_Sample_Loop)
                chan.Point_In_Sample = chan.Loop_Point_In_Sample;
            else if((module[chan.Point_In_Sample - 3] & 32) != 0)
                chan.Sample_Finished = true;
        }
        if((module[chan.Point_In_Ornament] & 128) != 0)
            chan.Loop_Point_In_Ornament = chan.Point_In_Ornament;
        chan.Addition_To_Note += module[chan.Point_In_Ornament + 1];
        chan.Current_Noise += (-(signed char)(module[chan.Point_In_Ornament] & 0x10)) | module[chan.Point_In_Ornament];
        chan.Point_In_Ornament += 2;
        if((module[chan.Point_In_Ornament - 2] & 64) != 0)
            chan.Point_In_Ornament = chan.Loop_Point_In_Ornament;
        if((TempMixer & 64) == 0)
            player->WriteAy(AY_NOISE_PERIOD, ((unsigned char)(chan.Current_Ton_Sliding >> 8) + chan.Current_Noise) & 0x1f);
        j = chan.Note + chan.Addition_To_Note;
        if(j < 0)
            j = 0;
        else if(j > 0x55)
            j = 0x55;
        chan.Ton = (ASM_Table[j] + chan.Ton_Deviation + (chan.Current_Ton_Sliding / 16)) & 0xfff;
        if(chan.Ton_Sliding_Counter != 0)
        {
            if((signed char)(chan.Ton_Sliding_Counter) > 0)
                chan.Ton_Sliding_Counter--;
            chan.Current_Ton_Sliding += chan.Substruction_for_Ton_Sliding;
        }
        if(chan.Envelope_Enabled && Sample_Says_OK_for_Envelope)
            chan.Amplitude |= 0x10;
    }
    TempMixer = TempMixer >> 1;
}

void ASC_Play(AYSongInfo &info)
{
    unsigned char *module = info.file_data;
    ASC1_File *header = (ASC1_File *)module;
    AbstractAudio *player = info.player;
    unsigned char TempMixer;

    if(info.timeElapsed >= info.Length)
    {
        info.timeElapsed = info.Loop;
        if(info.callback)
            info.callback(info.callback_arg);
    }

    if(--ASC(info).DelayCounter <= 0)
    {
        if(--ASC_A(info).Note_Skip_Counter < 0)
        {
            if(module[ASC_A(info).Address_In_Pattern] == 255)
            {
                if(++ASC(info).CurrentPosition >= header->ASC1_Number_Of_Positions)
                    ASC(info).CurrentPosition = header->ASC1_LoopingPosition;
                unsigned short ascPatPt = header->ASC1_PatternsPointers;
                ASC_A(info).Address_In_Pattern = (*(unsigned short *)&module[ascPatPt + 6 * module[ASC(info).CurrentPosition + 9]]) + ascPatPt;
                ASC_B(info).Address_In_Pattern = (*(unsigned short *)&module[ascPatPt + 6 * module[ASC(info).CurrentPosition + 9] + 2]) + ascPatPt;
                ASC_C(info).Address_In_Pattern = (*(unsigned short *)&module[ascPatPt + 6 * module[ASC(info).CurrentPosition + 9] + 4]) + ascPatPt;
                ASC_A(info).Initial_Noise = 0;
                ASC_B(info).Initial_Noise = 0;
                ASC_C(info).Initial_Noise = 0;
            }
            ASC_PatternInterpreter(info, ASC_A(info));
        }
        if(--ASC_B(info).Note_Skip_Counter < 0)
        {
            ASC_PatternInterpreter(info, ASC_B(info));
        }
        if(--ASC_C(info).Note_Skip_Counter < 0)
        {
            ASC_PatternInterpreter(info, ASC_C(info));
        }
        ASC(info).DelayCounter = ASC(info).Delay;
    }

    TempMixer = 0;
    ASC_GetRegisters(info, ASC_A(info), TempMixer);
    ASC_GetRegisters(info, ASC_B(info), TempMixer);
    ASC_GetRegisters(info, ASC_C(info), TempMixer);

    player->WriteAy(AY_MIXER, TempMixer);
    player->WriteAy(AY_CHNL_A_FINE, ASC_A(info).Ton & 0xff);
    player->WriteAy(AY_CHNL_A_COARSE, (ASC_A(info).Ton >> 8) & 0xf);
    player->WriteAy(AY_CHNL_B_FINE, ASC_B(info).Ton & 0xff);
    player->WriteAy(AY_CHNL_B_COARSE, (ASC_B(info).Ton >> 8) & 0xf);
    player->WriteAy(AY_CHNL_C_FINE, ASC_C(info).Ton & 0xff);
    player->WriteAy(AY_CHNL_C_COARSE, (ASC_C(info).Ton >> 8) & 0xf);
    player->WriteAy(AY_CHNL_A_VOL, ASC_A(info).Amplitude);
    player->WriteAy(AY_CHNL_B_VOL, ASC_B(info).Amplitude);
    player->WriteAy(AY_CHNL_C_VOL, ASC_C(info).Amplitude);

    info.timeElapsed++;
}

void ASC_GetInfo(AYSongInfo &info)
{
    unsigned char *module = info.file_data;
    ASC1_File *header = (ASC1_File *)module;
    AbstractAudio *player = info.player;
    short a1, a2, a3, a11, a22, a33;
    unsigned long j1, j2, j3;
    bool env1, env2, env3;
    long i, tm = 0;
    unsigned char b;

    unsigned char ascDelay = header->ASC1_Delay;
    unsigned short ascLoopPos = header->ASC1_LoopingPosition;
    unsigned short ascPatPt = header->ASC1_PatternsPointers;
    unsigned char ascNumPos = header->ASC1_Number_Of_Positions;

    b = ascDelay;
    a1 = a2 = a3 = a11 = a22 = a33 = 0;
    env1 = env2 = env3 = false;
    for(i = 0; i < ascNumPos; i++)
    {
        if(ascLoopPos == i)
            info.Loop = tm;
        j1 = (*(unsigned short *)&module[ascPatPt + 6 * module[i + 9]]) + ascPatPt;
        j2 = (*(unsigned short *)&module[ascPatPt + 6 * module[i + 9] + 2]) + ascPatPt;
        j3 = (*(unsigned short *)&module[ascPatPt + 6 * module[i + 9] + 4]) + ascPatPt;

        while(true)
        {
            a1--;
            if(a1 < 0)
            {
                if(module[j1] == 255)
                    break;
                while(true)
                {
                    unsigned char val = module[j1];
                    if(val <= 0x55)
                    {
                        a1 = a11;
                        j1++;
                        if(env1)
                            j1++;
                        break;
                    }
                    else if((val >= 0x56) && (val <= 0x5f))
                    {
                        a1 = a11;
                        j1++;
                        break;
                    }
                    else if((val >= 0x60) && (val <= 0x9f))
                    {
                        a11 = val - 0x60;
                    }
                    else if(val == 0xe0)
                    {
                        env1 = true;
                    }
                    else if((val >= 0xe1) && (val <= 0xef))
                    {
                        env1 = false;
                    }
                    else if((val == 0xf0) || ((val >= 0xf5) && (val <= 0xf7)) || (val == 0xf9) || (val == 0xfb))
                    {
                        j1++;
                    }
                    else if(val == 0xf4)
                    {
                        j1++;
                        b = module[j1];
                    }
                    j1++;
                }
            }

            a2--;
            if(a2 < 0)
            {
                while(true)
                {
                    unsigned char val = module[j2];
                    if(val <= 0x55)
                    {
                        a2 = a22;
                        j2++;
                        if(env2)
                            j2++;
                        break;
                    }
                    else if((val >= 0x56) && (val <= 0x5f))
                    {
                        a2 = a22;
                        j2++;
                        break;
                    }
                    else if((val >= 0x60) && (val <= 0x9f))
                    {
                        a22 = val - 0x60;
                    }
                    else if(val == 0xe0)
                    {
                        env2 = true;
                    }
                    else if((val >= 0xe1) && (val <= 0xef))
                    {
                        env2 = false;
                    }
                    else if((val == 0xf0) || ((val >= 0xf5) && (val <= 0xf7)) || (val == 0xf9) || (val == 0xfb))
                    {
                        j2++;
                    }
                    else if(val == 0xf4)
                    {
                        j2++;
                        b = module[j2];
                    }
                    j2++;
                }
            }

            a3--;
            if(a3 < 0)
            {
                while(true)
                {
                    unsigned char val = (unsigned char)module[j3];
                    if(val <= 0x55)
                    {
                        a3 = a33;
                        j3++;
                        if(env3)
                            j3++;
                        break;
                    }
                    else if((val >= 0x56) && (val <= 0x5f))
                    {
                        a3 = a33;
                        j3++;
                        break;
                    }
                    else if((val >= 0x60) && (val <= 0x9f))
                    {
                        a33 = val - 0x60;
                    }
                    else if(val == 0xe0)
                    {
                        env3 = true;
                    }
                    else if((val >= 0xe1) && (val <= 0xef))
                    {
                        env3 = false;
                    }
                    else if((val == 0xf0) || ((val >= 0xf5) && (val <= 0xf7)) || (val == 0xf9) || (val == 0xfb))
                    {
                        j3++;
                    }
                    else if(val == 0xf4)
                    {
                        j3++;
                        b = module[j3];
                    }
                    j3++;
                }
            }
            tm += b;
        }
    }
    info.Length = tm;
}

void ASC_Cleanup(AYSongInfo &info)
{
    if(info.data)
    {
        delete (ASC_SongInfo *)info.data;
        info.data = 0;
    }
}
