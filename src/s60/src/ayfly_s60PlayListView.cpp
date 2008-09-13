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

#define ESoftStartSong 0x8001

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
    if(currentSong)
    {
        ay_closesong(&currentSong);
    }
}

void Cayfly_s60PlayListView::ConstructL(const TRect& aRect)
{
    CreateWindowL();

    SetUpListBoxL();

    SetRect(aRect);
    ActivateL();
}

void Cayfly_s60PlayListView::SetUpListBoxL()
{
    iListBox = new (ELeave) CAknDoubleStyleListBox();
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
            currentIndex = iListBox->CurrentItemIndex();
        case ESoftStartSong:
        {// An item has been chosen and will be opened
            CTextListBoxModel* model = iListBox->Model();
            User::LeaveIfNull(model);
            if(model->NumberOfItems() < 1)
                return;
            model->SetOwnershipType(ELbmOwnsItemArray);
            CDesCArray* itemArray = static_cast<CDesCArray*> (model->ItemTextArray());
            User::LeaveIfNull(itemArray);
            TFileName lbString = itemArray->operator [](currentIndex);
            TFileName fileNameExt;
            TFileName fileDrive;
            TFileName filePath;
            TInt index = lbString.Locate('\t');
            if(index != KErrNotFound)
            {
                lbString = lbString.Mid(index + 1);
                index = lbString.Locate('\t');
                if(index != KErrNotFound)
                {
                    fileNameExt = lbString.Left(index);
                    lbString = lbString.Mid(index + 1);
                    index = lbString.Locate('\t');
                    fileDrive = lbString.Left(index);
                }
            }
            filePath.Zero();
            filePath.Append(fileDrive);
            filePath.Append(fileNameExt);
            //CEikonEnv::InfoWinL(_L("DeviceMessage"), filePath);
            if(currentSong)
            {
                ay_closesong(&currentSong);
            }
            currentSong = ay_initsong(filePath, 44100);
            ay_setvolume(currentSong, 0, volume, 0);
            ay_setvolume(currentSong, 1, volume, 0);
            ay_setvolume(currentSong, 2, volume, 0);
            ay_setcallback(currentSong, elapsedCallback, this);
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
    TFileName lbString;
    lbString.Zero();
    lbString.Append(_L("\t"));
    lbString.Append(tfp.NameAndExt());
    lbString.Append(_L("\t"));
    lbString.Append(tfp.DriveAndPath());
    lbString.Append(_L("\t"));
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
    HandleListBoxEventL(iListBox, ESoftStartSong);
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
    if(currentSong)
    {
        volume = ay_getvolume(currentSong, 0, 0);
        volume += 0.1;
        ay_setvolume(currentSong, 0, volume, 0);
        ay_setvolume(currentSong, 1, volume, 0);
        ay_setvolume(currentSong, 2, volume, 0);
        volume = ay_getvolume(currentSong, 0, 0);
    }
}

void Cayfly_s60PlayListView::DownVolume()
{
    if(currentSong)
    {
        volume = ay_getvolume(currentSong, 0, 0);
        volume -= 0.1;
        ay_setvolume(currentSong, 0, volume, 0);
        ay_setvolume(currentSong, 1, volume, 0);
        ay_setvolume(currentSong, 2, volume, 0);
        volume = ay_getvolume(currentSong, 0, 0);
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
        HandleListBoxEventL(iListBox, ESoftStartSong);       
        
    }
}

void Cayfly_s60PlayListView::elapsedCallback(void *arg)
{
    Cayfly_s60PlayListView *me = (Cayfly_s60PlayListView *)arg;
    me->NextSong();    
}

