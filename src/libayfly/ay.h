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
    AY_CHNL_A_FINE = 0,
    AY_CHNL_A_COARSE,
    AY_CHNL_B_FINE,
    AY_CHNL_B_COARSE,
    AY_CHNL_C_FINE,
    AY_CHNL_C_COARSE,
    AY_NOISE_PERIOD,
    AY_MIXER,
    AY_CHNL_A_VOL,
    AY_CHNL_B_VOL,
    AY_CHNL_C_VOL,
    AY_ENV_FINE,
    AY_ENV_COARSE,
    AY_ENV_SHAPE,
    AY_GPIO_A,
    AY_GPIO_B
};

class ay
{
public:
    ay();
    ~ay();
    void ayReset();
    void ayWrite(unsigned char reg, unsigned char val);
    unsigned char ayRead(unsigned char reg);
    unsigned long ayProcess(unsigned char *stream, unsigned long len);
    unsigned long ayProcessMono(unsigned char *stream, unsigned long len);
    unsigned long ayProcessTS(unsigned char *stream, unsigned long len);
    unsigned long ayProcessTSMono(unsigned char *stream, unsigned long len);
    inline void chnlMute(unsigned long chnl, bool mute)
    {
        chnl_mute[chnl] = !mute;
    }
    ;
    inline bool chnlMuted(unsigned long chnl)
    {
        return chnl_mute[chnl];
    }
    ;
    inline float GetVolume(unsigned long chnl)
    {
        return volume[chnl];
    }
    ;
    inline void SetVolume(unsigned long chnl, float new_volume)
    {
        volume[chnl] = new_volume > 1 ? 1 : (new_volume < 0 ? 0 : new_volume);
    }
    ;
    inline const unsigned char *GetRegs()
    {
        return regs;
    };

    void SetParameters(AYSongInfo *_songinfo = 0);
private:
    static const float init_levels_ay[32];
    static const float init_levels_ym[32];
    float levels_ay[32];
    float levels_ym[32];
    float *levels;
    unsigned char regs[16];
    long noise_period;
    long chnl_period[3];
    float chnl_vol[3];
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
    unsigned long env_type_old;
    long env_tick;
    unsigned long env_vol;
    long env_trigger;
    bool chnl_mute[3];
    unsigned long ay_tacts;
    void setEnvelope();
    void updateEnvelope();
    float volume[3];
    AYSongInfo *songinfo;
    unsigned long int_counter;
    unsigned long int_limit;
    long z80_per_sample;
    long int_per_z80;
    long z80_per_sample_counter;
    long int_per_z80_counter;
    void aySoftStep(float &s0, float &s1, float &s2);
    void ayZ80Step(float &s0, float &s1, float &s2);
    void ayCommonStep(float &s0, float &s1, float &s2);
    void ayStep(float &s0, float &s1, float &s2);
    typedef void (ay::*stepfunc)(float &s0, float &s1, float &s2);
    ay::stepfunc Step;
};

#endif /*AY_H_*/

