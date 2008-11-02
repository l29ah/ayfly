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

#ifdef S60

class CSongFilter : public MAknFileFilter
{
    TBool Accept(const TDesC &/*aDriveAndPath*/, const TEntry &aEntry) const
    {
        if (aEntry.IsDir() || ay_format_supported(aEntry.iName.Right(4)))
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
    iAppView = Cayfly_uiq3PlayListView::NewL(*this, KNullViewId);
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
#ifndef UIQ3
            TFileName FileName = PathInfo::MemoryCardRootPath();
            CAknMemorySelectionDialog* memDlg = CAknMemorySelectionDialog::NewL(ECFDDialogTypeSelect, ETrue);
            CAknMemorySelectionDialog::TMemory memory = CAknMemorySelectionDialog::EPhoneMemory;
            if(memDlg->ExecuteL(memory) == CAknFileSelectionDialog::ERightSoftkey)
            {
                // cancel selection
                break;
            }

            if(memory == CAknMemorySelectionDialog::EMemoryCard)
            {
                FileName = PathInfo::MemoryCardRootPath();
            }
            else
            {
                FileName = PathInfo::PhoneMemoryRootPath();
            }
            delete memDlg;
            CAknFileSelectionDialog* dlg = CAknFileSelectionDialog::NewL(ECFDDialogTypeSelect);
            CSongFilter* filter = new (ELeave) CSongFilter;
            dlg->SetDefaultFolderL(_L("\\"));
            dlg->AddFilterL(filter);
            TBool bRet = dlg->ExecuteL(FileName);
            delete dlg;

#else //UIQ3
            TFileName FileName = _L("E:\\");
            CDesCArray* mime = new (ELeave) CDesCArrayFlat(1);
            CleanupStack::PushL(mime);
            CDesCArray* file = new(ELeave) CDesCArrayFlat(1);
            CleanupStack::PushL(file);
            TBool bRet = CQikSelectFileDlg::RunDlgLD(*mime, *file);
            if(bRet)
            FileName = (*file) [0];
            CleanupStack::PopAndDestroy(2);
#endif
            if(bRet)
            {
                iAppView->AddFile(FileName);
            }
        }
            break;
        case EAddFolder:
        {
#ifndef UIQ3 
            TFileName FolderName = PathInfo::MemoryCardRootPath();
            CAknMemorySelectionDialog* memDlg = CAknMemorySelectionDialog::NewL(ECFDDialogTypeSelect, ETrue);
            CAknMemorySelectionDialog::TMemory memory = CAknMemorySelectionDialog::EPhoneMemory;
            if(memDlg->ExecuteL(memory) == CAknFileSelectionDialog::ERightSoftkey)
            {
                // cancel selection
                break;
            }

            if(memory == CAknMemorySelectionDialog::EMemoryCard)
            {
                FolderName = PathInfo::MemoryCardRootPath();
            }
            else
            {
                FolderName = PathInfo::PhoneMemoryRootPath();
            }
            delete memDlg;
            CAknFileSelectionDialog* dlg = CAknFileSelectionDialog::NewL(ECFDDialogTypeBrowse);

            // some dialog customizations:
            dlg->SetTitleL(_L("Select folder"));
            dlg->SetLeftSoftkeyFolderL(_L("Select"));
            dlg->SetRightSoftkeyRootFolderL(_L("Back")); // for root folder

            bool bRet = dlg->ExecuteL(FolderName);
            delete dlg;
            if(bRet)
            {
                RFs session;
                session.Connect();
                CDirScan* scan = CDirScan::NewL(session);
                CDir* files;
                scan->SetScanDataL(FolderName, KEntryAttNormal, ESortByName | EAscending, CDirScan::EScanDownTree);
                scan->NextL(files);
                while(files)
                {
                    for(TInt i = 0; i < files->Count(); i++)
                    {
                        TEntry file = (*files)[i];
                        if(ay_format_supported(file.iName.Right(4)))
                        {
                            TFileName FileName = _L("");
                            FileName.Append(scan->FullPath());
                            FileName.Append(file.iName);
                            iAppView->AddFile(FileName);
                        }
                    }
                    delete files;
                    scan->NextL(files);
                }
                delete scan;
                session.Close();
            }

#else //UIQ3
#endif
        }
            break;
        case EStartPlayer:
            iAppView->StartPlayer();
            break;
        case EStopPlayer:
            iAppView->StopPlayer();
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
                HBufC* title= iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
                dlg->QueryHeading()->SetTextL(*title);
                CleanupStack::PopAndDestroy(); //title
                HBufC* msg= iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
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

// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void Cayfly_s60AppUi::HandleStatusPaneSizeChange()
{
    if(iAppView)
        iAppView->SetRect(ClientRect());
}

// End of File
