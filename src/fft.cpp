/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew   							   *
 *   andrewderyabin@gmail.com   										   *
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

Cfft::Cfft(unsigned short _Points, double _SampleRate)
{
	Points = _Points;
	SampleRate = _SampleRate;
	Data = new fft_type [Points];
	Buffer = new fft_type [Points];
	SRLut = new double [Points / 2];
	divider = sqrt((double)_Points);

	double step = SampleRate / Points;
	for (unsigned short i = 0; i < Points / 2; i++)
	{
		Data [i] = fft_type(0, 0);
		Buffer [i] = fft_type(0, 0);
		SRLut [i] = (double)i * step;
	}

	Stages = (unsigned short)(log((double)Points)/log((double)2));
	printf("Points=%d,Stages=%d\n", Points, Stages);
	ExpLut = new fft_type [Stages + 1];

	for (unsigned short i = 1; i <= Stages; i++)
	{
		double le = ((1 << i) >> 1);
		double re = cos(PI / le);
		double im = -sin(PI / le);
		ExpLut [i] = fft_type(re, im);
	}

	BRLut = new unsigned short [Points];
	memset(BRLut, 0, Points * sizeof(unsigned short));
	unsigned short pt = Points;
	unsigned short bits = 0;

	while (!(pt & 1))
	{
		bits++;
		pt >>= 1;
	}

	unsigned short mask = 1;

	for (unsigned short i = 0; i < Points; i++)
	{
		pt = i;
		unsigned short rindex = 0;
		unsigned short tbits = bits;
		while (tbits)
		{
			if (pt & 1)
				rindex |= mask;

			rindex <<= 1;
			pt >>= 1;
			tbits--;
		}

		rindex >>= 1;
		BRLut [i] = rindex;
	}

}

Cfft::~Cfft()
{
	if (Data)
		delete [] Data;
	if (Buffer)
		delete [] Buffer;
	if (SRLut)
		delete [] SRLut;
}

void Cfft::SetSamples(float *buffer, unsigned short _Points)
{
	unsigned short limit = _Points ? (_Points <= Points ? _Points : Points) : Points;
	for (unsigned short i = 0; i < limit; i++)
	{
		Data [i] = fft_type(buffer [i], 0);
	}
	for (unsigned short i = limit; i < Points; i++)
		Data [i] = 0;
}

void Cfft::SetSamples(double *buffer, unsigned short _Points)
{
	//MessageBoxA(0, "!!!", "!!!!", MB_OK);
	unsigned short limit = _Points ? (_Points <= Points ? _Points : Points) : Points;	
	for (unsigned short i = 0; i < limit; i++)
	{
		Data [i] = fft_type(buffer [i], 0);
	}
	for (unsigned short i = limit; i < Points; i++)
		Data [i] = 0;
}

void Cfft::SetSamples(fft_type *buffer, unsigned short _Points)
{
	unsigned short limit = _Points ? (_Points <= Points ? _Points : Points) : Points;
	for (unsigned short i = 0; i < limit; i++)
	{
		Data [i] = buffer [i];
	}
	for (unsigned short i = limit; i < Points; i++)
		Data [i] = 0;
}

void Cfft::GetSamples(float *buffer, unsigned short _Points)
{
	unsigned short limit = _Points ? (_Points <= Points ? _Points : Points) : Points;
	for (unsigned short i = 0; i < limit; i++)
	{
		buffer [i] = Data [i].real();
	}
}

void Cfft::SetSample(float val, unsigned short i)
{
	Data [i] = fft_type(val, 0);
}

void Cfft::GetSamples(double *buffer, unsigned short _Points)
{
	unsigned short limit = _Points ? (_Points <= Points ? _Points : Points) : Points;
	for (unsigned short i = 0; i < limit; i++)
	{
		buffer [i] = Data [i].real();
	}
}

void Cfft::GetSamples(fft_type *buffer, unsigned short _Points)
{
	unsigned short limit = _Points ? (_Points <= Points ? _Points : Points) : Points;
	for (unsigned short i = 0; i < limit; i++)
	{
		buffer [i] = Data [i];
	}
}

fft_type *Cfft::GetSamples()
{
	return Data;
}

void Cfft::Sort()
{
	for (unsigned short i = 1; i < Points; i++)
	{
		Buffer [i] = Data [i];
	}
	for (unsigned short i = 1; i < Points; i++)
	{
		Data [i] = Buffer [BRLut [i]];
	}

}

void Cfft::DoFFT()
{
	Sort();
	fft_type t;

	for (unsigned short i = 1; i <= Stages; i++)
	{

		unsigned short le = (1 << i);
		unsigned short le2 = le >> 1;
		fft_type exp = ExpLut [i];
		fft_type u = 1;
		for (unsigned short j = 0; j < le2; j++)
		{
			for (unsigned short l = j; l < Points; l += le)
			{
				unsigned short ip = l + le2;
				t = u * Data [ip];
				Data [ip] = Data [l] - t;
				Data [l] += t;
			}
			u *= exp;
		}

	}

	for (unsigned short i = 0; i < Points; i++)
	{
		Data [i] /= divider;
	}
}

void Cfft::DoInverseFFT()
{
	for (unsigned short i = 0; i < Points; i++)
	{
		Data [i] = conj(Data [i]);
	}
	DoFFT();
	for (unsigned short i = 0; i < Points; i++)
	{
		Data [i] = conj(Data [i]);
	}
}
