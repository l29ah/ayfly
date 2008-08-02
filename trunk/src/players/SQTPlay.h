/* This player module was ported from:
 AY-3-8910/12 Emulator
 Version 3.0 for Windows 95
 Author Sergey Vladimirovich Bulba
 (c)1999-2004 S.V.Bulba
 */
unsigned short SQL_Table[] =
{ 0xd5d, 0xc9c, 0xbe7, 0xb3c, 0xa9b, 0xa02, 0x973, 0x8eb, 0x86b, 0x7f2, 0x780, 0x714, 0x6ae, 0x64e, 0x5f4, 0x59e, 0x54f, 0x501, 0x4b9, 0x475, 0x435, 0x3f9, 0x3c0, 0x38a, 0x357, 0x327, 0x2fa, 0x2cf, 0x2a7, 0x281, 0x25d, 0x23b, 0x21b, 0x1fc, 0x1e0, 0x1c5, 0x1ac, 0x194, 0x17d, 0x168, 0x153, 0x140, 0x12e, 0x11d, 0x10d, 0xfe, 0xf0, 0xe2, 0xd6, 0xca, 0xbe, 0xb4, 0xaa, 0xa0, 0x97, 0x8f, 0x87, 0x7f, 0x78, 0x71, 0x6b, 0x65, 0x5f, 0x5a, 0x55, 0x50, 0x4c, 0x47, 0x43, 0x40, 0x3c, 0x39, 0x35, 0x32, 0x30, 0x2d, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1b, 0x19, 0x18, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0xf, 0xe };

#ifndef __SYMBIAN32__
#pragma pack(push, 1)
#endif
struct SQT_File
{
    unsigned short SQT_Size, SQT_SamplesPointer, SQT_OrnamentsPointer, SQT_PatternsPointer, SQT_PositionsPointer, SQT_LoopPointer;
};
#ifndef __SYMBIAN32__
#pragma pack(pop)
#endif

struct SQT_Channel_Parameters
{
    unsigned short Address_In_Pattern, SamplePointer, Point_In_Sample, OrnamentPointer, Point_In_Ornament, Ton, ix27;
    unsigned char Volume, Amplitude, Note, ix21;
    short Ton_Slide_Step, Current_Ton_Sliding;
    char Sample_Tik_Counter, Ornament_Tik_Counter, Transposit;
    bool Enabled, Envelope_Enabled, Ornament_Enabled, Gliss, MixNoise, MixTon, b4ix0, b6ix0, b7ix0;
};

struct SQT_Parameters
{
    unsigned char Delay, DelayCounter, Lines_Counter;
    unsigned short Positions_Pointer;
};

SQT_Parameters SQT;
SQT_Channel_Parameters SQT_A, SQT_B, SQT_C;


void SQTGetInfo(const unsigned char *module, SongInfo &info)
{
    SQT_File *header = (SQT_File *)module;
    short t;
    unsigned char a,b,b1;
    unsigned long tm;
    int i,j;
    char a1,a2,a3,a11,a22,a33;
    unsigned long j1,j2,j3;
    int k,c1,c2,c3,c4,c5,c8;
    unsigned short *pwrd;
    bool Env1,Env2,Env3;
    unsigned long pptr,cptr;
    bool f71,f72,f73,
    f61,f62,f63,
    f41,f42,f43,flg;
    unsigned short j11,j22,j33;
    f71 = f72 = f73 = f61 = f62 = f63 = f41 = f42 = f43 = flg = false;

    pptr = header->SQT_PositionsPointer;
    while(module [pptr] != 0)
    {
        if(pptr == header->SQT_LoopPointer)
            info.Loop = tm;
        f41 = module [pptr] & 128 ? true : false;
        j1 = (*(unsigned short *)&module [(unsigned char)(module [pptr] * 2) + header->SQT_PatternsPointer]);
        j1++;
        pptr += 2;
        f42 = module [pptr] & 128 ? true : false;
        j2 = (*(unsigned short *)&module [(unsigned char)(module [pptr] * 2) + header->SQT_PatternsPointer]);
        j2++;
        pptr += 2;
        f43 = module [pptr] & 128 ? true : false;
        j3 = (*(unsigned short *)&module [(unsigned char)(module [pptr] * 2) + header->SQT_PatternsPointer]);
        j3++;
        pptr += 2;
        b = module [pptr];
        pptr++;
        a1 = a2 = a3 = 0;
        for(i = 1; i < module [j1 - 1]; i++)
        {
            if(a1 != 0)
            {
                a1--;
                if(f71)
                {
                    cptr = j11;
                    f61 = false;
                    if(module [cptr] <= 0x7f)
                    {
                        cptr++;
                        unsigned char val = module [cptr];
                        if(val <= 0x7f)
                        {
                            cptr++;
                            if(f61)
                                j1 = cptr + 1;
                            switch(module [cptr - 1] - 1)
                            {
                                case 4:
                                    if(f41)
                                    {
                                        b = module [cptr] & 31;
                                    }
                                    break;
                                case 5:
                                    if(f41)
                                    {
                                        b = (b + module [cptr]) & 31;
                                        if(b == 0)
                                            b = 32;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        else if(val >= 0x80)
                        {
                            if(val & 64 != 0)
                            {
                                cptr++;
                                if(module [cptr] & 15 != 0)
                                {
                                    cptr++;
                                    if(f61)
                                        j1 = cptr + 1;
                                    switch((module [cptr - 1] & 15) - 1)
                                    {
                                        case 4:
                                            if(f41)
                                            {
                                                b = module [cptr] & 31;
                                                if(b == 0)
                                                    b = 32;
                                            }
                                            break;
                                        case 5:
                                            if(f41)
                                            {
                                                b = (b + module [cptr]) & 31;
                                                if(b == 0)
                                                    b = 32;
                                            }
                                            break;
                                        default:
                                            break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                cptr = j1;
                f61 = true;
                f71 = false;
                while(true)
                {

                }
            }
        }
    }


}
