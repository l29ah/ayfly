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
#define VTX_Loop (header->Loop0 | (header->Loop1 << 8))
#define VTX_ChipFrq (header->ChipFrq0 | (header->ChipFrq1 << 8))
#define VTX_Year (header->Year0 | (header->Year1 << 8))
#define VTX_UnpackSize (header->UnpackSize | (header->UnpackSize1 << 8) | (header->UnpackSize2 << 16) | (header->UnpackSize3 << 24))

