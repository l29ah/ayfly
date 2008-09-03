/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew                                 *
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

#ifndef S60_H_
#    define S60_H_

#    ifndef UNICODE
#        define UNICODE
#    endif

#    ifndef _UNICODE
#        define _UNICODE
#    endif

#ifndef sizeof_array
#    define sizeof_array(x) sizeof(x) / sizeof(x [0])
#endif

#include "ayfly.h"

#ifdef EKA2
#include <e32cmn.h>
#include <eikstart.h>
#endif
#include <f32file.h>
#include <s32std.h>
#include <s32file.h>
#include <avkon.hrh>
#include <coemain.h>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <hlplch.h>
#include <caknfileselectiondialog.h>
#include <pathinfo.h>

#include "ayfly_s60.hrh"
#include "ayfly_s60.pan"
#include "ayfly_s60Document.h"
#include "ayfly_s60Application.h"
#include "ayfly_s60AppUi.h"
#include "ayfly_s60AppView.h"
#include "ayfly_s60.rsg"

#endif /*S60_H_*/
