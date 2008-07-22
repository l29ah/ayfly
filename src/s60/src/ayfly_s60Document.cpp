/*
 ============================================================================
 Name		: ayfly_s60Document.cpp
 Author	  : Andrew Deryabin
 Copyright   : Andrew Deryabin (c) 2008
 Description : Cayfly_s60Document implementation
 ============================================================================
 */

// INCLUDE FILES
#include "common.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cayfly_s60Document::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Cayfly_s60Document* Cayfly_s60Document::NewL(CEikApplication& aApp)
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
Cayfly_s60Document* Cayfly_s60Document::NewLC(CEikApplication& aApp)
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
Cayfly_s60Document::Cayfly_s60Document(CEikApplication& aApp) :
	CAknDocument(aApp)
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
