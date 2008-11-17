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

#include "ayfly.h"
#ifndef __SYMBIAN32__
#include "samplerate.h"
#endif

#define TACTS_MULT (float)800

const float ay::init_levels_ay[] =
{
0, 836, 1212, 1773, 2619, 3875, 5397, 8823, 10392, 16706, 23339, 29292, 36969, 46421, 55195, 65535
};

const float ay::init_levels_ym[] =
{
0, 0, 0xF8, 0x1C2, 0x29E, 0x33A, 0x3F2, 0x4D7, 0x610, 0x77F, 0x90A, 0xA42, 0xC3B, 0xEC2, 0x1137, 0x13A7, 0x1750, 0x1BF9, 0x20DF, 0x2596, 0x2C9D, 0x3579, 0x3E55, 0x4768, 0x54FF, 0x6624, 0x773B, 0x883F, 0xA1DA, 0xC0FC, 0xE094, 0xFFFF
};
#define TONE_ENABLE(ch) ((regs [AY_MIXER] >> (ch)) & 1)
#define NOISE_ENABLE(ch) ((regs [AY_MIXER] >> (3 + (ch))) & 1)
#define TONE_PERIOD(ch) (((((regs [((ch) << 1) + 1]) & 0xf) << 8)) | (regs [(ch) << 1]))
#define NOISE_PERIOD (regs [AY_NOISE_PERIOD] & 0x1f)
#define CHNL_VOLUME(ch) (regs [AY_CHNL_A_VOL + (ch)] & 0xf)
#define CHNL_ENVELOPE(ch) (regs [AY_CHNL_A_VOL + (ch)] & 0x10)
#define ENVELOPE_PERIOD (((regs [AY_ENV_COARSE]) << 8) | regs [AY_ENV_FINE])

ay::ay()
{
#ifndef __SYMBIAN32__
    for (unsigned long i = 0; i < sizeof_array(ay::levels_ay); i++)
    {
        ay::levels_ay[i] = (ay::init_levels_ay[i / 2] / (float)65536) / (float)1.5;
        ay::levels_ym[i] = (ay::init_levels_ym[i] / (float)65536) / (float)1.5;
    }   

    src_state = 0;
    src_remaining = 0;
#else
    for (unsigned long i = 0; i < sizeof_array(ay::levels_ay); i++)
        {
            ay::levels_ay[i] = (ay::init_levels_ay[i / 2]) / (float)4;
            ay::levels_ym[i] = (ay::init_levels_ym[i]) / (float)4;
        }
#endif
    
    songinfo = 0;
    ayReset();
}

ay::~ay()
{
#ifndef __SYMBIAN32__
    if(src_state)
    src_delete((SRC_STATE *)src_state);
#endif
}

void ay::SetParameters(AYSongInfo *_songinfo)
{
    if (_songinfo == 0 && songinfo == 0)
        return;
    if ((_songinfo != songinfo) && (_songinfo != 0))
        songinfo = _songinfo;
    if (songinfo->sr == 0 || songinfo->int_freq == 0)
        return;
#ifndef __SYMBIAN32__
    ay_tacts_f = ((double)songinfo->ay_freq) / (double)songinfo->sr / (double)8;
#else
    ay_tacts_f = ((double)songinfo->ay_freq * TACTS_MULT) / (double)songinfo->sr / (double)8;
#endif
    ay_tacts = ay_tacts_f;
    if ((ay_tacts_f - ay_tacts) >= 0.5)
        ay_tacts++;
    levels = songinfo->chip_type == 0 ? ay::levels_ay : ay::levels_ym;
#ifndef __SYMBIAN32__
    src_ratio = (double)songinfo->sr / ((double)songinfo->ay_freq / (double)8);    
    float int_limit_f = ((float)songinfo->sr * TACTS_MULT) / (float)songinfo->int_freq * ay_tacts_f;
#else
    float int_limit_f = ((float)songinfo->sr * TACTS_MULT) / (float)songinfo->int_freq;
#endif
    int_limit = int_limit_f;
    if (int_limit_f - int_limit >= 0.5)
        int_limit++;
    if (songinfo->is_z80)
    {
#ifndef __SYMBIAN32__
        float z80_per_sample_f = ((float)songinfo->z80_freq * TACTS_MULT) / songinfo->sr / ay_tacts_f;
#else
        float z80_per_sample_f = ((float)songinfo->z80_freq * TACTS_MULT) / songinfo->sr;
#endif
        z80_per_sample = z80_per_sample_f;
        if (z80_per_sample_f - z80_per_sample)
            z80_per_sample++;
        float int_per_z80_f = ((float)songinfo->z80_freq * TACTS_MULT) / (float)songinfo->int_freq;
        int_per_z80 = int_per_z80_f;
        if ((int_per_z80_f - int_per_z80) >= 0.5)
            int_per_z80++;

    }
#ifndef __SYMBIAN32__
    int src_err = 0;
    if(!src_state)
    src_state = src_new(SRC_SINC_FASTEST, 2, &src_err);
#endif
}

void ay::ayReset()
{
    //init regs with defaults
    int_limit = 0;
    int_counter = 0;
    z80_per_sample_counter = 0;
    int_per_z80_counter = 0;
    memset(regs, 0, sizeof(regs));
    regs[AY_GPIO_A] = regs[AY_GPIO_B] = 0xff;
    chnl_period[0] = chnl_period[1] = chnl_period[2] = 0;
    chnl_vol[0] = 0;
    chnl_vol[1] = 0;
    chnl_vol[2] = 0;
    chnl_enable[0] = false;
    chnl_enable[1] = false;
    chnl_enable[2] = false;
    noise_enable[0] = false;
    noise_enable[1] = false;
    noise_enable[2] = false;
    tone_period_init[0] = tone_period_init[1] = tone_period_init[2] = 1;
    chnl_mute[0] = chnl_mute[1] = chnl_mute[2] = false;
    env_type = 0;
    env_vol = 0;
    chnl_trigger[0] = chnl_trigger[1] = chnl_trigger[2] = 1;
    noise_reg = 0x1;
    noise_trigger = 1;
    noise_period = 0;

    volume[0] = volume[1] = volume[2] = 1;
    env_type_old = -1;
    env_step = 0;
    ay_tacts_counter = 0;

    SetParameters(0);
    setEnvelope();

}

void ay::ayWrite(unsigned char reg, unsigned char val)
{
    regs[reg & 0xf] = val;
    switch (reg)
    {
    case AY_CHNL_A_COARSE:
    case AY_CHNL_A_FINE:
        tone_period_init[0] = TONE_PERIOD(0);
        if (!tone_period_init[0])
            tone_period_init[0] = 4096;
        break;
    case AY_CHNL_B_COARSE:
    case AY_CHNL_B_FINE:
        tone_period_init[1] = TONE_PERIOD(1);
        if (!tone_period_init[1])
            tone_period_init[1] = 4096;
        break;
    case AY_CHNL_C_COARSE:
    case AY_CHNL_C_FINE:
        tone_period_init[2] = TONE_PERIOD(2);
        if (!tone_period_init[2])
            tone_period_init[2] = 4096;
        break;
    case AY_NOISE_PERIOD:
        noise_period_init = NOISE_PERIOD * 2;
        break;
    case AY_MIXER:
        break;
    case AY_CHNL_A_VOL:
    case AY_CHNL_B_VOL:
    case AY_CHNL_C_VOL:
        break;
    case AY_ENV_SHAPE:
        setEnvelope();
        break;
    case AY_ENV_FINE:
    case AY_ENV_COARSE:
        env_period_init = ENVELOPE_PERIOD;
        break;
    default:
        break;
    }
}

unsigned char ay::ayRead(unsigned char reg)
{
    reg &= 0xf;
    return regs[reg];
}

void ay::setEnvelope()
{
    env_type = regs[AY_ENV_SHAPE];
    switch (env_type)
    {
    case 0:
        env_step = 0;
        env_vol = 31;
        break;
    case 4:
        env_step = 1;
        env_vol = 0;
        break;
    case 8:
        env_step = 2;
        env_vol = 31;
        break;
    case 9:
        env_step = 0;
        env_vol = 31;
        break;
    case 10:
        env_step = 3;
        env_vol = 31;
        break;
    case 11:
        env_step = 4;
        env_vol = 31;
        break;
    case 12:
        env_step = 5;
        env_vol = 0;
        break;
    case 13:
        env_step = 6;
        env_vol = 0;
        break;
    case 14:
        env_step = 7;
        env_vol = 0;
        break;
    case 15:
        env_step = 1;
        env_vol = 0;
        break;
    default:
        env_step = 8;
        env_vol = 0;
        break;
    }

}

inline void ay::updateEnvelope()
{
    env_period++;
    if (env_period >= env_period_init)
    {
        env_period = 0;
        switch (env_step)
        {
        case 0:
            if (--env_vol == 0)
                env_step = 8;
            break;
        case 1:
            if (++env_vol == 32)
            {
                env_vol = 0;
                env_step = 8;
            }
            break;
        case 2:
            if (--env_vol == -1)
                env_vol = 31;
            break;
        case 3:
            if (--env_vol == -1)
            {
                env_vol = 0;
                env_step = 7;
            }
            break;
        case 4:
            if (--env_vol == -1)
            {
                env_vol = 31;
                env_step = 8;
            }
            break;
        case 5:
            if (++env_vol == 32)
                env_vol = 0;
            break;
        case 6:
            if (++env_vol == 31)
                env_step = 8;
            break;
        case 7:
            if (++env_vol == 32)
            {
                env_vol = 31;
                env_step = 3;
            }
            break;
        default:
            break;

        }
    }
}

inline void ay::ayCommonStep(float &s0, float &s1, float &s2)
{

    if (songinfo->is_z80)
    {
        while (z80_per_sample_counter < z80_per_sample)
        {
            int tstates = z80ex_step(songinfo->z80ctx) * TACTS_MULT;
            z80_per_sample_counter += tstates;
            int_per_z80_counter += tstates;
            if (int_per_z80_counter> int_per_z80)
            {
                tstates = z80ex_int(songinfo->z80ctx) * TACTS_MULT;
                z80_per_sample_counter += tstates;
                int_per_z80_counter = tstates;
                if (++songinfo->timeElapsed >= songinfo->Length)
                {
                    songinfo->timeElapsed = songinfo->Loop;
                    if (songinfo->e_callback)
                        songinfo->stopping = songinfo->e_callback(songinfo->e_callback_arg);
                }
            }

        }
        z80_per_sample_counter -= z80_per_sample;
    }
    else
    {
        int_counter += TACTS_MULT;
        if (int_counter > int_limit)
        {
            int_counter -= int_limit;
            ay_softexec(songinfo);
        }
    }
    volume_divider = 0;
    while (ay_tacts_counter < ay_tacts)
    {
        ay_tacts_counter += TACTS_MULT;
        volume_divider += 1;
        if (++chnl_period[0] >= tone_period_init[0])
        {
            chnl_period[0] -= tone_period_init[0];
            chnl_trigger[0] ^= 1;
        }
        if (++chnl_period[1] >= tone_period_init[1])
        {
            chnl_period[1] -= tone_period_init[1];
            chnl_trigger[1] ^= 1;
        }
        if (++chnl_period[2] >= tone_period_init[2])
        {
            chnl_period[2] -= tone_period_init[2];
            chnl_trigger[2] ^= 1;
        }

        if (++noise_period >= noise_period_init)
        {
            noise_period = 0;
            if ((noise_reg + 1) & 2)
                noise_trigger ^= 1;
            if (noise_reg & 1)
                noise_reg ^= 0x24000;
            noise_reg >>= 1;
        }
        updateEnvelope();

        if ((chnl_trigger[0] | TONE_ENABLE(0)) & (noise_trigger | NOISE_ENABLE(0)) & !chnl_mute[0])
            s0 += (CHNL_ENVELOPE(0) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(0) * 2]) * volume[0];
        if ((chnl_trigger[1] | TONE_ENABLE(1)) & (noise_trigger | NOISE_ENABLE(1)) & !chnl_mute[1])
            s1 += (CHNL_ENVELOPE(1) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(1) * 2]) * volume[1];
        if ((chnl_trigger[2] | TONE_ENABLE(2)) & (noise_trigger | NOISE_ENABLE(2)) & !chnl_mute[2])
            s2 += (CHNL_ENVELOPE(2) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(2) * 2]) * volume[2];
    }
    ay_tacts_counter -= ay_tacts;

    s0 = s0 / volume_divider;
    s1 = (s1 / volume_divider) / 1.50;
    s2 = s2 / volume_divider;
}

#ifndef __SYMBIAN32__
inline void ay::ayStep(float &s0, float &s1, float &s2)
{
    s0 = s1 = s2 = 0;
    if (++chnl_period[0] >= tone_period_init[0])
    {
        chnl_period[0] -= tone_period_init[0];
        chnl_trigger[0] ^= 1;
    }
    if (++chnl_period[1] >= tone_period_init[1])
    {
        chnl_period[1] -= tone_period_init[1];
        chnl_trigger[1] ^= 1;
    }
    if (++chnl_period[2] >= tone_period_init[2])
    {
        chnl_period[2] -= tone_period_init[2];
        chnl_trigger[2] ^= 1;
    }

    if (++noise_period >= noise_period_init)
    {
        noise_period = 0;
        if ((noise_reg + 1) & 2)
            noise_trigger ^= 1;
        if (noise_reg & 1)
            noise_reg ^= 0x24000;
        noise_reg >>= 1;
    }
    updateEnvelope();

    if ((chnl_trigger[0] | TONE_ENABLE(0)) & (noise_trigger | NOISE_ENABLE(0)) & !chnl_mute[0])
        s0 = (CHNL_ENVELOPE(0) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(0) * 2]) * volume[0];
    if ((chnl_trigger[1] | TONE_ENABLE(1)) & (noise_trigger | NOISE_ENABLE(1)) & !chnl_mute[1])
        s1 = (CHNL_ENVELOPE(1) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(1) * 2]) * volume[1];
    if ((chnl_trigger[2] | TONE_ENABLE(2)) & (noise_trigger | NOISE_ENABLE(2)) & !chnl_mute[2])
        s2 = (CHNL_ENVELOPE(2) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(2) * 2]) * volume[2];
    s1 /= 1.50;

}

unsigned long ay::ayProcess(unsigned char *stream, unsigned long len)
{
    unsigned long to_process = (len >> 1);
    float s0, s1, s2;
    unsigned long p = 0;
    if(src_remaining> 0)
    {
        short *stream16 = (short *)stream;
        src_float_to_short_array(ay_temp_buffer_out, stream16, src_remaining);
        to_process -= src_remaining;
        p += src_remaining;
    }
    src_remaining = 0;
    SRC_DATA src_data;
    while(to_process> 0)
    {
        unsigned long i;
        for(i = 0; i < AY_TEMP_BUFFER_SIZE; i += 2)
        {
            s0 = s1 = s2 = 0;
            if(songinfo->is_z80)
            {
                while(z80_per_sample_counter < z80_per_sample)
                {
                    int tstates = z80ex_step(songinfo->z80ctx) * TACTS_MULT;
                    z80_per_sample_counter += tstates;
                    int_per_z80_counter += tstates;
                    if(int_per_z80_counter> int_per_z80)
                    {
                        tstates = z80ex_int(songinfo->z80ctx) * TACTS_MULT;
                        z80_per_sample_counter += tstates;
                        int_per_z80_counter = tstates;
                        if(++songinfo->timeElapsed >= songinfo->Length)
                        {
                            songinfo->timeElapsed = songinfo->Loop;
                            if(songinfo->e_callback)
                            songinfo->stopping = songinfo->e_callback(songinfo->e_callback_arg);
                        }
                    }

                }
                z80_per_sample_counter -= z80_per_sample;
            }
            else
            {
                int_counter += TACTS_MULT;
                if(int_counter> int_limit)
                {
                    int_counter -= int_limit;
                    ay_softexec(songinfo);
                }
            }
            if(++chnl_period[0] >= tone_period_init[0])
            {
                chnl_period[0] -= tone_period_init[0];
                chnl_trigger[0] ^= 1;
            }
            if(++chnl_period[1] >= tone_period_init[1])
            {
                chnl_period[1] -= tone_period_init[1];
                chnl_trigger[1] ^= 1;
            }
            if(++chnl_period[2] >= tone_period_init[2])
            {
                chnl_period[2] -= tone_period_init[2];
                chnl_trigger[2] ^= 1;
            }

            if(++noise_period >= noise_period_init)
            {
                noise_period = 0;
                noise_reg = (noise_reg >> 1) ^ ((noise_reg & 1) ? 0x14000 : 0);
                noise_trigger = noise_reg & 1;
            }
            updateEnvelope();

            if((chnl_trigger[0] | TONE_ENABLE(0)) & (noise_trigger | NOISE_ENABLE(0)) & !chnl_mute[0])
            s0 = (CHNL_ENVELOPE(0) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(0) * 2]) * volume[0];
            if((chnl_trigger[1] | TONE_ENABLE(1)) & (noise_trigger | NOISE_ENABLE(1)) & !chnl_mute[1])
            s1 = (CHNL_ENVELOPE(1) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(1) * 2]) * volume[1];
            if((chnl_trigger[2] | TONE_ENABLE(2)) & (noise_trigger | NOISE_ENABLE(2)) & !chnl_mute[2])
            s2 = (CHNL_ENVELOPE(2) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(2) * 2]) * volume[2];
            s1 /= 1.50;

            if(songinfo->is_ts)
            {
                float s3, s4, s5;
                songinfo->ay8910 [1].ayStep(s3, s4, s5);
                s0 = (s0 + s3) / 2;
                s1 = (s1 + s4) / 2;
                s2 = (s2 + s5) / 2;
            }

            ay_temp_buffer_in[i] = s0 + s1;
            ay_temp_buffer_in[i + 1] = s2 + s1;
            if(songinfo->stopping)
            break;

        }
        src_data.data_in = ay_temp_buffer_in;
        src_data.data_out = ay_temp_buffer_out;
        src_data.input_frames = i / 2;
        src_data.output_frames = AY_TEMP_BUFFER_SIZE / 2;
        src_data.output_frames_gen = 0;
        src_data.src_ratio = src_ratio;
        src_data.end_of_input = 0;
        src_data.input_frames_used = 0;

        src_process((SRC_STATE *)src_state, &src_data);
        unsigned long remaining = src_data.output_frames_gen * 2> to_process ? to_process : src_data.output_frames_gen * 2;

        short *stream16 = (short *)stream;
        stream16 += p;
        src_float_to_short_array(ay_temp_buffer_out, stream16, remaining);
        p += remaining;
        to_process -= remaining;
        if(songinfo->stopping)
        return len - (to_process << 1);
        src_remaining = src_data.output_frames_gen * 2 - remaining;
    }
    if(src_remaining> 0)
    {
        memcpy(ay_temp_buffer_out, ay_temp_buffer_out + src_data.output_frames_gen * 2 - src_remaining, src_remaining * sizeof(float));
    }
    return len;
}
#else

inline void ay::ayStep(float &s0, float &s1, float &s2)
{
    volume_divider = 0;
    while(ay_tacts_counter < ay_tacts)
    {
        ay_tacts_counter += TACTS_MULT;
        volume_divider += 1;
        if(++chnl_period[0] >= tone_period_init[0])
        {
            chnl_period[0] -= tone_period_init[0];
            chnl_trigger[0] ^= 1;
        }
        if(++chnl_period[1] >= tone_period_init[1])
        {
            chnl_period[1] -= tone_period_init[1];
            chnl_trigger[1] ^= 1;
        }
        if(++chnl_period[2] >= tone_period_init[2])
        {
            chnl_period[2] -= tone_period_init[2];
            chnl_trigger[2] ^= 1;
        }

        if(++noise_period >= noise_period_init)
        {
            noise_period = 0;
            if((noise_reg + 1) & 2)
                noise_trigger ^= 1;
            if(noise_reg & 1)
                noise_reg ^= 0x24000;
            noise_reg >>= 1;
        }
        updateEnvelope();

        if((chnl_trigger[0] | TONE_ENABLE(0)) & (noise_trigger | NOISE_ENABLE(0)) & !chnl_mute[0])
            s0 += (CHNL_ENVELOPE(0) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(0) * 2]) * volume[0];
        if((chnl_trigger[1] | TONE_ENABLE(1)) & (noise_trigger | NOISE_ENABLE(1)) & !chnl_mute[1])
            s1 += (CHNL_ENVELOPE(1) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(1) * 2]) * volume[1];
        if((chnl_trigger[2] | TONE_ENABLE(2)) & (noise_trigger | NOISE_ENABLE(2)) & !chnl_mute[2])
            s2 += (CHNL_ENVELOPE(2) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(2) * 2]) * volume[2];
    }

    ay_tacts_counter -= ay_tacts;

    s0 = s0 / volume_divider;
    s1 = (s1 / volume_divider) / 1.5;
    s2 = s2 / volume_divider;
}

unsigned long ay::ayProcess(unsigned char *stream, unsigned long len)
{
    unsigned long work_len = (len >> 2);
    float s0, s1, s2;
    short *stream16 = (short *)stream;
    for (unsigned long i = 0; i < work_len; i++)
    {
        s0 = s1 = s2 = 0;
        if (songinfo->stopping == false)
        {
            if (songinfo->is_z80)
            {
                while (z80_per_sample_counter < z80_per_sample)
                {
                    int tstates = z80ex_step(songinfo->z80ctx) * TACTS_MULT;
                    z80_per_sample_counter += tstates;
                    int_per_z80_counter += tstates;
                    if (int_per_z80_counter> int_per_z80)
                    {
                        tstates = z80ex_int(songinfo->z80ctx) * TACTS_MULT;
                        z80_per_sample_counter += tstates;
                        int_per_z80_counter = tstates;
                        if (++songinfo->timeElapsed >= songinfo->Length)
                        {
                            songinfo->timeElapsed = songinfo->Loop;
                            if (songinfo->e_callback)
                                songinfo->stopping = songinfo->e_callback(songinfo->e_callback_arg);
                        }
                    }

                }
                z80_per_sample_counter -= z80_per_sample;
            }
            else
            {
                int_counter += TACTS_MULT;
                if (int_counter > int_limit)
                {
                    int_counter -= int_limit;
                    ay_softexec(songinfo);
                }
            }
            volume_divider = 0;
            while (ay_tacts_counter < ay_tacts)
            {
                ay_tacts_counter += TACTS_MULT;
                volume_divider += 1;
                if (++chnl_period[0] >= tone_period_init[0])
                {
                    chnl_period[0] -= tone_period_init[0];
                    chnl_trigger[0] ^= 1;
                }
                if (++chnl_period[1] >= tone_period_init[1])
                {
                    chnl_period[1] -= tone_period_init[1];
                    chnl_trigger[1] ^= 1;
                }
                if (++chnl_period[2] >= tone_period_init[2])
                {
                    chnl_period[2] -= tone_period_init[2];
                    chnl_trigger[2] ^= 1;
                }

                if (++noise_period >= noise_period_init)
                {
                    noise_period = 0;
                    if ((noise_reg + 1) & 2)
                        noise_trigger ^= 1;
                    if (noise_reg & 1)
                        noise_reg ^= 0x24000;
                    noise_reg >>= 1;
                }
                updateEnvelope();

                if ((chnl_trigger[0] | TONE_ENABLE(0)) & (noise_trigger | NOISE_ENABLE(0)) & !chnl_mute[0])
                    s0 += (CHNL_ENVELOPE(0) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(0) * 2]) * volume[0];
                if ((chnl_trigger[1] | TONE_ENABLE(1)) & (noise_trigger | NOISE_ENABLE(1)) & !chnl_mute[1])
                    s1 += (CHNL_ENVELOPE(1) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(1) * 2]) * volume[1];
                if ((chnl_trigger[2] | TONE_ENABLE(2)) & (noise_trigger | NOISE_ENABLE(2)) & !chnl_mute[2])
                    s2 += (CHNL_ENVELOPE(2) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(2) * 2]) * volume[2];
            }
            ay_tacts_counter -= ay_tacts;
            if (songinfo->is_ts)
            {
                float s3, s4, s5;
                songinfo->ay8910 [1].ayStep(s3, s4, s5);
                s0 = (s0 + s3) / 2;
                s1 = (s1 + s4) / 2;
                s2 = (s2 + s5) / 2;
            }

            s0 = s0 / volume_divider;
            s1 = (s1 / volume_divider) / 1.50;
            s2 = s2 / volume_divider;

        }
        else
            return (i << 2);
        stream16[i * 2] = s0 + s1;
        stream16[i * 2 + 1] = s2 + s1;
    }
    return len;
}
#endif
