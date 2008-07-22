/*
 ============================================================================
 Name		: ayfly_s60Application.h
 Author	  : Andrew Deryabin
 Copyright   : Andrew Deryabin (c) 2008
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __AYFLY_S60APPLICATION_H__
#define __AYFLY_S60APPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "ayfly_s60.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidayfly_s60App =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * Cayfly_s60Application application class.
 * Provides factory to create concrete document object.
 * An instance of Cayfly_s60Application is the application part of the
 * AVKON application framework for the ayfly_s60 example application.
 */
class Cayfly_s60Application : public CAknApplication
	{
public:
	Cayfly_s60Application();
	~Cayfly_s60Application();
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidayfly_s60App).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates Cayfly_s60Document document object. The returned
	 * pointer in not owned by the Cayfly_s60Application object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __AYFLY_S60APPLICATION_H__
// End of File
