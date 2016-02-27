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

#include "gui.h"

DnDFiles::DnDFiles(AyflyFrame *owner)
{
    m_owner = owner;
}
;

bool DnDFiles::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    size_t nFiles = filenames.GetCount();
    wxString str;

    for(size_t n = 0; n < nFiles; n++)
    {
        if(wxFileName::DirExists(filenames[n]))
            AddDir(filenames[n]);
        else
            m_owner->AddFile(filenames[n]);
    }
    return true;

}

void DnDFiles::AddDir(const wxString &DirName)
{
    wxArrayString filenames;
    wxDirTraverserSimple traverser(filenames);

    wxDir dir(DirName);
    dir.Traverse(traverser);
    size_t nFiles = filenames.GetCount();
    for(size_t n = 0; n < nFiles; n++)
    {
       m_owner->AddFile(filenames[n]);
    }

}
