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

Z80EX_CONTEXT *ctx = 0;
unsigned char *z80Memory = 0;
unsigned char *z80IO = 0;
AbstractAudio *player = 0;

static unsigned short ay_reg;
static long interrupt = Z80_TO_INTR;


Z80EX_BYTE readMemory(Z80EX_CONTEXT *cpu, Z80EX_WORD addr, int m1_state, void *user_data)
{
    //printf("[readMemory]: address = %d\n", (int)addr);
    unsigned char *mem = (unsigned char *)user_data;
    return mem [addr];
}

void writeMemory(Z80EX_CONTEXT *cpu, Z80EX_WORD addr, Z80EX_BYTE value, void *user_data)
{
    //printf("[writeMemory]: address = %d, data = %c\n", (int)addr, value);
    unsigned char *mem = (unsigned char *)user_data;
    mem [addr] = value;

}
Z80EX_BYTE readPort(Z80EX_CONTEXT *cpu, Z80EX_WORD port, void *user_data)
{
    //printf("[readPort]: port=%d\n", (int)port);
    unsigned char *io = (unsigned char *)user_data;
    return io [port];
}

void writePort(Z80EX_CONTEXT *cpu, Z80EX_WORD port, Z80EX_BYTE value, void *user_data)
{

    unsigned char *io = (unsigned char *)user_data;
    //printf("[writePort]: port=%d, value=%d\n", (int)port, (int)value);
    if ((port == 0xfffd) || ((port & 0xc000) == 0xc000)) //ay control port
    {
        ay_reg = value;
    }
    else if ((port == 0xbffd) || ((port & 0xc000) == 0x8000)) // ay data port
    {
        //printf("write: %d=>%d\n", (int)ay_reg, (int)value);
        player->WriteAy(ay_reg, value);
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

Z80EX_BYTE readMemoryDasm(Z80EX_WORD addr, void *user_data)
{
    unsigned char *mem = (unsigned char *)user_data;
    return mem [addr];
}


void initSpeccy()
{
    z80Memory = (unsigned char *)calloc(65536, 1);
    z80IO = (unsigned char *)calloc(65536, 1);
    ctx = z80ex_create(readMemory, z80Memory, writeMemory, z80Memory, readPort, z80IO, writePort, z80IO, readInt, 0);
    z80ex_reset(ctx);
    ay_reg = 0xff;
}

void resetSpeccy()
{
    interrupt = Z80_TO_INTR;
    z80ex_reset(ctx);
}

void shutdownSpeccy()
{
    if (z80Memory)
        free(z80Memory);
    if (z80IO)
        free(z80IO);
    z80Memory = z80IO = 0;
}

void setPlayer(AbstractAudio *_player)
{
    player = _player;
}

void execInstruction(ELAPSED_CALLBACK callback, void *arg)
{
    if(soft_play_proc)
    {
        soft_play_proc(z80Memory, callback, arg);
        return;
    }
    do
    {
        z80ex_step(ctx);
    }
    while (z80ex_get_reg(ctx, regPC) != 8);
    if (++timeElapsed >= maxElapsed)
    {
        if (callback)
            callback(arg);
    }
}
