/*
 * Z80~Ex, ZILoG Z80 CPU emulator.
 *
 * by Boo-boo [boo_boo(^at^)inbox.ru]
 * contains code from the FUSE project (http://fuse-emulator.sourceforge.net)
 * released under GNU GPL v2
 *
 */

#include <stdlib.h>
#include <string.h>

#define __Z80EX_SELF_INCLUDE

#include "z80ex/typedefs.h"
#include "z80ex/include/z80ex.h"
#include "z80ex/macros.h"

#define temp_byte cpu->tmpbyte
#define temp_byte_s cpu->tmpbyte_s
#define temp_addr cpu->tmpaddr
#define temp_word cpu->tmpword

static int initialized=0;

/* Whether a half carry occured or not can be determined by looking at
the 3rd bit of the two arguments and the result; these are hashed
into this table in the form r12, where r is the 3rd bit of the
result, 1 is the 3rd bit of the 1st argument and 2 is the
third bit of the 2nd argument; the tables differ for add and subtract
operations */
static const Z80EX_BYTE halfcarry_add_table[] =
  { 0, FLAG_H, FLAG_H, FLAG_H, 0, 0, 0, FLAG_H };
static const Z80EX_BYTE halfcarry_sub_table[] =
  { 0, 0, FLAG_H, 0, FLAG_H, 0, FLAG_H, FLAG_H };

/* Similarly, overflow can be determined by looking at the 7th bits; again
the hash into this table is r12 */
static const Z80EX_BYTE overflow_add_table[] = { 0, 0, 0, FLAG_V, FLAG_V, 0, 0, 0 };
static const Z80EX_BYTE overflow_sub_table[] = { 0, FLAG_V, 0, 0, 0, 0, FLAG_V, 0 };

/*flag tables*/
static Z80EX_BYTE sz53_table[0x100]; /* The S, Z, 5 and 3 bits of the index */
static Z80EX_BYTE parity_table[0x100]; /* The parity of the lookup value */
static Z80EX_BYTE sz53p_table[0x100]; /* OR the above two tables together */

#include "z80ex/daa_table.c"
#include "z80ex/opcodes/opcodes_base.c"
#include "z80ex/opcodes/opcodes_dd.c"
#include "z80ex/opcodes/opcodes_fd.c"
#include "z80ex/opcodes/opcodes_cb.c"
#include "z80ex/opcodes/opcodes_ed.c"
#include "z80ex/opcodes/opcodes_ddcb.c"
#include "z80ex/opcodes/opcodes_fdcb.c"

/* Initalise the tables used to set flags */
static void init_tables(void)
{
	int i,j,k;
	Z80EX_BYTE parity;

	for(i=0;i<0x100;i++)
	{
		sz53_table[i]= i & ( FLAG_3 | FLAG_5 | FLAG_S );
		j=i; parity=0;
		for(k=0;k<8;k++) { parity ^= j & 1; j >>=1; }
		parity_table[i]= ( parity ? 0 : FLAG_P );
		sz53p_table[i] = sz53_table[i] | parity_table[i];
	}

	sz53_table[0]  |= FLAG_Z;
	sz53p_table[0] |= FLAG_Z;
}

/* do one opcode (instruction or prefix) */
LIB_EXPORT int z80ex_step(Z80EX_CONTEXT *cpu)
{
	Z80EX_BYTE opcode, d;
	z80ex_opcode_fn ofn=NULL;

	cpu->doing_opcode=1;
	cpu->noint_once=0;
	cpu->tstate=0;
	cpu->op_tstate=0;

	opcode=READ_OP_M1(); /*fetch opcode*/
	if(cpu->int_vector_req)
	{
		TSTATES(2); /*interrupt eats two extra wait-states*/
	}
	R++; /*R increased by one on every first M1 cycle*/

	T_WAIT_UNTIL(4); /*M1 cycle eats min 4 t-states*/

	if(!cpu->prefix) opcodes_base[opcode](cpu);
	else
	{
		if((cpu->prefix | 0x20) == 0xFD && ((opcode | 0x20) == 0xFD || opcode == 0xED))
		{
			cpu->prefix=opcode;
			cpu->noint_once=1; /*interrupts are not accepted immediately after prefix*/
		}
		else
		{
			switch(cpu->prefix)
			{
				case 0xDD:
				case 0xFD:
					if(opcode == 0xCB)
					{
						d=READ_OP(); /*displacement*/
						temp_byte_s=(d & 0x80)? -(((~d) & 0x7f)+1): d;
						opcode=READ_OP();
						ofn = (cpu->prefix == 0xDD)? opcodes_ddcb[opcode]: opcodes_fdcb[opcode];
					}
					else
					{
						ofn = (cpu->prefix == 0xDD)? opcodes_dd[opcode]: opcodes_fd[opcode];
						if(ofn == NULL) ofn=opcodes_base[opcode]; /*'mirrored' instructions*/
					}
					break;

				case 0xED:
					ofn = opcodes_ed[opcode];
					if(ofn == NULL) ofn=opcodes_base[0x00];
					break;

				case 0xCB:
					ofn = opcodes_cb[opcode];
					break;

				default:
					/*this must'nt happen!*/
					break;
			}

			ofn(cpu);

			cpu->prefix=0;
		}
	}

	cpu->doing_opcode=0;
	return(cpu->tstate);
}

LIB_EXPORT Z80EX_BYTE z80ex_last_op_type(Z80EX_CONTEXT *cpu)
{
	return(cpu->prefix);
}

LIB_EXPORT void z80ex_reset(Z80EX_CONTEXT *cpu)
{
	PC=0x0000; IFF1=IFF2=0; IM=IM0;
	AF=SP=BC=DE=HL=IX=IY=AF_=BC_=DE_=HL_=0xffff;
	I=R=R7=0;
	cpu->noint_once=0; cpu->halted=0;
	cpu->int_vector_req=0;
	cpu->doing_opcode=0;
	cpu->tstate=cpu->op_tstate=0;
	cpu->prefix=0;
}

/**/
LIB_EXPORT Z80EX_CONTEXT *z80ex_create(
	z80ex_mread_cb mrcb_fn, void *mrcb_data,
	z80ex_mwrite_cb mwcb_fn, void *mwcb_data,
	z80ex_pread_cb prcb_fn, void *prcb_data,
	z80ex_pwrite_cb pwcb_fn, void *pwcb_data,
	z80ex_intread_cb ircb_fn, void *ircb_data
)
{
	Z80EX_CONTEXT *cpu;

	if(!initialized)
	{
		initialized=1;
		init_tables();
	}

	if((cpu=(Z80EX_CONTEXT *)malloc(sizeof(Z80EX_CONTEXT))) == NULL) return(NULL);
	memset(cpu,0x00,sizeof(Z80EX_CONTEXT));

	z80ex_reset(cpu);

	cpu->mread_cb=mrcb_fn;
	cpu->mread_cb_user_data=mrcb_data;
	cpu->mwrite_cb=mwcb_fn;
	cpu->mwrite_cb_user_data=mwcb_data;
	cpu->pread_cb=prcb_fn;
	cpu->pread_cb_user_data=prcb_data;
	cpu->pwrite_cb=pwcb_fn;
	cpu->pwrite_cb_user_data=pwcb_data;
	cpu->intread_cb=ircb_fn;
	cpu->intread_cb_user_data=ircb_data;

	return(cpu);
}

LIB_EXPORT void z80ex_destroy(Z80EX_CONTEXT *cpu)
{
	free(cpu);
}

LIB_EXPORT void z80ex_set_tstate_callback(Z80EX_CONTEXT *cpu, z80ex_tstate_cb cb_fn, void *user_data)
{
	cpu->tstate_cb=cb_fn;
	cpu->tstate_cb_user_data=user_data;
}

/*non-maskable interrupt*/
LIB_EXPORT int z80ex_nmi(Z80EX_CONTEXT *cpu)
{
	if(cpu->doing_opcode || cpu->noint_once || cpu->prefix) return(0);

	cpu->doing_opcode=1;

	R++; /*accepting interrupt increases R by one*/
	IFF1=0;

	TSTATES(5);

	cpu->mwrite_cb(cpu, --SP, cpu->pc.b.h, cpu->mwrite_cb_user_data); /*PUSH PC -- high byte */
	TSTATES(3);

	cpu->mwrite_cb(cpu, --SP, cpu->pc.b.l, cpu->mwrite_cb_user_data); /*PUSH PC -- low byte */
	TSTATES(3);

	PC=0x0066;
	MEMPTR=PC; /*FIXME: is it really so?*/

	cpu->doing_opcode=0;

	return(11); /*NMI always takes 11 t-states*/
}

/*maskable interrupt*/
LIB_EXPORT int z80ex_int(Z80EX_CONTEXT *cpu)
{
	Z80EX_WORD inttemp;
	Z80EX_BYTE iv;
	unsigned long tt;

	/*If the INT line is low and IFF1 is set, and there's no opcode executing just now,
	a maskable interrupt is accepted, whether or not the
	last INT routine has finished*/
	if(!IFF1 || cpu->noint_once || cpu->doing_opcode || cpu->prefix) return(0);

	cpu->tstate=0;
	cpu->op_tstate=0;

	if(cpu->halted) { PC++; cpu->halted = 0; } /*so we met an interrupt... stop waiting*/

	/*When an INT is accepted, both IFF1 and IFF2 are cleared, preventing another interrupt from
	occurring which would end up as an infinite loop*/
	IFF1=IFF2=0;

	cpu->int_vector_req=1;
	cpu->doing_opcode=1;

	switch(IM)
	{
		case IM0:
			/*note: there's no need to do R++ and WAITs here, it'll be handled by z80ex_step*/
			tt=z80ex_step(cpu);

			while(cpu->prefix) /*this is not the end?*/
			{
				tt+=z80ex_step(cpu);
			}

			cpu->tstate=tt;
			break;

		case IM1:
			R++;
			TSTATES(2); /*two extra wait-states*/
			/*An RST 38h is executed, no matter what value is put on the bus or what
			value the I register has. 13 t-states (2 extra + 11 for RST).*/
			opcodes_base[0xff](cpu); /*RST38*/
			break;

		case IM2:
			R++;
			/*takes 19 clock periods to complete (seven to fetch the
			lower eight bits from the interrupting device, six to save the program
			counter, and six to obtain the jump address)*/
			iv=READ_OP();
			T_WAIT_UNTIL(7);
			inttemp=(0x100*I)+iv;

			PUSH(PC,7,10);

			READ_MEM(PCL,inttemp++,13); READ_MEM(PCH,inttemp,16);
			MEMPTR=PC;
			T_WAIT_UNTIL(19);

			break;
	}

	cpu->doing_opcode=0;
	cpu->int_vector_req=0;

	return(cpu->tstate);
}

LIB_EXPORT void z80ex_w_states(Z80EX_CONTEXT *cpu, unsigned w_states)
{
	TSTATES(w_states);
}

LIB_EXPORT void z80ex_next_t_state(Z80EX_CONTEXT *cpu)
{
	if(cpu->tstate_cb != NULL) cpu->tstate_cb(cpu, cpu->tstate_cb_user_data);
	cpu->tstate++;
	cpu->op_tstate++;
}

LIB_EXPORT Z80EX_WORD z80ex_get_reg(Z80EX_CONTEXT *cpu, Z80_REG_T reg)
{
	switch(reg)
	{
		case regAF: return(AF);
		case regBC: return(BC);
		case regDE: return(DE);
		case regHL: return(HL);
		case regAF_: return(AF_);
		case regBC_: return(BC_);
		case regDE_: return(DE_);
		case regHL_: return(HL_);
		case regIX: return(IX);
		case regIY: return(IY);
		case regPC: return(PC);
		case regSP: return(SP);
		case regI: return(I);
		case regR: return(R);
		case regR7: return(R7);
		case regIM: return(IM);
		case regIFF1: return(IFF1);
		case regIFF2: return(IFF2);
	}

	return(0);
}

LIB_EXPORT void z80ex_set_reg(Z80EX_CONTEXT *cpu, Z80_REG_T reg, Z80EX_WORD value)
{
	switch(reg)
	{
		case regAF: AF=value; return;
		case regBC: BC=value; return;
		case regDE: DE=value; return;
		case regHL: HL=value; return;
		case regAF_: AF_=value; return;
		case regBC_: BC_=value; return;
		case regDE_: DE_=value; return;
		case regHL_: HL_=value; return;
		case regIX: IX=value; return;
		case regIY: IY=value; return;
		case regPC: PC=value; return;
		case regSP: SP=value; return;
		case regI: I=(value & 0xff); return;
		case regR: R=(value & 0xff); return;
		case regR7: R7=(value & 0xff); return;
		case regIM:
			switch(value & 0x03)
			{
				case 0: IM=IM0; return;
				case 1: IM=IM1; return;
				case 2: IM=IM2; return;
			}
		case regIFF1: IFF1=(value & 0x01); return;
		case regIFF2: IFF2=(value & 0x01); return;
	}

	return;
}

LIB_EXPORT int z80ex_op_tstate(Z80EX_CONTEXT *cpu)
{
	return(cpu->tstate);
}

LIB_EXPORT int z80ex_doing_halt(Z80EX_CONTEXT *cpu)
{
	return(cpu->halted);
}
