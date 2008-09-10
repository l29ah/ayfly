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
	iAppView = Cayfly_s60AppView::NewL(ClientRect() );
#ifdef UIQ3
	AddViewL(*iAppView);
#endif
	iVolume = 5;
	currentSong = 0;
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
	if (iAppView)
	{
		delete iAppView;
		iAppView = NULL;
	}

	if(currentSong)
	    ay_closesong((void **)currentSong);

}

// -----------------------------------------------------------------------------
// Cayfly_s60AppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void Cayfly_s60AppUi::HandleCommandL(TInt aCommand)
{
	switch (aCommand)
	{
	case EEikCmdExit:
#ifndef UIQ3 //S60
	case EAknSoftkeyExit:
#endif
	{
	    if(currentSong)
	    {
	        ay_closesong(&currentSong);
	    }
		Exit();
	}
		break;
	case ECommand1:
	{
	    TFileName FileName = _L("E:");
#ifndef UIQ3
		TBool bRet = CAknFileSelectionDialog::RunDlgLD(FileName, _L("E:"), _L("Select file!"), NULL);
#else //S60
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
		    if(currentSong)
		        ay_closesong(&currentSong);
		    currentSong = ay_initsong(FileName, 44100);
		    if(!currentSong)
		    {
		        CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("Can't open file!"));
		    }
		}
	}
		break;

	case ECommand2:
	{
		if(currentSong)
		{
		    ay_startsong(currentSong);
		    Cayfly_s60Audio *_player = (Cayfly_s60Audio *)ay_getsongplayer(currentSong);
		    _player->SetDeviceVolume(iVolume);
		    iVolume = _player->GetDeviceVolume();
		}
	}
		break;
	case ECommand3:
	{
	    if(currentSong)
	    {
	        ay_stopsong(currentSong);
	    }
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
	    CEikonEnv::InfoWinL(_L("DeviceMessage"), _L("!!!"));
		Panic(Eayfly_s60Ui);
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
        if(aKeyEvent.iCode == EKeyUpArrow)
        {
            if(currentSong)
            {
                Cayfly_s60Audio *_player = (Cayfly_s60Audio *)ay_getsongplayer(currentSong);
                iVolume++;
                _player->SetDeviceVolume(iVolume);
                iVolume = _player->GetDeviceVolume();
            }
        }
        else if(aKeyEvent.iCode == EKeyDownArrow)
        {
            if(currentSong)
            {
                Cayfly_s60Audio *_player = (Cayfly_s60Audio *)ay_getsongplayer(currentSong);
                iVolume--;
                _player->SetDeviceVolume(_player->GetDeviceVolume() - 1);
                iVolume = _player->GetDeviceVolume();
            }
        }
        return EKeyWasConsumed;
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
	iAppView->SetRect(ClientRect() );
}

// End of File
