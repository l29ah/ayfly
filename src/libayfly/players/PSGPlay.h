/* This player module was ported from:
 AY-3-8910/12 Emulator
 Version 3.0 for Windows 95
 Author Sergey Vladimirovich Bulba
 (c)1999-2004 S.V.Bulba
 */

struct PSG_SongInfo
{
    unsigned short PSG_Skip;
    unsigned short file_pointer;
};

void PSG_Init(AYSongInfo &info)
{
    unsigned char *module = info.module;
    AbstractAudio *player = info.player;
    if(info.data)
    {
        delete (PSG_SongInfo *)info.data;
        info.data = 0;
    }
    info.data = (void *)new PSG_SongInfo;
    if(!info.data)
        return;
    ((PSG_SongInfo *)info.data)->PSG_Skip = 0;
    ((PSG_SongInfo *)info.data)->file_pointer = 16;

    player->ResetAy();

}

void PSG_Play(AYSongInfo &info)
{
    unsigned char *module = info.module;
    AbstractAudio *player = info.player;
    PSG_SongInfo *psg_info = (PSG_SongInfo *)info.data;
    unsigned char b, b2;
    if(info.timeElapsed >= info.Length)
    {
        info.timeElapsed = info.Loop;
        if(info.callback)
            info.callback(info.callback_arg);
    }
    info.timeElapsed++;
    if(psg_info->PSG_Skip > 0)
    {
        psg_info->PSG_Skip--;
        return;
    }
    if(info.file_len <= 16)
        return;
    if(psg_info->file_pointer >= info.file_len)
    {
        psg_info->PSG_Skip = 0;
        psg_info->file_pointer = 16;
    }

    do
    {
        b = module[psg_info->file_pointer++];
        if(b == 255)
            return;
        if(b == 254)
        {
            b = module[psg_info->file_pointer++];
            psg_info->PSG_Skip = b * 4 - 1;
            return;
        }

        if(psg_info->file_pointer < info.file_len)
        {
            b2 = module[psg_info->file_pointer++];
            if(b < 14)
            {
                switch(b)
                {

                    case 13:
                        player->WriteAy(AY_ENV_SHAPE, b2 & 15);
                        break;
                    case 1:
                    case 3:
                    case 5:
                        player->WriteAy(b, b2 & 15);
                        break;
                    case 6:
                        player->WriteAy(AY_NOISE_PERIOD, b2 & 31);
                        break;
                    case 7:
                        player->WriteAy(AY_MIXER, b2 & 63);
                        break;
                    case 8:
                        player->WriteAy(AY_CHNL_A_VOL, b2 & 31);
                        break;
                    case 9:
                        player->WriteAy(AY_CHNL_B_VOL, b2 & 31);
                        break;
                    case 10:
                        player->WriteAy(AY_CHNL_C_VOL, b2 & 31);
                        break;
                    default:
                        player->WriteAy(b, b2);
                        break;
                }
            }
        }

    }
    while(psg_info->file_pointer <= info.file_len);

}

void PSG_GetInfo(AYSongInfo &info)
{
    unsigned char *module = info.file_data;
    unsigned short file_pointer = 16;
    unsigned char b, b1;
    unsigned long tm = 0;

    while(file_pointer < info.file_len)
    {
        b = module [file_pointer++];
        if(b == 255)
            tm++;
        else if(b == 254)
        {
            b1 = module [file_pointer++];
            tm += b1 * 4;
        }
        else
            file_pointer++;
    }
    if(b < 254)
        tm++;
    info.Length = tm;
}

void PSG_Cleanup(AYSongInfo &info)
{
    if(info.data)
    {
        delete (VTX_SongInfo *)info.data;
        info.data = 0;
    }
}
