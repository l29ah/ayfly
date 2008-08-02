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

double ay::init_levels [] = {0x0000, 0x0385, 0x053D, 0x0770, 0x0AD7, 0x0FD5, 0x15B0, 0x230C, 0x2B4C,
    0x43C1, 0x5A4B, 0x732F, 0x9204, 0xAFF1, 0xD921, 0xFFFF
};



#define TONE_ENABLE(ch) ((regs [AY_MIXER] >> (ch)) & 1)
#define NOISE_ENABLE(ch) ((regs [AY_MIXER] >> (3 + (ch))) & 1)
#define TONE_PERIOD(ch) (((((regs [((ch) << 1) + 1]) & 0xf) << 8)) | (regs [(ch) << 1]))
#define NOISE_PERIOD (regs [AY_NOISE_PERIOD] & 0x1f)
#define CHNL_VOLUME(ch) (regs [AY_CHNL_A_VOL + (ch)] & 0xf)
#define CHNL_ENVELOPE(ch) (regs [AY_CHNL_A_VOL + (ch)] & 0x10)
#define ENVELOPE_PERIOD (((regs [AY_ENV_COARSE]) << 8) | regs [AY_ENV_FINE])

FILE *f = 0;

ay::ay(long _ay_freq, int _buf_sz)
{
    buffer [0] = 0;
    buffer [1] = 0;
    buffer [2] = 0;

    ay_freq = _ay_freq;
    tail_len = 512;//buf_sz >> 2;
    if(tail_len < 4)
        tail_len = 4;

    buffer_tail [0] = new double [tail_len];
    buffer_tail [1] = new double [tail_len];
    buffer_tail [2] = new double [tail_len];

    float ay_tacts_f = (float)ay_freq / AUDIO_FREQ / 8;
    ay_tacts = ay_tacts_f;
    /*if(ay_tacts != ay_tacts_f)
        ay_tacts++;*/

    SetBufferSize(_buf_sz);

    for(unsigned long i = 0; i < tail_len; i++)
    {
        buffer_tail [0] [i] = buffer_tail [1] [i] = buffer_tail [2] [i] = 0;
    }

    double div = ay_tacts + 3;


    for(unsigned long i = 0; i < sizeof_array(ay::levels); i++)
    {
        ay::levels [i] = ay::init_levels [i] / div;
    }


    FilterOpts fopts;
    fopts.Fs = AUDIO_FREQ * ay_tacts;
    fopts.bw = 1.8;
    fopts.f0 = 22000;
    fopts.type = LPF;
    flt = new Filter3;
    flt->Init(&fopts);
    fopts.Fs = AUDIO_FREQ;
    fopts.bw = 1;
    fopts.f0 = 300;
    fopts.type = LPF;
    flt_bass = new Filter3;
    flt_bass->Init(&fopts);

    elapsedCallback = 0;
    elapsedCallbackArg = 0;

    int_limit = ay_tacts * (AUDIO_FREQ / INTR_FREQ);

    ayReset();
    //f = fopen("ay_tone.raw", "wb");
}

ay::~ay()
{
    delete [] buffer [0];
    delete [] buffer [1];
    delete [] buffer [2];
    delete [] buffer_tail [0];
    delete [] buffer_tail [1];
    delete [] buffer_tail [2];
    delete flt;
}

void ay::SetBufferSize(int _buf_sz)
{
    buf_sz = _buf_sz;
    q_len = buf_sz * ay_tacts;

    if(buffer [0])
    {
        delete [] buffer [0];
        buffer [0] = 0;
    }
    if(buffer [1])
    {
        delete [] buffer [1];
        buffer [1] = 0;
    }
    if(buffer [2])
    {
        delete [] buffer [2];
        buffer [2] = 0;
    }

    buffer [0] = new double [q_len];
    buffer [1] = new double [q_len];
    buffer [2] = new double [q_len];
}

void ay::ayReset()
{
    //init regs with defaults
    int_counter = 0;
    memset(regs, 0, sizeof (regs));
    regs [AY_GPIO_A] = regs [AY_GPIO_B] = 0xff;
    chnl_period [0] = chnl_period [1] = chnl_period [2] = 0;
    chnl_vol [0] = 0;
    chnl_vol [1] = 0;
    chnl_vol [2] = 0;
    chnl_enable [0] = false;
    chnl_enable [1] = false;
    chnl_enable [2] = false;
    noise_enable [0] = false;
    noise_enable [1] = false;
    noise_enable [2] = false;
    tone_period_init [0] = tone_period_init [1] = tone_period_init [2] = 1;
    chnl_mute [0] = chnl_mute [1] = chnl_mute [2] = false;
    env_type = 0;
    env_vol = 0;
    chnl_trigger [0] = chnl_trigger [1] = chnl_trigger [2] = 1;
    noise_reg = 0x1;
    noise_trigger = 1;
    noise_period = 0;
    ay_tone_freq = ay_freq / 16;
    ay_env_freq = ay_freq / 256;

    volume [0] = volume [1] = volume [2] = 1;

    setEnvelope();
}

void ay::ayWrite(unsigned char reg, unsigned char val)
{
    regs [reg] = val;
    switch(reg)
    {
    case AY_CHNL_A_COARSE:
    case AY_CHNL_A_FINE:
        tone_period_init [0] = TONE_PERIOD(0);
        if(!tone_period_init [0])
            tone_period_init [0]++;
        break;
    case AY_CHNL_B_COARSE:
    case AY_CHNL_B_FINE:
        tone_period_init [1] = TONE_PERIOD(1);
        if(!tone_period_init [1])
            tone_period_init [1]++;
        break;
    case AY_CHNL_C_COARSE:
    case AY_CHNL_C_FINE:
        tone_period_init [2] = TONE_PERIOD(2);
        if(!tone_period_init [2])
            tone_period_init [2]++;
        break;
    case AY_NOISE_PERIOD:
        noise_period_init = NOISE_PERIOD;
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
    return regs [reg];
}

void ay::setEnvelope()
{
    static unsigned long env_type_old = 0;

    /*long env_reg = ENVELOPE_PERIOD;
    if (env_reg == 0)
            env_reg = 65535;
    env_period = (float)AUDIO_FREQ / ((float)ay_env_freq / env_reg);*/
    env_type = regs [AY_ENV_SHAPE];
    if(env_type != env_type_old)
    {
        env_type_old = env_type;
        env_period = 0;
        env_tick = 0;
        bool env_attack = (env_type & 0x4) ? true : false;
        env_vol = env_attack ? 0 : 15;
        env_trigger = env_attack ? 1 : -1;
        //printf("env_type = %ld\n", env_type);
    }


}

void ay::updateEnvelope()
{
    env_period++;
    if(env_period >= env_period_init)
    {
        //printf("env_vol = %ld\n", env_vol);
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

void ay::ayProcess(unsigned char *stream, int len)
{
    unsigned long work_len = (len >> 2) * ay_tacts;
    for(unsigned long i = 0; i < work_len; i++)
    {
        buffer [0] [i] = buffer [1] [i] = buffer [2] [i] = 0;

        /*for(unsigned long ii = 0; ii < 4; ii++)
            execInstruction(elapsedCallback, elapsedCallbackArg);*/

        if(++int_counter > int_limit)
        {
            int_counter = 0;
            execInstruction(elapsedCallback, elapsedCallbackArg);
        }

        //if(TONE_ENABLE(0) == 0)
            if(++chnl_period [0] >= tone_period_init [0])
            {
                //while(chnl_period [0] >= tone_period_init [0])
                    chnl_period [0] -= tone_period_init [0];
                chnl_trigger [0] ^= 1;
            }
        //if(TONE_ENABLE(1) == 0)
            if(++chnl_period [1] >= tone_period_init [1])
            {
                //while(chnl_period [1] >= tone_period_init [1])
                    chnl_period [1] -= tone_period_init [1];
                chnl_trigger [1] ^= 1;
            }
        //if(TONE_ENABLE(2) == 0)
            if(++chnl_period [2] >= tone_period_init [2])
            {
                //while(chnl_period [2] >= tone_period_init [2])
                    chnl_period [2] -= tone_period_init [2];
                chnl_trigger [2] ^= 1;
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

        if((chnl_trigger [0] | TONE_ENABLE(0)) & (noise_trigger | NOISE_ENABLE(0)) & !chnl_mute [0])
            buffer [0] [i] = (CHNL_ENVELOPE(0) ? ay::levels [env_vol] : ay::levels [CHNL_VOLUME(0)]) * volume [0];
        if((chnl_trigger [1] | TONE_ENABLE(1)) & (noise_trigger | NOISE_ENABLE(1)) & !chnl_mute [1])
        {
            buffer [1] [i] = (CHNL_ENVELOPE(1) ? ay::levels [env_vol] : ay::levels [CHNL_VOLUME(1)]) * volume [1];
            buffer [1] [i] /= 1.42;
        }
        if((chnl_trigger [2] | TONE_ENABLE(2)) & (noise_trigger | NOISE_ENABLE(2)) & !chnl_mute [2])
            buffer [2] [i] = (CHNL_ENVELOPE(2) ? ay::levels [env_vol] : ay::levels [CHNL_VOLUME(2)]) * volume [2];

        flt->Process3(buffer [0] [i], buffer [1] [i], buffer [2] [i]);
    }

    short *stream16 = (short *)stream;
    unsigned long j = 0;
    unsigned long k = 0;
    unsigned long k_tail = 0;
    double tail0, tail1;
    //unsigned long t_index;

    work_len = len >> 1;
    unsigned long work_tail_len = tail_len * ay_tacts;
    for(unsigned long i = 0; i < (unsigned long)work_len; i += 2, j += ay_tacts, k++, k_tail += ay_tacts)
    {
        tail0 = k < tail_len ? buffer_tail [0] [k] : buffer [0] [k_tail - work_tail_len];
        tail1 = k < tail_len ? buffer_tail [2] [k] : buffer [2] [k_tail - work_tail_len];

#ifndef __SYMBIAN32__
        double s = buffer [0] [j] + buffer [1] [j] + buffer [2] [j];
        flt_bass->Process(s);

        buffer [1] [j] += s/2;

        //buffer [0] [j] = buffer [2] [j] = 0;
        //buffer [1] [j] = s;
#endif

        stream16 [i] = (buffer [0] [j] + buffer [1] [j] + tail1 / 4);
        stream16 [i + 1] = (buffer [2] [j] + buffer [1] [j] + tail0 / 4);
    }

    j = ((len >> 2) - tail_len) * ay_tacts;
    for(unsigned long i = 0; i < tail_len; i++, j += ay_tacts)
    {
        buffer_tail [0] [i] = buffer [0] [j];
        buffer_tail [2] [i] = buffer [2] [j];
    }
    //fwrite(stream, len, 1, f);

}

void ay::SetCallback(ELAPSED_CALLBACK _callback, void *_arg)
{
    elapsedCallback = _callback;
    elapsedCallbackArg = _arg;
}

