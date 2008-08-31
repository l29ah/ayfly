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
// Cayfly_s60AppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Cayfly_s60AppView* Cayfly_s60AppView::NewL(const TRect& aRect)
	{
	Cayfly_s60AppView* self = Cayfly_s60AppView::NewLC(aRect);
	CleanupStack::Pop(self);	
	return self;
	}

// -----------------------------------------------------------------------------
// Cayfly_s60AppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Cayfly_s60AppView* Cayfly_s60AppView::NewLC(const TRect& aRect)
	{
	Cayfly_s60AppView* self = new ( ELeave ) Cayfly_s60AppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// Cayfly_s60AppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Cayfly_s60AppView::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// Cayfly_s60AppView::Cayfly_s60AppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
Cayfly_s60AppView::Cayfly_s60AppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// Cayfly_s60AppView::~Cayfly_s60AppView()
// Destructor.
// -----------------------------------------------------------------------------
//
Cayfly_s60AppView::~Cayfly_s60AppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// Cayfly_s60AppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void Cayfly_s60AppView::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(Rect());

	// Clears the screen
	gc.Clear(drawRect);

	}

// -----------------------------------------------------------------------------
// Cayfly_s60AppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void Cayfly_s60AppView::SizeChanged()
	{
	DrawNow();
	}
// End of File
