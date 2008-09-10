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

/**
Creates and constructs the view.

@param aAppUi Reference to the AppUi
@return Pointer to a CHelloWorldView object
*/
Cayfly_s60AppView* Cayfly_s60AppView::NewL(CQikAppUi& aAppUi)
    {
    Cayfly_s60AppView* self = new (ELeave) Cayfly_s60AppView(aAppUi);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/**
Constructor for the view.
Passes the application UI reference to the construction of the super class.

KNullViewId should normally be passed as parent view for the applications 
default view. The parent view is the logical view that is normally activated 
when a go back command is issued. KNullViewId will activate the system 
default view. 

@param aAppUi Reference to the application UI
*/
Cayfly_s60AppView::Cayfly_s60AppView(CQikAppUi& aAppUi) 
    : CQikViewBase(aAppUi, KNullViewId)
    {
        appui = (Cayfly_s60AppUi *)&aAppUi; 
    }

/**
Destructor for the view
*/
Cayfly_s60AppView::~Cayfly_s60AppView()
    {
    }

/**
2nd stage construction of the view.
*/
void Cayfly_s60AppView::ConstructL()
    {
    // Calls ConstructL that initialises the standard values. 
    // This should always be called in the concrete view implementations.
    BaseConstructL();
    }
    
/**
Inherited from CQikViewBase and called upon by the UI Framework. 
It creates the view from resource.
*/
void Cayfly_s60AppView::ViewConstructL()
    {
    // Loads information about the UI configurations this view supports
    // together with definition of each view.   
    ViewConstructFromResourceL(R_AYFLY_UI_CONFIGURATIONS);
    }

/**
Returns the view Id

@return Returns the Uid of the view
*/
TVwsViewId Cayfly_s60AppView::ViewId()const
    {
    return TVwsViewId(KUidayfly_s60App, KUidayfly_s60AppView);
    }

/**
Handles all commands in the view.
Called by the UI framework when a command has been issued.
The command Ids are defined in the .hrh file.

@param aCommand The command to be executed
@see CQikViewBase::HandleCommandL
*/
void Cayfly_s60AppView::HandleCommandL(CQikCommand& aCommand)
    {
    
    appui->HandleCommandL(aCommand.Id());
    
    /*switch(aCommand.Id())
        {
        
        default:
            CQikViewBase::HandleCommandL(aCommand);
            break;
        }*/
    }
