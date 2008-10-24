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

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
{ wxCMD_LINE_PARAM, NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
{ wxCMD_LINE_NONE } };

bool AyflyApp::OnInit()
{
    wxCmdLineParser cmdParser(g_cmdLineDesc, argc, argv);
    int res;
    {
        wxLogNull log;
        // Pass false to suppress auto Usage() message
        res = cmdParser.Parse(false);
    }
    int fileCount = cmdParser.GetParamCount();
    wxArrayString filenames;
    if(fileCount > 0)
    {        
        for(int i = 0; i < fileCount; i++)
        {
            wxString cmdFilename = cmdParser.GetParam(i);
            wxFileName fName(cmdFilename);
            fName.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);
            cmdFilename = fName.GetFullPath();
            filenames.Add(cmdFilename);
        }        
    }
    wxString title = wxT(WINDOW_TEXT);
    AyflyFrame *frame = new AyflyFrame(title.c_str(), filenames);

#ifndef _WIN32_WCE
    wxSize sz = frame->GetSize();
    sz.x = 550;
    sz.y = 400;
    frame->SetSize(sz);
#endif

    frame->Show(true);
    return true;
}
