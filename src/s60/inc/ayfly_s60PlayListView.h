/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew                                 *
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

#ifndef AYFLY_S60PLAYLISTVIEW_H_
#define AYFLY_S60PLAYLISTVIEW_H_

#include <eiklbo.h>
#include <coecntrl.h>

class Cayfly_s60PlayListView: public CCoeControl, public MEikListBoxObserver
{
public:
    static Cayfly_s60PlayListView* NewL(const TRect& aRect);
    ~Cayfly_s60PlayListView();
public:
    //  from CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
public:
    // from MEikListBoxObserver 
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
    void AddFile(TFileName filePath);
    void StartPlayer();
    void StopPlayer();
    void UpVolume();
    void DownVolume();
    void NextSong();    
protected:
    //  From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;    
private:
    void SetUpListBoxL();
private:
    Cayfly_s60PlayListView();
    void ConstructL(const TRect& aRect);
private:
    void Draw(const TRect& aRect) const;
private:
    CAknSingleStyleListBox* iListBox;
private:
    void *currentSong;
    float volume;
    TInt currentIndex;
    static bool elapsedCallback(void *arg);
    Cayfly_s60Audio *player;
    static void stopCallback(void *arg);
    CDesC16Array* iSongArray;
    void StartSong(TInt index);
    
};

#endif /* AYFLY_S60PLAYLISTVIEW_H_ */
