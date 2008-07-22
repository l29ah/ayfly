/*
 ============================================================================
 Name		: ayfly_s60AppView.cpp
 Author	  : Andrew Deryabin
 Copyright   : Andrew Deryabin (c) 2008
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include "common.h"
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
