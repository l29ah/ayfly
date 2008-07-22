/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew   				               *
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

#include "common.h"

#ifndef PI
#define PI 3.1415926535
#endif

Filter3::Filter3()
{
}

Filter3::~Filter3()
{

}

void Filter3::Init(const FilterOpts *opts)
{
	unsigned long Fs = opts->Fs;
	double f0 = opts->f0;
	double bw = opts->bw;
	double w0 = 2 * PI * f0 / Fs;

        in_delay00 = in_delay01 = in_delay10 = in_delay11 = in_delay20 = in_delay21 = 0;
        out_delay00 = out_delay01 = out_delay10 = out_delay11 = out_delay20 = out_delay21 = 0;

	double alpha = sin(w0) * sinh((log((double)2) / 2) * bw * (w0 / sin(w0)));
	//double one_q = 2 * sinh(log((double)2) / 2 * bw * w0 / sin(w0));

	switch(opts->type)
	{
	case LPF:
		b0 = (1 - cos(w0)) / 2;
		b1 = 1 - cos(w0);
		b2 = (1 - cos(w0)) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cos(w0);
		a2 = 1 - alpha;
		break;
	case HPF:
		b0 = (1 + cos(w0)) / 2;
		b1 = -(1 + cos(w0));
		b2 = (1 + cos(w0)) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cos(w0);
		a2 = 1 - alpha;
		break;
	case BPF:
		b0 = sin(w0) / 2;
		b1 = 0;
		b2 = -sin(w0) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cos(w0);
		a2 = 1 - alpha;
		break;
	default:
		b0 = b1 = b2 = a0 = a1 = a2 = 0;
		b0_a0 = b1_a0 = b2_a0 = a1_a0 = a2_a0 = 0;
		return;
	}

	b0_a0 = b0 / a0;
	b1_a0 = b1 / a0;
	b2_a0 = b2 / a0;
	a1_a0 = a1 / a0;
	a2_a0 = a2 / a0;
}
