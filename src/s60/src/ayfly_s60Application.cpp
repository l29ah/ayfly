/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew   				               *
 *   andrew@it-optima.ru                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// INCLUDE FILES
#include "s60.h"


Cayfly_s60Application::Cayfly_s60Application()
{
}

Cayfly_s60Application::~Cayfly_s60Application()
{
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
