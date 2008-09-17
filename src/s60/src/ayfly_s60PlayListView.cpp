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
#include "s60.h"
#include <barsread.h>

Cayfly_s60PlayListView* Cayfly_s60PlayListView::NewL(const TRect& aRect)
{
    Cayfly_s60PlayListView* me = new (ELeave) Cayfly_s60PlayListView();
    CleanupStack::PushL(me);
    me->ConstructL(aRect);
    CleanupStack::Pop(me);
    return (me);
}

Cayfly_s60PlayListView::Cayfly_s60PlayListView()
{
    currentSong = 0;
    currentIndex = 0;
    volume = 0.5;    
}

Cayfly_s60PlayListView::~Cayfly_s60PlayListView()
{
    iFocusPos.Close();
    delete iListBox;
    delete iSongArray;
    if(currentSong)
    {
        ay_closesong(&currentSong);
    }
    if(player)
    {
        delete player;
        player = 0;
    }
}

void Cayfly_s60PlayListView::ConstructL(const TRect& aRect)
{
    player = new Cayfly_s60Audio;
    iSongArray = new (ELeave) CDesC16ArrayFlat(5);
    CreateWindowL();

    SetUpListBoxL();

    SetRect(aRect);
    ActivateL();
}

void Cayfly_s60PlayListView::SetUpListBoxL()
{
    iListBox = new (ELeave) CAknSingleStyleListBox();
    iListBox->SetContainerWindowL(*this);

    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC(reader, R_AYFLY_PLAYLIST);

    // Create the list box
    iListBox->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader


    // Add this to observe the list box
    iListBox->SetListBoxObserver(this);

    //  Add vertical scroll bars (which are visible when necessary)
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
}

void Cayfly_s60PlayListView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
{
    switch(aEventType)
    {
        case EEventEnterKeyPressed:
        case EEventItemClicked:            
        {// An item has been chosen and will be opened  
            iListBox->DrawDeferred();
            if(iSongArray->Count() < 1)
                return;
            currentIndex = iListBox->CurrentItemIndex();
            TFileName filePath = iSongArray->operator [](currentIndex);
            if(currentSong)
            {
                ay_closesong(&currentSong);
            }
            currentSong = ay_initsong(filePath, 44100, player);            
            ay_setelapsedcallback(currentSong, Cayfly_s60PlayListView::elapsedCallback, this);
            ay_setstoppedcallback(currentSong, Cayfly_s60PlayListView::stopCallback, this);
            ay_startsong(currentSong);

        }
            break;
        default: // Nothing to do
            break;
    };
}

TKeyResponse Cayfly_s60PlayListView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if(aType == EEventKeyDown)
    {
        if(aKeyEvent.iScanCode == EStdKeyRightArrow)
        {
            UpVolume();
            return EKeyWasConsumed;
        }
        else if(aKeyEvent.iScanCode == EStdKeyLeftArrow)
        {
            DownVolume();
            return EKeyWasConsumed;
        }
        else if(aKeyEvent.iScanCode == EStdKeyBackspace)
        {
            CTextListBoxModel* model = iListBox->Model();
            User::LeaveIfNull(model);
            if(model->NumberOfItems() < 1)
                return EKeyWasConsumed;
            model->SetOwnershipType(ELbmOwnsItemArray);
            CDesCArray* itemArray = static_cast<CDesCArray*> (model->ItemTextArray());
            User::LeaveIfNull(itemArray);
            if(currentIndex == iListBox->CurrentItemIndex())
            {
                ay_closesong(&currentSong);
            }
            if(currentIndex >= iListBox->CurrentItemIndex())
            {
                currentIndex--;
            }
            iSongArray->Delete(iListBox->CurrentItemIndex());
            itemArray->Delete(iListBox->CurrentItemIndex()); 
            iListBox->HandleItemRemovalL();
            iListBox->DrawDeferred();
            return EKeyWasConsumed;

        }
    }
    return (iListBox->OfferKeyEventL(aKeyEvent, aType));
}

void Cayfly_s60PlayListView::Draw(const TRect& aRect) const
{
    CWindowGc& gc = SystemGc();
    gc.Clear(aRect);
}

void Cayfly_s60PlayListView::SizeChanged()
{
    const TPoint listPosition(0, 0);
    iListBox->SetExtent(listPosition, iListBox->MinimumSize());
}

TInt Cayfly_s60PlayListView::CountComponentControls() const
{
    return (1); // return number of controls inside this container
}

CCoeControl* Cayfly_s60PlayListView::ComponentControl(TInt aIndex) const
{
    switch(aIndex)
    {
        case 0:
            return (iListBox);
        default:
            return (NULL);
    }
}

void Cayfly_s60PlayListView::AddFile(TFileName filePath)
{
    CTextListBoxModel* model = iListBox->Model(); // Does not own the returned model
    User::LeaveIfNull(model);
    model->SetOwnershipType(ELbmOwnsItemArray);
    CDesCArray* itemArray = static_cast<CDesCArray*> (model->ItemTextArray());
    User::LeaveIfNull(itemArray);

    TParse tfp;
    tfp.Set(filePath, NULL, NULL);
    TFileName FullPath;
    FullPath.Zero();
    TFileName lbString;
    lbString.Zero();
    lbString.Append(_L("\t"));
    lbString.Append(tfp.NameAndExt());
    lbString.Append(_L("\t"));
    //lbString.Append(tfp.DriveAndPath());
    //lbString.Append(_L("\t"));
    iSongArray->AppendL(filePath);
    itemArray->AppendL(lbString);
    iListBox->HandleItemAdditionL();
    if(model->NumberOfItems() == 1)
    {
        currentIndex = 0;
        iListBox->SetCurrentItemIndex(0);
    }
    iListBox->DrawNow();
}

void Cayfly_s60PlayListView::StartPlayer()
{
    if(currentSong && ay_songstarted(currentSong))
    {
        return;
    }
    CTextListBoxModel* model = iListBox->Model(); // Does not own the returned model
    User::LeaveIfNull(model);
    if(model->NumberOfItems() < 1)
        return;
    if(currentIndex < 0)
        currentIndex = 0;
    iListBox->SetCurrentItemIndex(currentIndex);    
    HandleListBoxEventL(iListBox, EEventEnterKeyPressed);
}

void Cayfly_s60PlayListView::StopPlayer()
{
    if(currentSong)
    {
        ay_closesong(&currentSong);
    }
}

void Cayfly_s60PlayListView::UpVolume()
{
    if(currentSong && player)
    {
        player->UpDeviceVolume();
    }
}

void Cayfly_s60PlayListView::DownVolume()
{
    if(currentSong)
    {
        player->DownDeviceVolume();
    }
}

void Cayfly_s60PlayListView::NextSong()
{
    if(currentSong)
    {
        ay_closesong(&currentSong);
        currentIndex++;
        CTextListBoxModel* model = iListBox->Model();
        User::LeaveIfNull(model);
        if(model->NumberOfItems() <= currentIndex)
        {
            currentIndex = 0;
            return;
        }
        if(currentIndex <0)
            currentIndex = 0;
        iListBox->SetCurrentItemIndex(currentIndex);
        HandleListBoxEventL(iListBox, EEventEnterKeyPressed);
    }
}

void Cayfly_s60PlayListView::stopCallback(void *arg)
{
    Cayfly_s60PlayListView *me = (Cayfly_s60PlayListView *)arg;
    me->NextSong();
}

bool Cayfly_s60PlayListView::elapsedCallback(void *arg)
{
    return true;    
}

