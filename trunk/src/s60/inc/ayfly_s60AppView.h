/*
 ============================================================================
 Name		: ayfly_s60AppView.h
 Author	  : Andrew Deryabin
 Copyright   : Andrew Deryabin (c) 2008
 Description : Declares view class for application.
 ============================================================================
 */

#ifndef __AYFLY_S60APPVIEW_h__
#define __AYFLY_S60APPVIEW_h__

// INCLUDES
#include <coecntrl.h>

const TUid KUidayfly_s60View =
    {
    1
    };

// CLASS DECLARATION

class Cayfly_s60AppView : public CCoeControl
	{
public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a Cayfly_s60AppView object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of Cayfly_s60AppView.
	 */
	static Cayfly_s60AppView* NewL(const TRect& aRect);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Create a Cayfly_s60AppView object, which will draw itself
	 * to aRect.
	 * @param aRect Rectangle this view will be drawn to.
	 * @return A pointer to the created instance of Cayfly_s60AppView.
	 */
	static Cayfly_s60AppView* NewLC(const TRect& aRect);

	/**
	 * ~Cayfly_s60AppView
	 * Virtual Destructor.
	 */
	virtual ~Cayfly_s60AppView();
	/*TVwsViewId ViewId() const
	{
	    return TVwsViewId(KUidayfly_s60App, KUidayfly_s60View);	
	};*/

public:
	// Functions from base classes

	/**
	 * From CCoeControl, Draw
	 * Draw this Cayfly_s60AppView to the screen.
	 * @param aRect the rectangle of this view that needs updating
	 */
	void Draw(const TRect& aRect) const;

	/**
	 * From CoeControl, SizeChanged.
	 * Called by framework when the view size is changed.
	 */
	virtual void SizeChanged();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * Cayfly_s60AppView object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect);

	/**
	 * Cayfly_s60AppView.
	 * C++ default constructor.
	 */
	Cayfly_s60AppView();

	};

#endif // __AYFLY_S60APPVIEW_h__
// End of File
