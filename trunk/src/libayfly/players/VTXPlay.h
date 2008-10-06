struct VTX_File
{
    unsigned char Id0, Id1;
    unsigned char Mode;
    unsigned char Loop0, Loop1;
    unsigned char ChipFrq0, ChipFrq1;
    unsigned char InterFrq;
    unsigned char Year0, Year1;
    unsigned char UnpackSize0, UnpackSize1, UnpackSize2, UnpackSize3;
};

#define VTX_Id (header->Id0 | (header->Id1 << 8))
#define VTX_Loop (header->Loop0 | (header->Loop1 << 8))
#define VTX_ChipFrq (header->ChipFrq0 | (header->ChipFrq1 << 8))
#define VTX_Year (header->Year0 | (header->Year1 << 8))
#define VTX_UnpackSize (header->UnpackSize0 | (header->UnpackSize1 << 8) | (header->UnpackSize2 << 16) | (header->UnpackSize3 << 24))

struct VTX_SongInfo
{
    int i;
    unsigned char *module;
};

void VTX_Init(AYSongInfo &info)
{
    unsigned char *module = info.module;
    VTX_File *header = (VTX_File *) module;
    if(info.data)
    {
        delete (VTX_SongInfo *) info.data;
        info.data = 0;
    }
    info.data = (void *) new VTX_SongInfo;
    if(!info.data)
        return;
    if((VTX_Id != 0x5941) && (VTX_Id != 0x4d59) && (VTX_Id != 0x7961) && (VTX_Id != 0x6d79))
    {
        return;
    }
    if(info.module != 0)
    {
        delete [] info.module;
        info.module_len = VTX_UnpackSize;
        info.module = new unsigned char [info.module_len];
        memset(info.module, 0, sizeof(info.module_len));
    }
    ay_sys_decodelha(info);
    info.module = 0;
    
}

void VTX_Play(AYSongInfo &info)
{
}

void VTX_Cleanup(AYSongInfo &info)
{
    if(info.data)
    {
        delete (VTX_SongInfo *) info.data;
        info.data = 0;
    }
}

void VTX_GetInfo(AYSongInfo &info)
{
    unsigned char *module = info.file_data;
    VTX_File *header = (VTX_File *) module;
    info.Length = VTX_UnpackSize / 14;
    info.Loop = VTX_Loop;
    
}
