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
#ifdef UIQ3
#include <qikappui.h>
#else
#include <aknappui.h>
#endif

// FORWARD DECLARATIONS
class Cayfly_s60AppView;

// CLASS DECLARATION
/**
 * Cayfly_s60AppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */

struct CurrentSong
{
    TFileName FileName;
    //AYSongInfo *song;
};

class Cayfly_s60AppUi :
#ifdef UIQ3
    public CQikAppUi
#else
    public CAknAppUi
#endif
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
	
	/**
	     * From CEikAppUi, HandleCommandL.
	     * Takes care of command handling.
	     * @param aCommand Command to be handled.
	     */
	void HandleCommandL(TInt aCommand);

private:
	// Functions from base classes

	

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
	void *currentSong;
	TInt iVolume;
	//Cayfly_s60Audio *player;
	//TFileName folderName;


	};

#endif // __AYFLY_S60APPUI_h__
// End of File
