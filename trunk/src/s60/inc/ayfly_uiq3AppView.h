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

#ifndef AYFLY_UIQ3APPVIEW_H_
#define AYFLY_UIQ3APPVIEW_H_

#include <qikviewbase.h>

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidayfly_s60AppView =
    {
    1
    };

/**
A very simple view that displays the text "Hello world", drawn using the default title 
font supplied by UIQ. It also consist of three commands that will bring up infoprints.
*/
class Cayfly_s60AppView : public CQikViewBase
    {
public:
    static Cayfly_s60AppView* NewLC(CQikAppUi& aAppUi);
    ~Cayfly_s60AppView();
    
    // from CQikViewBase
    TVwsViewId ViewId()const;
    void HandleCommandL(CQikCommand& aCommand);
    
protected: 
    // from CQikViewBase
    void ViewConstructL();
    
private:
    Cayfly_s60AppView(CQikAppUi& aAppUi);
    void ConstructL();
    };

#endif /* AYFLY_UIQ3APPVIEW_H_ */
