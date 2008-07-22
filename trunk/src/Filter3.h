/***************************************************************************
 *   Copyright (C) 2007 by Deryabin Andrew                                 *
 *   andrewderyabin@gmail.com                                              *
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
class Filter3;

enum FilterType
{
    LPF = 0,
    HPF,
    BPF
};

struct FilterOpts
{
    unsigned long Fs; //sample rate
    double f0; //center freq
    double bw; //bandwidth
    FilterType type; //filter type
};

class Filter3
{
public:
    Filter3();
    ~Filter3();
    void Init(const FilterOpts *opts);

    inline void Process3(double &s0, double &s1, double &s2)
    {
        _s0 = s0;
        _s1 = s1;
        _s2 = s2;

        s0 = b0_a0 * s0 + b1_a0 * in_delay01 + b2_a0 * in_delay00 - a1_a0 * out_delay01 - a2_a0 * out_delay00;
        s1 = b0_a0 * s1 + b1_a0 * in_delay11 + b2_a0 * in_delay10 - a1_a0 * out_delay11 - a2_a0 * out_delay10;
        s2 = b0_a0 * s2 + b1_a0 * in_delay21 + b2_a0 * in_delay20 - a1_a0 * out_delay21 - a2_a0 * out_delay20;

        in_delay00 = in_delay01;
        in_delay10 = in_delay11;
        in_delay20 = in_delay21;
        out_delay00 = out_delay01;
        out_delay10 = out_delay11;
        out_delay20 = out_delay21;

        in_delay01 = _s0;
        in_delay11 = _s1;
        in_delay21 = _s2;
        out_delay01 = s0;
        out_delay11 = s1;
        out_delay21 = s2;
    }
    inline void Process(double &s0)
    {
        _s0 = s0;
        s0 = b0_a0 * s0 + b1_a0 * in_delay01 + b2_a0 * in_delay00 - a1_a0 * out_delay01 - a2_a0 * out_delay00;
        in_delay00 = in_delay01;
        out_delay00 = out_delay01;
        in_delay01 = _s0;
        out_delay01 = s0;        
    }
private:
    double _s0, _s1, _s2;
    double a0, a1, a2;
    double b0, b1, b2;
    double b0_a0, b1_a0, b2_a0;
    double a1_a0, a2_a0;
    double in_delay00, in_delay01, in_delay10, in_delay11, in_delay20, in_delay21;
    double out_delay00, out_delay01, out_delay10, out_delay11, out_delay20, out_delay21;
    Filter3 *next;
};
