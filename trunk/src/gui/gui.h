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

#ifndef GUI_H_
#    define GUI_H_

#    ifndef _UNICODE
#        define _UNICODE
#    endif

#    ifdef _WINDOWS
#       ifndef WINDOWS
#           define WINDOWS
#       endif
#    endif

#    ifdef WINDOWS
#       ifndef _WINDOWS
#           define _WINDOWS
#       endif
#        define WIN32_LEAN_AND_MEAN
#    endif


#    ifdef _MSC_VER
#        pragma warning(disable:4309)
#    endif

#include "ayfly.h"

#        include "wx/wx.h"
#        include "wx/file.h"
#        include "wx/textfile.h"
#        include "wx/filedlg.h"
#        include "wx/toolbar.h"
#        include "wx/dnd.h"
#        include "wx/slider.h"
#        include "wx/spinbutt.h"
#        include "wx/tglbtn.h"
#        include "wx/listctrl.h"
#        include "wx/dialog.h"
#        include "wx/filename.h"
#        include "wx/dir.h"
#        include "wx/cmdline.h"

struct bindings
{
    wxString name; /* button name */
    wxString str_id; /* string id for action */
    int id; /* int id for action */
    int key; /* key mapping */
    int modifier; /* modifier */
};


#ifndef sizeof_array
#    define sizeof_array(x) sizeof(x) / sizeof(x [0])
#endif

#        include "AyflyApp.h"
#        include "AyflyFrame.h"
#        include "DnDFiles.h"
#        include "AyflyBindingsDlg.h"

#endif /*COMMON_H_*/
