#include "common.h"

DnDFiles::DnDFiles(AyflyFrame *owner)
{
    m_owner = owner;
};

bool DnDFiles::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    size_t nFiles = filenames.GetCount();
    wxString str;
    str.Printf(wxT("%d files dropped"), (int)nFiles);
    for(size_t n = 0; n < nFiles; n++)
    {        
        m_owner->AddFile(filenames[n]);
    }
    return true;

}
