/*
 ============================================================================
 Name		: ayfly_s60AppUi.h
 Author	  : Andrew Deryabin
 Copyright   : Andrew Deryabin (c) 2008
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __AYFLY_S60APPUI_h__
#define __AYFLY_S60APPUI_h__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class Cayfly_s60AppView;

// CLASS DECLARATION
/**
 * Cayfly_s60AppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class Cayfly_s60AppUi : public CAknAppUi
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * Cayfly_s60AppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	Cayfly_s60AppUi();

	/**
	 * ~Cayfly_s60AppUi.
	 * Virtual Destructor.
	 */
	virtual ~Cayfly_s60AppUi();

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange();

	TKeyResponse HandleKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);


private:
	// Data

	/**
	 * The application view
	 * Owned by Cayfly_s60AppUi
	 */
	Cayfly_s60AppView* iAppView;
	TFileName fileName;
	//TFileName folderName;


	};

#endif // __AYFLY_S60APPUI_h__
// End of File
