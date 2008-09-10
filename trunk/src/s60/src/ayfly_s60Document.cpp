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
// Cayfly_s60Document::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
#ifdef UIQ3
Cayfly_s60Document* Cayfly_s60Document::NewL(CQikApplication& aApp)
#else
Cayfly_s60Document* Cayfly_s60Document::NewL(CEikApplication& aApp)
#endif
	{
	Cayfly_s60Document* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// Cayfly_s60Document::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
#ifdef UIQ3
Cayfly_s60Document* Cayfly_s60Document::NewLC(CQikApplication& aApp)
#else
Cayfly_s60Document* Cayfly_s60Document::NewLC(CEikApplication& aApp)
#endif
	{
	Cayfly_s60Document* self = new ( ELeave ) Cayfly_s60Document( aApp );

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// Cayfly_s60Document::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Cayfly_s60Document::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// Cayfly_s60Document::Cayfly_s60Document()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//

#ifdef UIQ3
Cayfly_s60Document::Cayfly_s60Document(CQikApplication& aApp) :
    CQikDocument(aApp)
#else //S60
Cayfly_s60Document::Cayfly_s60Document(CEikApplication& aApp) :
	CAknDocument(aApp)
#endif
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// Cayfly_s60Document::~Cayfly_s60Document()
// Destructor.
// ---------------------------------------------------------------------------
//
Cayfly_s60Document::~Cayfly_s60Document()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// Cayfly_s60Document::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* Cayfly_s60Document::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new ( ELeave )Cayfly_s60AppUi;
	}

// End of File
