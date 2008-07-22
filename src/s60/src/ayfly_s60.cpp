/*
 ============================================================================
 Name		: ayfly_s60.cpp
 Author	  : Andrew Deryabin
 Copyright   : Andrew Deryabin (c) 2008
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "common.h"

CConsoleBase *gConsole = 0;

LOCAL_C CApaApplication* NewApplication()
{
	return new Cayfly_s60Application;
}

GLDEF_C TInt E32Main()
{		
	return EikStart::RunApplication( NewApplication );
}

