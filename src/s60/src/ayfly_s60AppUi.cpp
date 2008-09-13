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

// INCLUDE FILES
#include "s60.h"
#include <maknfilefilter.h>

#ifdef S60

class CSongFilter: public MAknFileFilter
{
    TBool Accept(const TDesC &/*aDriveAndPath*/, const TEntry &aEntry) const
    {
        if(aEntry.IsDir() || ay_format_supported(aEntry.iName.Right(4)))
        return ETrue;
        return EFalse;
    }
};

#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cayfly_s60AppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Cayfly_s60AppUi::ConstructL()
{
    // Initialise app UI with standard value.
#ifdef UIQ3
    CQikAppUi::ConstructL();
#else
    BaseConstructL(CAknAppUi::EAknEnableSkin);
#endif
    // Create view object
#ifdef UIQ3
    iAppView = Cayfly_s60AppView::NewL(*this);
#else
    iAppView = Cayfly_s60PlayListView::NewL(ClientRect());
#endif

#ifdef UIQ3
    AddViewL(*iAppView);
#else
    iAppView->SetMopParent(this);
    AddToStackL(iAppView);
#endif
    iVolume = 5;
}
// -----------------------------------------------------------------------------
// Cayfly_s60AppUi::Cayfly_s60AppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
Cayfly_s60AppUi::Cayfly_s60AppUi()
{
    // No implementation required
}

// -----------------------------------------------------------------------------
// Cayfly_s60AppUi::~Cayfly_s60AppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
Cayfly_s60AppUi::~Cayfly_s60AppUi()
{
    if(iAppView)
    {
        delete iAppView;
        iAppView = NULL;
    }

}
// -----------------------------------------------------------------------------
// Cayfly_s60AppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void Cayfly_s60AppUi::HandleCommandL(TInt aCommand)
{
    switch(aCommand)
    {
        case EEikCmdExit:
#ifndef UIQ3 //S60
        case EAknSoftkeyExit:
#endif
        {
            Exit();
        }
            break;
        case EAddFile:
        {
            TFileName FileName = _L("C:\\");
#ifndef UIQ3            
            CAknFileSelectionDialog* dlg = CAknFileSelectionDialog::NewL(ECFDDialogTypeSelect);
            CSongFilter* filter = new(ELeave) CSongFilter;
            dlg->SetDefaultFolderL(_L("\\"));
            dlg->AddFilterL(filter);
            TBool bRet = dlg->ExecuteL(FileName);

#else //UIQ3
                    CDesCArray* mime = new (ELeave) CDesCArrayFlat(1);
                    CleanupStack::PushL(mime);
                    CDesCArray* file = new(ELeave) CDesCArrayFlat(1);
                    CleanupStack::PushL(file);
                    TBool bRet = CQikSelectFileDlg::RunDlgLD(*mime, *file);
                    if(bRet)
                    FileName = (*file) [0];
                    CleanupStack::PopAndDestroy(2);
#endif
                    if (bRet)
                    {                        
                        iAppView->AddFile(FileName);
                    }
                }
                break;
                
                case EStopPlayer:
                {
                    iAppView->StopSong();
                }
                break;
                case EHelp:
                {

                }
                break;
#ifdef EKA2
#ifndef UIQ3
                    case EAbout:
                    {

                        CAknMessageQueryDialog* dlg = new (ELeave)CAknMessageQueryDialog();
                        dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
                        HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
                        dlg->QueryHeading()->SetTextL(*title);
                        CleanupStack::PopAndDestroy(); //title
                        HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
                        dlg->SetMessageTextL(*msg);
                        CleanupStack::PopAndDestroy(); //msg
                        dlg->RunLD();
                    }
                    break;
#endif
#endif
                    default:
                    {
                        //CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("!!!"));
                        //Panic(Eayfly_s60Ui);
                    }
                    break;
                }
            }

TKeyResponse Cayfly_s60AppUi::HandleKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
{
    if(aType != EEventKey)
    {
        return EKeyWasNotConsumed;
    }
    else
    {
        if(aKeyEvent.iCode == EKeyRightArrow)
        {
                return EKeyWasConsumed;
        }
        else if(aKeyEvent.iCode == EKeyLeftArrow)
        {
                return EKeyWasConsumed;
        }        
    }
    return EKeyWasNotConsumed;
}

// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void Cayfly_s60AppUi::HandleStatusPaneSizeChange()
{
    iAppView->SetRect(ClientRect());
}

// End of File
