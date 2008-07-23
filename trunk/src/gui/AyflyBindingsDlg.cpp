#include "common.h"

BEGIN_EVENT_TABLE(AyflyBindingsDlg, wxDialog)
    EVT_BUTTON(wxID_OK, AyflyBindingsDlg::OnOk)
END_EVENT_TABLE()

struct bindings
{
    wxString name;
    wxString key;
};

bindings default_bindings [] =
{
    {wxT("Open song"), wxT("o")},
    {wxT("Play/Pause"), wxT("x")},
    {wxT("Rewind"), wxT("o")},
    {wxT("Previous song"), wxT("o")},
    {wxT("Next song"), wxT("o")},
    {wxT("Stop"), wxT("o")},
    {wxT("Repeat current"), wxT("r")},
    {wxT("Toggle A channel"), wxT("1")},
    {wxT("Toggle B channel"), wxT("2")},
    {wxT("Toggle C channel"), wxT("3")}

};

AyflyBindingsDlg::AyflyBindingsDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

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
    for (unsigned long i = 0; i < sizeof_array(default_bindings); i++)
    {
        bindingsView->InsertItem(index, default_bindings [i].name);
        bindingsView->SetItem(index, 1, default_bindings [i].key);
        index++;
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
