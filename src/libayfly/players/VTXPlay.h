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
#define VTX_UnpackSize (header->UnpackSize | (header->UnpackSize1 << 8) | (header->UnpackSize2 << 16) | (header->UnpackSize3 << 24))

struct VTX_SongInfo
{
    int i;
    unsigned char *module;
};

void VTX_Init(AYSongInfo &info)
{
    unsigned char *module = info.module;
    VTX_File *header = (VTX_File *) module;
    AbstractAudio *player = info.player;
    int i,j,k;
    intLooping_VBL,TimLen;
    int ChipFrq,PlrFrq,ChanMode;
    intSter:integer;
    signed char Ch;
    unsigned short Wrd;
    unsigned long DWrd;
    int FormSpec;
    int orisize;
    orisize = 0;
    Looping_VBL = -1;
    TimLen = 0;
    ChipFrq = -1;
    PlrFrq = -1;
    ChanMode = -1;
    ChType = 0;
    Ster = 0;
    FormSpec = -1;
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

}
