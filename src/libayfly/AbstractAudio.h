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

#ifndef AUDIO_H_
#define AUDIO_H_

class AbstractAudio
{
public:
    AbstractAudio(unsigned long _sr, AYSongInfo *info);
    virtual ~AbstractAudio();
    virtual bool Start(void) = 0;
    virtual void Stop() = 0;
    virtual void WriteAy(unsigned char reg, unsigned char val)
    {
        ay8910->ayWrite(reg, val);
    }
    ;
    unsigned char ReadAy(unsigned char reg)
    {
        return ay8910->ayRead(reg);
    }
    ;
    void ResetAy()
    {
        ay8910->ayReset();
    }
    ;
    inline virtual bool ChnlMute(unsigned long chnl, bool mute)
    {
        if (ay8910)
            ay8910->chnlMute(chnl, mute);
    }
    ;
    inline virtual bool ChnlMuted(unsigned long chnl)
    {
        return ay8910 ? ay8910->chnlMuted(chnl) : true;
    }
    ;
    bool Started()
    {
        return started;
    }
    ;
    inline double GetVolume(unsigned long chnl)
    {
        return ay8910 ? ay8910->GetVolume(chnl) : 0;
    }
    ;
    inline void SetVolume(unsigned long chnl, double new_volume)
    {
        if (ay8910)
        {
            ay8910->SetVolume(chnl, new_volume);
        }
    }
    ;
    inline const unsigned char *GetAYRegs(unsigned long chip_num)
    {
        return ay8910 ? ay8910->GetRegs() : 0;
    }
    ;
protected:
    unsigned long sr;
    ay *ay8910;
    bool started;
    AYSongInfo *songinfo;
};

#endif /*AUDIO_H_*/
