//LZH-file header
struct LZHFileHeader
{
    unsigned char HSize;
    unsigned char ChkSum;
    char Method[5];
    unsigned char CompSize0, CompSize1, CompSize2, CompSize3; //int
    unsigned char UCompSize0, UCompSize1, UCompSize2, UCompSize3; //int
    unsigned char Dos_DT0, Dos_DT1, Dos_DT2, Dos_DT3; //int
    unsigned char Attr0, Attr1; //unsigned short
    unsigned char FileNameLen;
};

struct YM5FileHeader
{
 unsigned char Id0, Id1, Id2, Id3; //dword
 char Leo [8];
 unsigned char Num_of_tiks0, Num_of_tiks1, Num_of_tiks2, Num_of_tiks3; //dword
 unsigned char Song_Attr0, Song_Attr1, Song_Attr2, Song_Attr3; //dword
 unsigned char Num_of_Dig0, Num_of_Dig1; //word
 unsigned char ChipFrq0, ChipFrq1, ChipFrq2, ChipFrq3; //dword
 unsigned char InterFrq0, InterFrq1; //word
 unsigned char Loop0, Loop1, Loop2, Loop3; //dword
 unsigned char Add_Size0, Add_Size1; //word
};


#define LZH_CompSize (lzh_header->CompSize0 | (lzh_header->CompSize1 << 8) | (lzh_header->CompSize2 << 16) | (lzh_header->CompSize3 << 24))
#define LZH_UCompSize (lzh_header->UCompSize0 | (lzh_header->UCompSize1 << 8) | (lzh_header->UCompSize2 << 16) | (lzh_header->UCompSize3 << 24))
#define LZH_DosDT (lzh_header->Dos_DT0 | (lzh_header->Dos_DT1 << 8) | (lzh_header->Dos_DT2 << 16) | (lzh_header->Dos_DT3 << 24))
#define LZH_Attr (lzh_header->Attr0 | (lzh_header->Attr1 << 8))

#define YM5_Id (ym5_header->Id0 | (ym5_header->Id1 << 8) | (ym5_header->Id2 << 16) | (ym5_header->Id3 << 24))
#define YM5_Num_of_tiks (ym5_header->Num_of_tiks0 | (ym5_header->Num_of_tiks1 << 8) | (ym5_header->Num_of_tiks2 << 16) | (ym5_header->Num_of_tiks3 << 24))
#define YM5_Song_Attr (ym5_header->Song_Attr0 | (ym5_header->Song_Attr1 << 8) | (ym5_header->Song_Attr2 << 16) | (ym5_header->Song_Attr3 << 24))
#define YM5_Num_of_Dig (ym5_header->Num_of_Dig0 | (ym5_header->Num_of_Dig1 << 8))
#define YM5_ChipFrq (ym5_header->ChipFrq0 | (ym5_header->ChipFrq1 << 8) | (ym5_header->ChipFrq2 << 16) | (ym5_header->ChipFrq3 << 24))
#define YM5_InterFrq (ym5_header->InterFrq0 | (ym5_header->InterFrq1 << 8) | (ym5_header->InterFrq2 << 16) | (ym5_header->InterFrq3 << 24))
#define YM5_Loop (ym5_header->Loop0 | (ym5_header->Loop1 << 8) | (ym5_header->Loop2 << 16) | (ym5_header->Loop3 << 24))
#define YM5_Add_Size (ym5_header->Add_Size0 | (ym5_header->Add_Size1 << 8))


void YM_Init(AYSongInfo &info)
{
    unsigned char *module = info.file_data;
    LZHFileHeader *lzh_header = (LZHFileHeader *)module;

    if(info.module != 0)
    {
        delete[] info.module;
        info.module_len = LZH_UCompSize * 2;
        info.module = new unsigned char[info.module_len];
        memset(info.module, 0, info.module_len);
    }
    ay_sys_decodelha(info, lzh_header->HSize + 2);
    YM5FileHeader *ym5_header = (YM5FileHeader *)info.module;
    if(!strcmp(ym5_header, "YM6!"))
    {
        
    }
    
    
}

bool YM_Detect(unsigned char *module, unsigned long length)
{
    LZHFileHeader *header = (LZHFileHeader *)module;
    if(!strcmp(header->Method, "-lh5-"))
        return true;
    return false;

}
