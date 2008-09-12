/*
 ============================================================================
 Name		: ayfly_s60Document.h
 Author	  : Andrew Deryabin
 Copyright   : Andrew Deryabin (c) 2008
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __AYFLY_S60DOCUMENT_h__
#define __AYFLY_S60DOCUMENT_h__

// INCLUDES
#ifdef UIQ3
#include <qikdocument.h>
#include <qikapplication.h>
#else //S60
#include <akndoc.h>
#endif

// FORWARD DECLARATIONS
class Cayfly_s60AppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * Cayfly_s60Document application class.
 * An instance of class Cayfly_s60Document is the Document part of the
 * AVKON application framework for the ayfly_s60 example application.
 */
class Cayfly_s60Document :
#ifdef UIQ3
    public CQikDocument
#else
    public CAknDocument
#endif
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a Cayfly_s60Document for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of Cayfly_s60Document.
	 */
#ifdef UIQ3
	static Cayfly_s60Document* NewL(CQikApplication& aApp);
#else
	static Cayfly_s60Document* NewL(CEikApplication& aApp);
#endif

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a Cayfly_s60Document for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of Cayfly_s60Document.
	 */
#ifdef UIQ3
	static Cayfly_s60Document* NewLC(CQikApplication& aApp);
#else
	static Cayfly_s60Document* NewLC(CEikApplication& aApp);
#endif

	/**
	 * ~Cayfly_s60Document
	 * Virtual Destructor.
	 */
	virtual ~Cayfly_s60Document();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a Cayfly_s60AppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * Cayfly_s60Document.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
#ifdef UIQ3
	Cayfly_s60Document(CQikApplication& aApp);
#else //S60
	Cayfly_s60Document(CEikApplication& aApp);
#endif

	};

#endif // __AYFLY_S60DOCUMENT_h__
// End of File
