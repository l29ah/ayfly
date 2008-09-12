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
_LIT (KListFormatString, "%d\t");

const TInt KFolderIconSlot = 0;
const TInt KFileIconSlot = 1;

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
}

Cayfly_s60PlayListView::~Cayfly_s60PlayListView()
{    
    iFocusPos.Close();
    delete iListBox;
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
    iListBox = new (ELeave) CAknSingleGraphicStyleListBox();
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
            
        }
            break;
        default: // Nothing to do
            break;
    };
}

TKeyResponse Cayfly_s60PlayListView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
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
    CTextListBoxModel* model = iListBox->Model();    // Does not own the returned model
    User::LeaveIfNull(model);
    model->SetOwnershipType(ELbmOwnsItemArray);
    CDesCArray* itemArray = static_cast<CDesCArray*>(model->ItemTextArray());
    User::LeaveIfNull(itemArray);
    
    itemArray->Reset();
    
    TParse tfp;
    tfp.Set(filePath, NULL, NULL);
    TFileName fileName = tfp.NameAndExt();
    itemArray->AppendL(fileName);
    iListBox->HandleItemAdditionL();
    iListBox->SetCurrentItemIndex(itemArray->Count() - 1);
    iListBox->DrawNow();
}
    
