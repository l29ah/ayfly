/*
 ============================================================================
 Name		: ayfly_s60.pan
 Author	  : Andrew Deryabin
 Copyright   : Andrew Deryabin (c) 2008
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __AYFLY_S60_PAN__
#define __AYFLY_S60_PAN__

/** ayfly_s60 application panic codes */
enum Tayfly_s60Panics
	{
	Eayfly_s60Ui = 1
	// add further panics here
	};

inline void Panic(Tayfly_s60Panics aReason)
	{
	_LIT(applicationName, "ayfly_s60");
	User::Panic(applicationName, aReason);
	}

#endif // __AYFLY_S60_PAN__
