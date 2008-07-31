/* This player module was ported from:
AY-3-8910/12 Emulator
Version 3.0 for Windows 95
Author Sergey Vladimirovich Bulba
(c)1999-2004 S.V.Bulba
*/
unsigned short ASM_Table [] =
{0xedc, 0xe07, 0xd3e, 0xc80, 0xbcc, 0xb22, 0xa82, 0x9ec, 0x95c, 0x8d6, 0x858, 0x7e0, 0x76e, 0x704, 0x69f,
 0x640, 0x5e6, 0x591, 0x541, 0x4f6, 0x4ae, 0x46b, 0x42c, 0x3f0, 0x3b7, 0x382, 0x34f, 0x320, 0x2f3, 0x2c8,
 0x2a1, 0x27b, 0x257, 0x236, 0x216, 0x1f8, 0x1dc, 0x1c1, 0x1a8, 0x190, 0x179, 0x164, 0x150, 0x13d, 0x12c,
 0x11b, 0x10b, 0xfc, 0xee, 0xe0, 0xd4, 0xc8, 0xbd, 0xb2, 0xa8, 0x9f, 0x96, 0x8d, 0x85, 0x7e, 0x77, 0x70, 0x6a,
 0x64, 0x5e, 0x59, 0x54, 0x50, 0x4b, 0x47, 0x43, 0x3f, 0x3c, 0x38, 0x35, 0x32, 0x2f, 0x2d, 0x2a, 0x28, 0x26, 0x24,
 0x22, 0x20, 0x1e, 0x1c};

#pragma pack(push, 0)
struct ASC1_File
{
    unsigned char ASC1_Delay, ASC1_LoopingPosition;
    unsigned short ASC1_PatternsPointers,ASC1_SamplesPointers,ASC1_OrnamentsPointers;
    unsigned char ASC1_Number_Of_Positions;
    unsigned char ASC1_Positions [65535 - 8];
};

struct ASC_Channel_Parameters
{
    unsigned short Initial_Point_In_Sample,
    Point_In_Sample,
    Loop_Point_In_Sample,
    Initial_Point_In_Ornament,
    Point_In_Ornament,
    Loop_Point_In_Ornament,
    Address_In_Pattern,
    Ton,
    Ton_Deviation;
    unsigned char Note,
    Addition_To_Note,
    Number_Of_Notes_To_Skip,
    Initial_Noise,
    Current_Noise,
    Volume,
    Ton_Sliding_Counter,
    Amplitude,
    Amplitude_Delay,
    Amplitude_Delay_Counter;
    int Current_Ton_Sliding,
    Substruction_for_Ton_Sliding;
    short Note_Skip_Counter,
    Addition_To_Amplitude;
    bool Envelope_Enabled,
    Sound_Enabled,
    Sample_Finished,
    Break_Sample_Loop,
    Break_Ornament_Loop;
};

struct ASC_Parameters
{
    unsigned char Delay,
    DelayCounter,
    CurrentPosition;
};
#pragma pack(pop)

ASC_Parameters ASC;
ASC_Channel_Parameters ASC_A, ASC_B, ASC_C;

void ASC_Init(unsigned char *module)
{
    ASC1_File *header = (ASC1_File *)module;
    unsigned short ascPatPt = header->ASC1_PatternsPointers;
    memset(&ASC_A, 0, sizeof(ASC_A));
    memset(&ASC_B, 0, sizeof(ASC_B));
    memset(&ASC_C, 0, sizeof(ASC_C));
    ASC.CurrentPosition = 0;
    ASC.DelayCounter = 1;
    ASC.Delay = header->ASC1_Delay;
    ASC_A.Address_In_Pattern = (*(unsigned short *) &module [ascPatPt + 6 * module[9]]) + ascPatPt;
    ASC_B.Address_In_Pattern = (*(unsigned short *) &module [ascPatPt + 6 * module[9] + 2]) + ascPatPt;
    ASC_C.Address_In_Pattern = (*(unsigned short *) &module [ascPatPt + 6 * module[9] + 4]) + ascPatPt;
    player->ResetAy();
}

void ASC_PatternInterpreter(unsigned char *module, ASC_Channel_Parameters &chan)
{
    ASC1_File *header = (ASC1_File *)module;
    int delta_ton;
    bool Initialization_Of_Ornament_Disabled,
    Initialization_Of_Sample_Disabled;

    Initialization_Of_Ornament_Disabled = Initialization_Of_Sample_Disabled = false;

    chan.Ton_Sliding_Counter = 0;
    chan.Amplitude_Delay_Counter = 0;
    while (true)
    {
        unsigned char val = module [chan.Address_In_Pattern];
        if ((val >= 0) && (val <= 0x55))
        {
            chan.Note = val;
            chan.Address_In_Pattern++;
            chan.Current_Noise = chan.Initial_Noise;
            if((char)chan.Ton_Sliding_Counter <= 0)
                chan.Ton_Sliding_Counter = 0;
            if(!Initialization_Of_Ornament_Disabled)
            {
                chan.Addition_To_Amplitude = 0;
                chan.Ton_Deviation = 0;
                chan.Point_In_Sample = chan.Initial_Point_In_Sample;
                chan.Sound_Enabled = true;
                chan.Sample_Finished = false;
                chan.Break_Sample_Loop = false;
            }
            break;
            if(!Initialization_Of_Ornament_Disabled)
            {
                chan.Point_In_Ornament = chan.Initial_Point_In_Ornament;
                chan.Addition_To_Note = 0;
            }
            if(chan.Envelope_Enabled)
            {
                player->WriteAy(AY_ENV_FINE, module [chan.Address_In_Pattern]);
                chan.Address_In_Pattern++;
            }
            break;
        }
        else if ((val >= 0x56) && (val <= 0x5d))
        {
            chan.Address_In_Pattern++;
            break;
        }
        else if(val == 0x5e)
        {
            chan.Break_Sample_Loop = true;
            chan.Address_In_Pattern++;
        }
        else if(val == 0x5f)
        {
            chan.Sound_Enabled = false;
            chan.Address_In_Pattern++;
        }
        else if ((val >= 0x60) && (val <= 0x9f))
        {
            chan.Number_Of_Notes_To_Skip = module[chan.Address_In_Pattern] - 0x60;
        }
        else if ((val >= 0xa0) && (val <= 0xbf))
        {
            chan.Initial_Point_In_Sample = *(unsigned short *) &module [(module [chan.Address_In_Pattern] - 0xa0) * 2 + header->ASC1_PatternsPointers] + header->ASC1_PatternsPointers;
        }
        else if ((val >= 0xc0) && (val <= 0xdf))
        {
            chan.Initial_Point_In_Ornament = *(unsigned short *) &module [(module [chan.Address_In_Pattern] - 0xc0) * 2 + header->ASC1_OrnamentsPointers] + header->ASC1_OrnamentsPointers;
        }
        else if (val == 0xe0)
        {
            chan.Volume = 15;
            chan.Envelope_Enabled = true;
        }
        else if ((val >= 0xe1) && (val <= 0xef))
        {
            chan.Volume = val - 0xe0;
            chan.Envelope_Enabled = false;
        }
        else if (val == 0xf0)
        {
            chan.Address_In_Pattern++;
            chan.Initial_Noise = module [chan.Address_In_Pattern];
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
        else if (val == 0xf4)
        {
            chan.Address_In_Pattern++;
            ASC.Delay = module [chan.Address_In_Pattern];
        }
        else if (val == 0xf5)
        {
            chan.Address_In_Pattern++;
            chan.Substruction_for_Ton_Sliding = -((short)module [chan.Address_In_Pattern]) * 16;
            chan.Ton_Sliding_Counter = 255;
        }
        else if (val == 0xf6)
        {
            chan.Address_In_Pattern++;
            chan.Substruction_for_Ton_Sliding = ((short)module [chan.Address_In_Pattern]) * 16;
            chan.Ton_Sliding_Counter = 255;
        }
        else if (val == 0xf7)
        {
            chan.Address_In_Pattern++;
            Initialization_Of_Sample_Disabled = true;
            if(module [chan.Address_In_Pattern + 1] < 0x56)
            {
                delta_ton = ASM_Table [chan.Note] + (chan.Current_Ton_Sliding / 16) -
                            ASM_Table [module [chan.Address_In_Pattern + 1]];
            }
            else
            {
                delta_ton = (chan.Current_Ton_Sliding / 16) << 4;
                chan.Substruction_for_Ton_Sliding = -delta_ton / (short)module [chan.Address_In_Pattern];
                chan.Current_Ton_Sliding = delta_ton - (delta_ton % (short)module [chan.Address_In_Pattern]);
                chan.Ton_Sliding_Counter = (short)module [chan.Address_In_Pattern];
            }
        }
        else if (val == 0xf8)
        {
            player->WriteAy(AY_ENV_SHAPE, 8);
        }
        else if (val == 0xf9)
        {
            chan.Address_In_Pattern++;
            if(module [chan.Address_In_Pattern + 1] < 0x56)
            {
                delta_ton = ASM_Table [chan.Note] - ASM_Table [module [chan.Address_In_Pattern + 1]];
            }
            else
            {
                delta_ton = (chan.Current_Ton_Sliding / 16) << 4;
                chan.Substruction_for_Ton_Sliding = -delta_ton / (short)module [chan.Address_In_Pattern];
                chan.Current_Ton_Sliding = delta_ton - (delta_ton % (short)module [chan.Address_In_Pattern]);
                chan.Ton_Sliding_Counter = (short)module [chan.Address_In_Pattern];
            }

        }
        else if (val == 0xfa)
        {
            player->WriteAy(AY_ENV_SHAPE, 10);
        }
        else if (val == 0xfb)
        {
            chan.Address_In_Pattern++;
            if((module [chan.Address_In_Pattern] & 32) == 0)
            {
                chan.Amplitude_Delay = module [chan.Address_In_Pattern] << 3;
                chan.Amplitude_Delay_Counter = chan.Amplitude_Delay;
            }
            else
            {
                chan.Amplitude_Delay = ((module [chan.Address_In_Pattern] << 3) ^ 0xf8) + 9;
            }
        }
        else if (val == 0xfc)
        {
            player->WriteAy(AY_ENV_SHAPE, 12);
        }
        else if (val == 0xfe)
        {
            player->WriteAy(AY_ENV_SHAPE, 14);
        }
        chan.Address_In_Pattern++;
    }
    chan.Note_Skip_Counter = chan.Number_Of_Notes_To_Skip;
}

void ASC_GetRegisters(unsigned char *module, ASC_Channel_Parameters &chan, unsigned char &TempMixer)
{
    short j;
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
                {
                    chan.Addition_To_Amplitude++;
                    chan.Amplitude_Delay_Counter += chan.Amplitude_Delay;
                }
            }
        }
        if((module [chan.Point_In_Sample] & 128) != 0)
        {
            chan.Loop_Point_In_Sample = chan.Point_In_Sample;
        }
        if((module [chan.Point_In_Sample] & 96) == 32)
        {
            chan.Sample_Finished = true;
            chan.Ton_Deviation += (short)module [chan.Point_In_Sample + 1];
            TempMixer |= (module [chan.Point_In_Sample + 2] & 9) << 3;
        }
        if((module [chan.Point_In_Sample + 2] & 6) == 2)
            Sample_Says_OK_for_Envelope = true;
        else
            Sample_Says_OK_for_Envelope = false;
        if((module [chan.Point_In_Sample + 2] & 6) == 4)
        {
            if(chan.Addition_To_Amplitude > -15)
                chan.Addition_To_Amplitude--;
        }
        if((module [chan.Point_In_Sample + 2] & 6) == 6)
        {
            if(chan.Addition_To_Amplitude < 15)
                chan.Addition_To_Amplitude++;
        }
        chan.Amplitude = chan.Addition_To_Amplitude + (module [chan.Point_In_Sample + 2] >> 4);
        if((short)chan.Amplitude < 0)
            chan.Amplitude = 0;
        else if(chan.Amplitude > 15)
            chan.Amplitude = 15;
        chan.Amplitude = (chan.Amplitude * (chan.Volume + 1)) >> 4;
        if(Sample_Says_OK_for_Envelope && ((TempMixer && 64) != 0))
            player->WriteAy(AY_ENV_FINE, player->ReadAy(AY_ENV_FINE) + ((module [chan.Point_In_Sample] << 3) / 8));
        else
            chan.Current_Noise += ((short)module [chan.Point_In_Sample] << 3) / 8;
        chan.Point_In_Sample += 3;
        if((module [chan.Point_In_Sample - 3] & 64) != 0)
        {
            if(!chan.Break_Sample_Loop)
                chan.Point_In_Sample = chan.Loop_Point_In_Sample;
            else if((module [chan.Point_In_Sample - 3] & 32) != 0)
                chan.Sample_Finished = true;
        }
        if((module [chan.Point_In_Ornament] & 128) != 0)
            chan.Loop_Point_In_Ornament = chan.Point_In_Ornament;
        chan.Addition_To_Note += module [chan.Point_In_Ornament + 1];
        chan.Current_Noise += (-(short)(module [chan.Point_In_Ornament] & 0x10)) | module [chan.Point_In_Ornament];
        if((module [chan.Point_In_Ornament - 2] & 64) != 0)
            chan.Point_In_Ornament = chan.Loop_Point_In_Ornament;
        if((TempMixer & 64) == 0)
            player->WriteAy(AY_NOISE_PERIOD, (unsigned char)(chan.Current_Ton_Sliding >> 8) + chan.Current_Noise);
        j = chan.Note + chan.Addition_To_Note;
        if(j < 0)
            j = 0;
        else if(j > 0x55)
            j = 0x55;
        chan.Ton = (ASM_Table [j] + chan.Ton_Deviation + (chan.Current_Ton_Sliding / 16));
        if(chan.Ton_Sliding_Counter != 0)
        {
            if((short)chan.Ton_Sliding_Counter > 0)
                chan.Ton_Sliding_Counter--;
            chan.Current_Ton_Sliding += chan.Substruction_for_Ton_Sliding;
        }
        if(chan.Envelope_Enabled && Sample_Says_OK_for_Envelope)
            chan.Amplitude |= 0x10;
    }
    TempMixer >>= 1;
}

void ASC_Play(unsigned char *module, ELAPSED_CALLBACK callback, void *arg)
{
    ASC1_File *header = (ASC1_File *)module;
    unsigned char TempMixer;
    unsigned short ascPatPt = header->ASC1_PatternsPointers;
    if (timeElapsed >= maxElapsed)
    {
        if (callback)
            callback(arg);
    }

    if(--ASC.DelayCounter <= 0)
    {
        if(--ASC_A.Note_Skip_Counter < 0)
        {
            if(module [ASC_A.Address_In_Pattern] == 255)
            {
                if(++ASC.CurrentPosition >= header->ASC1_Number_Of_Positions)
                    ASC.CurrentPosition = header->ASC1_LoopingPosition;
                ASC_A.Address_In_Pattern = (*(unsigned short *) &module [ascPatPt + 6 * module[ASC.CurrentPosition + 9]]) + ascPatPt;
                ASC_B.Address_In_Pattern = (*(unsigned short *) &module [ascPatPt + 6 * module[ASC.CurrentPosition + 9] + 2]) + ascPatPt;
                ASC_C.Address_In_Pattern = (*(unsigned short *) &module [ascPatPt + 6 * module[ASC.CurrentPosition + 9] + 4]) + ascPatPt;
                ASC_A.Initial_Noise = 0;
                ASC_B.Initial_Noise = 0;
                ASC_C.Initial_Noise = 0;
            }
            ASC_PatternInterpreter(module, ASC_A);
        }
        if(--ASC_B.Note_Skip_Counter < 0)
        {
            ASC_PatternInterpreter(module, ASC_B);
        }
        if(--ASC_C.Note_Skip_Counter < 0)
        {
            ASC_PatternInterpreter(module, ASC_C);
        }
        ASC.DelayCounter = ASC.Delay;
    }

    TempMixer =0;
    ASC_GetRegisters(module, ASC_A, TempMixer);
    ASC_GetRegisters(module, ASC_B, TempMixer);
    ASC_GetRegisters(module, ASC_C, TempMixer);

    player->WriteAy(AY_MIXER, TempMixer);
    player->WriteAy(AY_CHNL_A_FINE, ASC_A.Ton & 0xff);
    player->WriteAy(AY_CHNL_A_COARSE, (ASC_A.Ton & 0xf00) >> 8);
    player->WriteAy(AY_CHNL_B_FINE, ASC_B.Ton & 0xff);
    player->WriteAy(AY_CHNL_B_COARSE, (ASC_B.Ton & 0xf00) >> 8);
    player->WriteAy(AY_CHNL_C_FINE, ASC_C.Ton & 0xff);
    player->WriteAy(AY_CHNL_C_COARSE, (ASC_C.Ton & 0xf00) >> 8);
    player->WriteAy(AY_CHNL_A_VOL, ASC_A.Amplitude);
    player->WriteAy(AY_CHNL_B_VOL, ASC_B.Amplitude);
    player->WriteAy(AY_CHNL_C_VOL, ASC_C.Amplitude);

    timeElapsed++;
}


