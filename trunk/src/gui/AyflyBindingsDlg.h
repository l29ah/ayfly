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

#ifndef AYFLYBINDINGSDLG_H_INCLUDED
#define AYFLYBINDINGSDLG_H_INCLUDED

class AyflyBindingsDlg : public wxDialog {
private:

protected:
    wxListView* bindingsView;
    wxStdDialogButtonSizer* buttonsSizer;
    wxButton* buttonsSizerOK;

    // Virtual event handlers, overide them in your derived class
    virtual void OnListKeyDown(wxListEvent& event){ event.Skip(); }
    virtual void OnOk(wxCommandEvent& event);


public:
    AyflyBindingsDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Ayfly key bindings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(536,260), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP|wxCLIP_CHILDREN);
    ~AyflyBindingsDlg();
private:

    DECLARE_EVENT_TABLE()

};

#endif // AYFLYBINDINGSDLG_H_INCLUDED
