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
