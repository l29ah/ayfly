/*
 * Z80~Ex, ZILoG Z80 CPU emulator.
 *
 * by Boo-boo [boo_boo(^at^)inbox.ru]
 * contains code from the FUSE project (http://fuse-emulator.sourceforge.net)
 * Released under GNU GPL v2
 *
 */

#ifndef _Z80EX_DASM_H_INCLUDED
#define _Z80EX_DASM_H_INCLUDED

#include "z80ex/include/z80ex_common.h"

/*callback that returns byte for a given adress*/
typedef Z80EX_BYTE (*z80ex_dasm_readbyte_cb)(Z80EX_WORD addr, void *user_data);

/*flags*/
enum Z80EX_DASM_FLAGS {
	WORDS_DEC = 1,
	BYTES_DEC = 2
};

#ifndef __Z80EX_SELF_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

/*z80ex_dasm: disassemble single instruction at the given adress
output - text buffer
output_size - buffer length
flags - output format settings
t_states will be set to T-states instruction
for branching commands t_states2 will be T-states when PC is changed, for other commands t_states2=0
readbyte_cb - callback function for reading byte at given adress
addr - adress of the first byte of instruction
user_data - will be passed to readbyte callback
*/
extern int z80ex_dasm(char *output, int output_size, unsigned flags, int *t_states, int *t_states2,
	z80ex_dasm_readbyte_cb readbyte_cb, Z80EX_WORD addr, void *user_data);

#ifdef __cplusplus
}
#endif

#endif

#endif
