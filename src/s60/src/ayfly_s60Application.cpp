/*
 ============================================================================
 Name		: ayfly_s60Application.cpp
 Author	  : Andrew Deryabin
 Copyright   : Andrew Deryabin (c) 2008
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "common.h"


Cayfly_s60Application::Cayfly_s60Application()
{
	gConsole = Console::NewL(_L("Debug output"), TSize(KDefaultConsWidth, KDefaultConsHeight));	
}

Cayfly_s60Application::~Cayfly_s60Application()
{
	if(gConsole)
	{
		delete gConsole;
		gConsole = 0;
	}
	
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cayfly_s60Application::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* Cayfly_s60Application::CreateDocumentL()
	{
	// Create an ayfly_s60 document, and return a pointer to it
	return Cayfly_s60Document::NewL(*this);
	}

// -----------------------------------------------------------------------------
// Cayfly_s60Application::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid Cayfly_s60Application::AppDllUid() const
	{
	// Return the UID for the ayfly_s60 application
	return KUidayfly_s60App;
	}

// End of File
