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

#ifndef AY_H_
#define AY_H_

enum
{
    AY_CHNL_A_FINE = 0, AY_CHNL_A_COARSE, AY_CHNL_B_FINE, AY_CHNL_B_COARSE, AY_CHNL_C_FINE, AY_CHNL_C_COARSE, AY_NOISE_PERIOD, AY_MIXER, AY_CHNL_A_VOL, AY_CHNL_B_VOL, AY_CHNL_C_VOL, AY_ENV_FINE, AY_ENV_COARSE, AY_ENV_SHAPE, AY_GPIO_A, AY_GPIO_B
};

class ay
{
public:
    ay(long _ay_freq, int _buf_sz);
    ~ay();
    void ayReset();
    void ayWrite(unsigned char reg, unsigned char val);
    unsigned char ayRead(unsigned char reg);
    void ayProcess(unsigned char *stream, int len);
    inline bool chnlToggle(unsigned long chnl)
    {
        chnl_mute[chnl] = !chnl_mute[chnl];
        return chnl_mute[chnl];
    }
    ;
    inline bool chnlMuted(unsigned long chnl)
    {
        return chnl_mute[chnl];
    }
    ;
    inline unsigned long GetVolume(unsigned long chnl)
    {
        return volume[chnl];
    }
    ;
    inline void SetVolume(unsigned long chnl, unsigned long new_volume)
    {
        volume[chnl] = new_volume;
    }
    ;

    virtual void SetCallback(ELAPSED_CALLBACK _callback, void *_arg);
    void SetBufferSize(int _buf_sz);
private:
    static unsigned long init_levels[16];
    unsigned long levels[16];
    long ay_freq;
    unsigned long ay_tone_freq;
    unsigned long ay_env_freq;
    unsigned char regs[16];
    long noise_period;
    long chnl_period[3];
    unsigned long chnl_vol[3];
    bool chnl_enable[3];
    long tone_period_init[3];
    long noise_period_init;
    long chnl_trigger[3];
    bool noise_enable[3];
    unsigned long noise_trigger;
    unsigned long noise_reg;
    long env_period_init;
    long env_period;
    unsigned long env_type;
    unsigned long env_tick;
    unsigned long env_vol;
    long env_trigger;
    unsigned long *buffer[3];
    unsigned long *buffer_tail[3];
    unsigned long tail_len;
    bool chnl_mute[3];
    unsigned long q_len;
    unsigned long half_len;
    FILE *fd;
    int buf_sz;
    unsigned long tails_len;
    unsigned long ay_tacts;
    Filter3 *flt;
    Filter3 *flt_bass;
    void setEnvelope();
    void updateEnvelope();
    unsigned long volume[3];
    ELAPSED_CALLBACK elapsedCallback;
    void *elapsedCallbackArg;
};

#endif /*AY_H_*/


