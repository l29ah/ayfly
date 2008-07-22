#include "common.h"

bool AyflyApp::OnInit()
{
    wxString title = wxT(WINDOW_TEXT);
    AyflyFrame *frame = new AyflyFrame(title.c_str());
    
#ifndef _WIN32_WCE
    wxSize sz = frame->GetSize();
    sz.x = 550;
    sz.y = 400;
    frame->SetSize(sz);
#endif

    frame->Show(true);
    return true;
}
