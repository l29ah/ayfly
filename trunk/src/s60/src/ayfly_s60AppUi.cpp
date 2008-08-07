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

extern AbstractAudio *player;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cayfly_s60AppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Cayfly_s60AppUi::ConstructL()
{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);

	// Create view object
	iAppView = Cayfly_s60AppView::NewL(ClientRect() );
	//iFileData = new TUint16[512];
	//aFileName = new TPtr16(iFileData, 512, 512);
	//iFileData [0] = 0;
	fileName = PathInfo::MemoryCardRootPath();
	//folderName = PathInfo::MemoryCardRootPath();

	initSpeccy();
	//readFile(TEXT("E:\\Others\\ayfly\\KSA-MTV.stc"));
	player = new Cayfly_s60Audio();
	setPlayer(player);

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

	if (player)
	{
		delete player;
		player = 0;
	}

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
	case EAknSoftkeyExit:
	{
        shutdownSpeccy();
        delete player;
        player = 0;
		Exit();
	}
		break;
	case ECommand1:
	{
		TBool bRet = CAknFileSelectionDialog::RunDlgLD(fileName, PathInfo::MemoryCardRootPath(), _L("Select file!"), NULL);
		if (bRet)
		{
			player->Stop();
			shutdownSpeccy();
			initSpeccy();
			info.FilePath = fileName;
			readFile(info);
			fileName = PathInfo::MemoryCardRootPath();
		}
	}
		break;

	case ECommand2:
	{
		player->Start();
	}
		break;
	case ECommand3:
	{
		player->Stop();
	}
		break;
	case EHelp:
	{

		CArrayFix<TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
		HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
	}
		break;
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
            Cayfly_s60Audio *_player = (Cayfly_s60Audio *)player;
            _player->SetDeviceVolume(_player->GetDeviceVolume() + 1);
        }
        else if(aKeyEvent.iCode == EKeyDownArrow)
        {
            Cayfly_s60Audio *_player = (Cayfly_s60Audio *)player;
            _player->SetDeviceVolume(_player->GetDeviceVolume() - 1);
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
