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

BEGIN_EVENT_TABLE(AyflyBindingsDlg, wxDialog)
    EVT_BUTTON(wxID_OK, AyflyBindingsDlg::OnOk)
END_EVENT_TABLE()

extern struct bindings default_bindings [];

AyflyBindingsDlg::AyflyBindingsDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(400, 300);

    wxBoxSizer* allSizer;
    allSizer = new wxBoxSizer(wxVERTICAL);

    bindingsView = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_VRULES|wxLC_HRULES);
    allSizer->Add(bindingsView, 1, wxALL|wxEXPAND, 5);

    buttonsSizer = new wxStdDialogButtonSizer();
    buttonsSizerOK = new wxButton(this, wxID_OK);
    buttonsSizer->AddButton(buttonsSizerOK);
    buttonsSizer->Realize();
    allSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT, 5);

    this->SetSizer(allSizer);
    this->Layout();

    // Connect Events
    bindingsView->Connect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(AyflyBindingsDlg::OnListKeyDown), NULL, this);
    buttonsSizerOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AyflyBindingsDlg::OnOk), NULL, this);


    wxRect sz = bindingsView->GetRect();

    wxListItem itemCol;
    itemCol.SetText(wxT("Name"));
    bindingsView->InsertColumn(0, itemCol);
    long col0_width = (sz.GetWidth() * 80) / 100;
    bindingsView->SetColumnWidth(0, col0_width);

    itemCol.SetText(wxT("Key"));
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    bindingsView->InsertColumn(1, itemCol);
    bindingsView->SetColumnWidth(1, sz.GetWidth() - col0_width - 15);

    unsigned long index = bindingsView->GetItemCount();
    int i = 0;
    while(default_bindings [i].key != 0)
    {
        bindingsView->InsertItem(index, default_bindings [i].name);
        wxString buf;
		wxString mod = wxEmptyString;
		if(default_bindings [i].modifier == wxACCEL_CTRL)
		{
			mod = wxT("CTRL + ");
		}
        buf.Printf(wxT("%c"), default_bindings [i].key);
        bindingsView->SetItem(index, 1, mod + buf);
        index++;
        i++;
    }

}

AyflyBindingsDlg::~AyflyBindingsDlg()
{
    // Disconnect Events
    bindingsView->Disconnect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(AyflyBindingsDlg::OnListKeyDown), NULL, this);
    buttonsSizerOK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AyflyBindingsDlg::OnOk), NULL, this);
}

void AyflyBindingsDlg::OnOk(wxCommandEvent &event)
{
    EndModal(1);
}
