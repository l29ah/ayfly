/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew                                 *
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

#include "ayfly.h"

Z80EX_BYTE readMemory(Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data)
{
    unsigned char *mem = ((AYSongInfo *)user_data)->module;
    return mem [addr];
}

void writeMemory(Z80EX_CONTEXT *cpu, Z80EX_WORD addr, Z80EX_BYTE value, void *user_data)
{
    unsigned char *mem = ((AYSongInfo *)user_data)->module;
    mem [addr] = value;

}
Z80EX_BYTE readPort(Z80EX_CONTEXT *cpu, Z80EX_WORD port, void *user_data)
{
    unsigned char *io = ((AYSongInfo *)user_data)->z80IO;
    return io [port];
}

void writePort(Z80EX_CONTEXT *cpu, Z80EX_WORD port, Z80EX_BYTE value, void *user_data)
{

    AYSongInfo *info = (AYSongInfo *)user_data;
    unsigned char *io = info->z80IO;
    if ((port == 0xfffd) || ((port & 0xc000) == 0xc000)) //ay control port
    {
        info->ay_reg = value;
    }
    else if ((port == 0xbffd) || ((port & 0xc000) == 0x8000)) // ay data port
    {
        info->player->WriteAy(info->ay_reg, value);
        io [65533] = value;
    }
    else
    {
        io [port] = value;
    }
}

Z80EX_BYTE readInt(Z80EX_CONTEXT *cpu, void *user_data)
{
    return 0xff;
}


bool ay_sys_initz80(AYSongInfo &info)
{
    info.z80ctx = z80ex_create(readMemory, &info, writeMemory, &info, readPort, &info, writePort, &info, readInt, 0);
    if(!info.z80ctx)
        return false;
    z80ex_reset(info.z80ctx);
    info.ay_reg = 0xff;
    return true;
}

void ay_sys_resetz80(AYSongInfo &info)
{
    z80ex_reset(info.z80ctx);
}

void ay_sys_shutdownz80(AYSongInfo &info)
{
    ay_sys_resetz80(info);
    memset(info.module, 0, 65536);
    memset(info.z80IO, 0, 65536);
}

void ay_z80exec(AYSongInfo &info)
{
    if(info.soft_play_proc)
    {
        info.soft_play_proc(info);
        return;
    }
    do
    {
        z80ex_step(info.z80ctx);
    }
    while (z80ex_get_reg(info.z80ctx, regPC) != 8);
    if (++info.timeElapsed >= info.Length)
    {
        if (info.callback)
            info.callback(info.callback_arg);
    }
}
