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

const float ay::init_levels[] =
{ 0x0000, 0x0385, 0x053D, 0x0770, 0x0AD7, 0x0FD5, 0x15B0, 0x230C, 0x2B4C, 0x43C1, 0x5A4B, 0x732F, 0x9204, 0xAFF1, 0xD921, 0xFFFF };

#define TONE_ENABLE(ch) ((regs [AY_MIXER] >> (ch)) & 1)
#define NOISE_ENABLE(ch) ((regs [AY_MIXER] >> (3 + (ch))) & 1)
#define TONE_PERIOD(ch) (((((regs [((ch) << 1) + 1]) & 0xf) << 8)) | (regs [(ch) << 1]))
#define NOISE_PERIOD (regs [AY_NOISE_PERIOD] & 0x1f)
#define CHNL_VOLUME(ch) (regs [AY_CHNL_A_VOL + (ch)] & 0xf)
#define CHNL_ENVELOPE(ch) (regs [AY_CHNL_A_VOL + (ch)] & 0x10)
#define ENVELOPE_PERIOD (((regs [AY_ENV_COARSE]) << 8) | regs [AY_ENV_FINE])

#define AY_PROCESS_INIT \
    unsigned long work_len = (len >> 2);\
    float s0, s1, s2;\
    short *stream16 = (short *) stream;

#define AY_PROCESS_STEP \
    if(++int_counter > int_limit)\
    {\
        int_counter = 0;\
        ay_z80xec(songinfo);\
    }\
\
    s0 = s1 = s2 = 0;\
\
    for(unsigned long k = 0; k < ay_tacts; k++)\
    {\
\
        if(++chnl_period[0] >= tone_period_init[0])\
        {\
            chnl_period[0] -= tone_period_init[0];\
            chnl_trigger[0] ^= 1;\
        }\
        if(++chnl_period[1] >= tone_period_init[1])\
        {\
            chnl_period[1] -= tone_period_init[1];\
            chnl_trigger[1] ^= 1;\
        }\
        if(++chnl_period[2] >= tone_period_init[2])\
        {\
            chnl_period[2] -= tone_period_init[2];\
            chnl_trigger[2] ^= 1;\
        }\
\
        if(++noise_period >= noise_period_init)\
        {\
            noise_period = 0;\
            if((noise_reg + 1) & 2)\
                noise_trigger ^= 1;\
            if(noise_reg & 1)\
                noise_reg ^= 0x24000;\
            noise_reg >>= 1;\
        }\
        updateEnvelope();\
\
        if((chnl_trigger[0] | TONE_ENABLE(0)) & (noise_trigger | NOISE_ENABLE(0)) & !chnl_mute[0])\
            s0 += (CHNL_ENVELOPE(0) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(0)]) * volume[0];\
        if((chnl_trigger[1] | TONE_ENABLE(1)) & (noise_trigger | NOISE_ENABLE(1)) & !chnl_mute[1])\
            s1 += (CHNL_ENVELOPE(1) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(1)]) * volume[1];\
        if((chnl_trigger[2] | TONE_ENABLE(2)) & (noise_trigger | NOISE_ENABLE(2)) & !chnl_mute[2])\
            s2 += (CHNL_ENVELOPE(2) ? ay::levels[env_vol] : ay::levels[CHNL_VOLUME(2)]) * volume[2];\
    }\
\
    s0 = s0 / (float)ay_tacts;\
    s1 = (s1 / (float)ay_tacts) / 1.42;\
    s2 = s2 / (float)ay_tacts;

#define AY_PROCESS_START_LOOP \
    for(unsigned long i = 0; i < work_len; i++)\
    {

#define AY_PROCESS_END_LOOP \
    }

ay::ay(AYSongInfo *info)
{
    songinfo = info;
    int_limit = songinfo->sr / info->int_freq;

    float ay_tacts_f = (float)songinfo->ay_freq / (float)songinfo->sr / (float)8;
    ay_tacts = ay_tacts_f;
    if((ay_tacts_f - ay_tacts) >= 0.5)
        ay_tacts++;

    for(unsigned long i = 0; i < sizeof_array(ay::levels); i++)
    {
        ay::levels[i] = ay::init_levels[i] / 4;
    }

    ayReset();
}

ay::~ay()
{
}

void ay::SetParameters()
{
    float ay_tacts_f = (float)songinfo->ay_freq / (float)songinfo->sr / (float)8;
    ay_tacts = ay_tacts_f;
    if((ay_tacts_f - ay_tacts) >= 0.5)
        ay_tacts++;
    int_limit = songinfo->sr / songinfo->int_freq;
}

void ay::ayReset()
{
    //init regs with defaults
    int_counter = 0;
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
    env_type_old = 0;

    setEnvelope();
}

void ay::ayWrite(unsigned char reg, unsigned char val)
{
    regs[reg & 0xf] = val;
    switch(reg)
    {
        case AY_CHNL_A_COARSE:
        case AY_CHNL_A_FINE:
            tone_period_init[0] = TONE_PERIOD(0);
            if(!tone_period_init[0])
                tone_period_init[0]++;
            break;
        case AY_CHNL_B_COARSE:
        case AY_CHNL_B_FINE:
            tone_period_init[1] = TONE_PERIOD(1);
            if(!tone_period_init[1])
                tone_period_init[1]++;
            break;
        case AY_CHNL_C_COARSE:
        case AY_CHNL_C_FINE:
            tone_period_init[2] = TONE_PERIOD(2);
            if(!tone_period_init[2])
                tone_period_init[2]++;
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
        case AY_ENV_FINE:
        case AY_ENV_COARSE:
            env_period_init = ENVELOPE_PERIOD * 2;
            setEnvelope();
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
    if(env_type != env_type_old)
    {
        env_type_old = env_type;
        env_period = 0;
        env_tick = 0;
        bool env_attack = (env_type & 0x4) ? true : false;
        env_vol = env_attack ? 0 : 15;
        env_trigger = env_attack ? 1 : -1;
    }

}

void ay::updateEnvelope()
{
    env_period++;
    if(env_period >= env_period_init)
    {
        env_period = 0;
        if(env_tick < 16) //if >=16 - no more processing
        {
            if((env_tick == 15) && (env_type & 0x1) && (env_type && 0x8)) //hold + continue
            {
                bool env_attack = (env_type & 0x4) ? true : false;
                if(env_type & 0x2) //alternate;
                    env_attack = !env_attack;
                env_vol = env_attack ? 15 : 0;
                env_trigger = env_attack ? 1 : -1;
                env_tick = 16; //end processing;
            }
            else if((env_tick == 15) && !(env_type & 0x8)) //do not continue
            {
                env_tick = 16;
                env_vol = 0;
            }
            else
            {
                env_tick = ++env_tick % 16;
                if(env_tick == 0) //new cycle
                {
                    if(env_type & 0x2) //alternate
                        env_trigger = -env_trigger;
                    else //restart
                    {
                        bool env_attack = (env_type & 0x4) ? true : false;
                        env_vol = env_attack ? 0 : 15;
                        env_trigger = env_attack ? 1 : -1;
                    }
                }
                else
                    env_vol += env_trigger;
            }
        }
    }
}

void ay::ayProcess(unsigned char *stream, unsigned long len)
{
    AY_PROCESS_INIT
    AY_PROCESS_START_LOOP
    AY_PROCESS_STEP
    stream16[i * 2] = s0 + s1;
    stream16[i * 2 + 1] = s2 + s1;
    AY_PROCESS_END_LOOP
}
void ay::ayProcessMono(unsigned char *stream, unsigned long len)
{
    AY_PROCESS_INIT
    AY_PROCESS_START_LOOP
    AY_PROCESS_STEP
    stream16[i] = s0 + s1 + s2;
    AY_PROCESS_END_LOOP
}
