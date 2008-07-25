/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew                      *
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

#include "common.h"

#include "images/open.xpm"
#include "images/play.xpm"
#include "images/stop.xpm"
#include "images/pause.xpm"
#include "images/rewind.xpm"
#include "images/prev.xpm"
#include "images/next.xpm"
#include "images/A.xpm"
#include "images/B.xpm"
#include "images/C.xpm"
#include "images/about.xpm"
#include "images/repeat.xpm"
#include "images/icon.xpm"
#include "images/keys.xpm"

IMPLEMENT_APP(AyflyApp)

#define wxID_OPEN 1000
#define wxID_PLAY 1001
#define wxID_REWIND 1002
#define wxID_PREV 1003
#define wxID_NEXT 1004
#define wxID_STOP 1005
#define wxID_REPEAT 1006
#define wxID_KEYS 1007


#define wxID_AMUTE 1014
#define wxID_BMUTE 1015
#define wxID_CMUTE 1016

#define wxID_CALLBACK 2000

#define SLIDER_VOLA_ID 1020
#define SLIDER_VOLB_ID 1021
#define SLIDER_VOLC_ID 1022

#define wxID_POSSLIDER 1030

#define wxID_SELECTALL 1100
#define wxID_SETREPEAT 1101

#define TIMER_ID 1040
#define TIMER_INTERVAL 200

#define BUTTON_LINK_ID 1

#define PLAYLIST_ID 1010

BEGIN_EVENT_TABLE(AyflyFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, AyflyFrame::OnAbout)
    EVT_MENU(wxID_EXIT, AyflyFrame::OnQuit)
    EVT_MENU(wxID_OPEN, AyflyFrame::OnOpen)
    EVT_MENU(wxID_PLAY, AyflyFrame::OnPlay)
    EVT_MENU(wxID_REWIND, AyflyFrame::OnRewind)
    EVT_MENU(wxID_PREV, AyflyFrame::OnPrev)
    EVT_MENU(wxID_NEXT, AyflyFrame::OnNext)
    EVT_MENU(wxID_STOP, AyflyFrame::OnStop)
    EVT_MENU(wxID_AMUTE, AyflyFrame::OnChnlMute)
    EVT_MENU(wxID_BMUTE, AyflyFrame::OnChnlMute)
    EVT_MENU(wxID_CMUTE, AyflyFrame::OnChnlMute)
    EVT_MENU(wxID_SELECTALL, AyflyFrame::OnSelectAll)
    EVT_MENU(wxID_SETREPEAT, AyflyFrame::OnSetRepeat)
	EVT_MENU(wxID_KEYS, AyflyFrame::OnKeyBindings)
    EVT_TIMER(TIMER_ID, AyflyFrame::OnTimer)
    EVT_COMMAND_SCROLL(SLIDER_VOLA_ID, AyflyFrame::OnScroll)
    EVT_COMMAND_SCROLL(SLIDER_VOLB_ID, AyflyFrame::OnScroll)
    EVT_COMMAND_SCROLL(SLIDER_VOLC_ID, AyflyFrame::OnScroll)
    EVT_COMMAND_SCROLL_THUMBTRACK(wxID_POSSLIDER, AyflyFrame::OnScroll)
    EVT_COMMAND_SCROLL_THUMBRELEASE(wxID_POSSLIDER, AyflyFrame::OnScroll)
    EVT_LIST_ITEM_ACTIVATED(PLAYLIST_ID, AyflyFrame::OnSelectSong)
    //EVT_LIST_KEY_DOWN(PLAYLIST_ID, AyflyFrame::OnListKeyDown)
	//EVT_CHAR(AyflyFrame::OnChar)
END_EVENT_TABLE()


#ifdef WINDOWS
HWND hWndMain;
#endif

unsigned long timeElapsed;
unsigned long maxElapsed;

struct bindings default_bindings [] =
{
    {wxT("Open song"), wxT("wxID_OPEN"), wxID_OPEN, (int)'O', wxACCEL_NORMAL},
    {wxT("Play/Pause"), wxT("wxID_PLAY"), wxID_PLAY, (int)'X', wxACCEL_NORMAL},
    {wxT("Rewind"), wxT("wxID_REWIND"), wxID_REWIND, (int)'W', wxACCEL_NORMAL},
    {wxT("Previous song"), wxT("wxID_PREV"), wxID_PREV, (int)'Z', wxACCEL_NORMAL},
    {wxT("Next song"), wxT("wxID_NEXT"), wxID_NEXT, (int)'B', wxACCEL_NORMAL},
    {wxT("Stop"), wxT("wxID_STOP"), wxID_STOP, (int)'V', wxACCEL_NORMAL},
    {wxT("Toggle Repeat mode"), wxT("wxID_SETREPEAT"), wxID_SETREPEAT, (int)'R', wxACCEL_NORMAL},
    {wxT("Toggle A channel"), wxT("wxID_AMUTE"), wxID_AMUTE, (int)'1', wxACCEL_NORMAL},
    {wxT("Toggle B channel"), wxT("wxID_BMUTE"), wxID_BMUTE, (int)'2', wxACCEL_NORMAL},
    {wxT("Toggle C channel"), wxT("wxID_CMUTE"), wxID_CMUTE, (int)'3', wxACCEL_NORMAL},
    {wxT("Select all playlist items"), wxT("wxID_SELECTALL"), wxID_SELECTALL, (int)'A', wxACCEL_CTRL},
    {wxT(""), wxT(""), 0, 0, 0}
};

AyflyFrame::AyflyFrame(const wxString &title) :
        wxFrame(NULL, wxID_ANY, title), timer(this, TIMER_ID)
{
    SetBackgroundStyle(wxBG_STYLE_COLOUR);
    SetBackgroundColour(wxColour(0xef, 0xeb, 0xe7));
    SetIcon(wxIcon(Icon_xpm));
    CreateStatusBar(2);
    SetStatusText(wxT("Welcome to Ayfly!"));
    player = 0;
    toolBar = 0;
    fileOpened = false;
    defaultDir = wxEmptyString;
    defaultFileName = wxEmptyString;
    path = wxEmptyString;
    timeElapsed = 0;
    currentIndex = 0;
    currentSong = 0;
    bTracking = false;
#ifdef _WIN32_WCE
    player = new WaveAudio(AUDIO_FREQ);
#else
#    ifdef WINDOWS
    hWndMain = (HWND) GetHandle();
    player = new DXAudio(AUDIO_FREQ);
#    else
    player = new SDLAudio(AUDIO_FREQ);
#    endif
#endif
    player->SetCallback(AyflyFrame::ElapsedCallback, this);
    setPlayer(player);

    SetDropTarget(new DnDFiles(this));

#ifdef WINDOWS
    this->SetSizeHints(620, 400);
#else
    this->SetSizeHints(700, 400);
#endif

    wxBoxSizer* allSizer;
    allSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* volumeAllSizer;
    volumeAllSizer = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Volume levels")), wxHORIZONTAL);

    wxBoxSizer* chnlLblSizer;
    chnlLblSizer = new wxBoxSizer(wxVERTICAL);

    chnlATxt = new wxStaticText(this, wxID_ANY, wxT("A:"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    chnlATxt->Wrap(-1);
    chnlATxt->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

    chnlLblSizer->Add(chnlATxt, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

    chnlBTxt = new wxStaticText(this, wxID_ANY, wxT("B:"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    chnlBTxt->Wrap(-1);
    chnlBTxt->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

    chnlLblSizer->Add(chnlBTxt, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

    chnlCTxt = new wxStaticText(this, wxID_ANY, wxT("C:"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
    chnlCTxt->Wrap(-1);
    chnlCTxt->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

    chnlLblSizer->Add(chnlCTxt, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

    volumeAllSizer->Add(chnlLblSizer, 0, wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL | wxTOP | wxBOTTOM, 5);

    wxGridSizer* volumeSizer;
    volumeSizer = new wxGridSizer(3, 1, 0, 0);

    slidera = new wxSlider(this, SLIDER_VOLA_ID, 0, -40, 0, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    volumeSizer->Add(slidera, 1, wxALL | wxEXPAND, 5);

    sliderb = new wxSlider(this, SLIDER_VOLB_ID, 0, -40, 0, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    volumeSizer->Add(sliderb, 0, wxEXPAND | wxALL, 5);

    sliderc = new wxSlider(this, SLIDER_VOLC_ID, 0, -40, 0, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    volumeSizer->Add(sliderc, 0, wxEXPAND | wxALL, 5);

    volumeAllSizer->Add(volumeSizer, 1, wxTOP | wxBOTTOM, 5);

    wxGridSizer* volumeTextSizer;
    volumeTextSizer = new wxGridSizer(3, 1, 0, 0);

    txta = new wxStaticText(this, wxID_ANY, wxT("0dB"), wxPoint(-1, -1), wxDefaultSize, 0);
    txta->Wrap(-1);
    volumeTextSizer->Add(txta, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxTOP | wxBOTTOM, 5);

    txtb = new wxStaticText(this, wxID_ANY, wxT("0dB"), wxDefaultPosition, wxDefaultSize, 0);
    txtb->Wrap(-1);
    volumeTextSizer->Add(txtb, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxTOP | wxBOTTOM, 5);

    txtc = new wxStaticText(this, wxID_ANY, wxT("0dB"), wxDefaultPosition, wxDefaultSize, 0);
    txtc->Wrap(-1);
    volumeTextSizer->Add(txtc, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxTOP | wxBOTTOM, 5);

    volumeAllSizer->Add(volumeTextSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    wxBoxSizer* btnLinkSizer;
    btnLinkSizer = new wxBoxSizer(wxVERTICAL);

    btnLink = new wxToggleButton(this, BUTTON_LINK_ID, wxT("-Link-"), wxDefaultPosition, wxDefaultSize, 0);
    btnLink->SetValue(true);
    btnLinkSizer->Add(btnLink, 0, wxALIGN_CENTER_VERTICAL, 5);

    volumeAllSizer->Add(btnLinkSizer, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT, 10);

    allSizer->Add(volumeAllSizer, 0, wxEXPAND, 5);

    wxBoxSizer* PosSizer;
    PosSizer = new wxBoxSizer(wxVERTICAL);

    posslider = new wxSlider(this, wxID_POSSLIDER, 0, 0, 0, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    posslider->Enable(false);

    PosSizer->Add(posslider, 0, wxALL | wxEXPAND, 5);

    allSizer->Add(PosSizer, 0, wxEXPAND, 5);

    playListView = new wxListView(this, PLAYLIST_ID, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES);
    allSizer->Add(playListView, 1, wxALL | wxEXPAND, 5);

    this->SetSizer(allSizer);
    this->Layout();

    wxRect sz = playListView->GetRect();

    wxListItem itemCol;
    itemCol.SetText(wxT("Song Name"));
    playListView->InsertColumn(0, itemCol);
    long col0_width = (sz.GetWidth() * 80) / 100;
    playListView->SetColumnWidth(0, col0_width);

    itemCol.SetText(wxT("Length"));
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    playListView->InsertColumn(1, itemCol);
    playListView->SetColumnWidth(1, sz.GetWidth() - col0_width);

	RecreateToolbar();

    wxAcceleratorEntry accel_entries[sizeof_array(default_bindings) - 1];
    int i = 0;

    while (default_bindings [i].id != 0)
    {
        accel_entries[i].Set(default_bindings [i].modifier, default_bindings [i].key, default_bindings [i].id);
        i++;
    }

    wxAcceleratorTable accel(sizeof_array(default_bindings) - 1, accel_entries);
    SetAcceleratorTable(accel);
}

AyflyFrame::~AyflyFrame()
{
    if (player)
    {
        if (player->Started())
            player->Stop();

        delete player;

        player = 0;

        setPlayer(player);
    }
}

void AyflyFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(wxT("Programming: Andrew Deryabin, 2008\n Icons,Graphics,Testing&Bug reports:\n Alexander Shatin, 2008\n  Using z80ex library by Stanislav Lomakin."), wxT("About Ayfly.."), wxOK | wxICON_INFORMATION, this);
}

void AyflyFrame::OnQuit(wxCommandEvent &event)
{
    Close();
}

void AyflyFrame::OnOpen(wxCommandEvent &event)
{
    wxString caption = wxT("Select AY file");
    wxString filter = wxT("All AY files (*.stc;*.stp;*.pt2;*.pt3;*.psc;*.ay)|*.stc;*.stp;*.pt2;*.pt3;*.psc;*.ay|Sound tracker (*.stc)|*.stc|Sound tracker pro (*.stp)|*.stp|Pro tracker 2.x (*.pt2)|*.pt2|Pro tracker 3.x (*.pt3)|*.pt3|Pro sound creator (*.psc)|*.psc|AY dumps (*.ay)|*.ay");

    wxFileDialog dialog(this, caption, defaultDir, defaultFileName, filter, wxFD_OPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        path = dialog.GetPath();
#ifndef WINDOWS
        defaultFileName = dialog.GetFilename();
#endif
        defaultDir = dialog.GetDirectory();
        AddFile(path);
    }
}

void AyflyFrame::OnPlay(wxCommandEvent &event)
{
    playListView->Select(currentIndex);
    playListView->EnsureVisible(currentIndex);

    if (currentSong)
        if (currentSong->Length)
            maxElapsed = currentSong->Length;

    posslider->SetRange(0, maxElapsed);


    if (fileOpened)
    {
        if (player->Started())
        {
            if (timer.IsRunning())
                timer.Stop();

            player->Stop();

            RecreateToolbar();
        }
        else
        {
            if (timer.IsRunning())
                timer.Stop();

            player->Start();

            timer.Start(TIMER_INTERVAL);

            RecreateToolbar();
        }
    }
    else
    {
        if (currentIndex >= playListView->GetItemCount())
            return;

        currentSong = (SongInfo *) playListView->GetItemData(currentIndex);

        wxString *filePath = currentSong->FilePath;

        timeElapsed = 0;

        if (filePath)
        {
            if (OpenFile(*filePath))
            {
                wxCommandEvent evt;
                OnPlay(evt);
            }
        }

    }

    posslider->SetValue(timeElapsed);

    if (player->Started())
    {
        posslider->Enable();
    }
    else
    {
        posslider->Disable();
    }
}

void AyflyFrame::OnRewind(wxCommandEvent &event)
{
    if (fileOpened)
    {
        posslider->Disable();

        if (timer.IsRunning())
            timer.Stop();

        timeElapsed = 0;

        bool started = player->Started();

        if (started)
        {
            player->Stop();
        }

        if (path != wxEmptyString)
            readFile(path);

        resetSpeccy();

        if (started)
        {
            posslider->Enable();
            player->Start();
            timer.Start(TIMER_INTERVAL);
        }
    }

}

void AyflyFrame::OnPrev(wxCommandEvent &event)
{
    if (playListView->GetItemCount() == 0)
        return;

    playListView->Select(currentIndex, false);

    playListView->EnsureVisible(currentIndex);

    currentIndex--;

    if (currentIndex < 0)
    {
        currentIndex = 0;
        wxCommandEvent evt;
        OnStop(evt);
        return;
    }

    bool started = player->Started();

    wxListEvent evt;
    playListView->Select(currentIndex);
    playListView->EnsureVisible(currentIndex);
    currentSong = (SongInfo *) playListView->GetItemData(currentIndex);
    wxString *filePath = currentSong->FilePath;
    timeElapsed = 0;

    if (filePath)
    {
        if (OpenFile(*filePath))
        {
            if (started)
            {
                wxCommandEvent evt;
                OnPlay(evt);
            }
        }
    }
}

void AyflyFrame::OnNext(wxCommandEvent &event)
{
    if (playListView->GetItemCount() == 0)
        return;

    playListView->Select(currentIndex, false);

    playListView->EnsureVisible(currentIndex);

    currentIndex++;

    if (currentIndex >= playListView->GetItemCount())
    {
        currentIndex = 0;
        wxCommandEvent evt;
        OnStop(evt);
        currentSong = (SongInfo *) playListView->GetItemData(currentIndex);
        wxString *filePath = currentSong->FilePath;
        OpenFile(*filePath);
        return;
    }

    bool started = player->Started();

    if (event.GetId() == wxID_CALLBACK)
        started = true;

    wxListEvent evt;

    playListView->Select(currentIndex);

    playListView->EnsureVisible(currentIndex);

    currentSong = (SongInfo *) playListView->GetItemData(currentIndex);

    wxString *filePath = currentSong->FilePath;

    timeElapsed = 0;

    if (filePath)
    {
        if (OpenFile(*filePath))
        {
            if (started)
            {
                wxCommandEvent evt;
                OnPlay(evt);
            }
        }
    }

}

void AyflyFrame::OnStop(wxCommandEvent &event)
{
    if (fileOpened)
    {
        posslider->Disable();
        posslider->SetValue(0);
        posslider->SetRange(0, 1);

        if (timer.IsRunning())
            timer.Stop();

        timeElapsed = 0;

        maxElapsed = 4 * 60 * 50;

        if (currentSong)
            if (currentSong->Length)
                maxElapsed = currentSong->Length;

        if (player->Started())
        {
            player->Stop();
            RecreateToolbar();
        }

        if (path != wxEmptyString)
            readFile(path);

        resetSpeccy();
    }
}

void AyflyFrame::OnChnlMute(wxCommandEvent &event)
{
    switch (event.GetId())
    {
        case wxID_AMUTE:
            if (player)
                player->ChnlToggle(0);

            break;

        case wxID_BMUTE:
            if (player)
                player->ChnlToggle(1);

            break;

        case wxID_CMUTE:
            if (player)
                player->ChnlToggle(2);

            break;

        default:
            return;
    }

    RecreateToolbar();
}

void AyflyFrame::OnTimer(wxTimerEvent& event)
{
    //timeElapsed++;

    if (timeElapsed >= maxElapsed)
    {
        wxCommandEvent evt;
        evt.SetId(wxID_CALLBACK);
        OnNext(evt);
        return;
    }

    if (!bTracking)
        posslider->SetValue(timeElapsed);

    unsigned long elapsedSeconds = timeElapsed / 50;

    unsigned long tm = elapsedSeconds;

    unsigned long hours = tm / 3600;

    tm %= 3600;

    unsigned long minutes = tm / 60;

    unsigned long seconds = tm % 60;

    wxString strTime;

    strTime.Printf(wxT("Elapsed: %.2lu:%.2lu:%.2lu"), hours, minutes, seconds);

    SetStatusText(strTime, 1);

}

void AyflyFrame::ElapsedCallback(void *arg)
{
    AyflyFrame *frame = (AyflyFrame *) arg;
    //frame->timer.Stop();
    wxCommandEvent evt;

    if (frame->toolBar->GetToolState(wxID_REPEAT))
    {
        timeElapsed = frame->currentSong->Loop;
        //frame->timer.Start(TIMER_INTERVAL);
        return;
    }

    //frame->timer.Start(TIMER_INTERVAL);
}

void AyflyFrame::OnScroll(wxScrollEvent &event)
{
    int id = event.GetId();

    switch (id)
    {
        case SLIDER_VOLA_ID:
        case SLIDER_VOLB_ID:
        case SLIDER_VOLC_ID:
        {
            double vol_int = event.GetPosition();
            double vol = CalculateVolume(vol_int);

            if (btnLink->GetValue())
            {
                if (player)
                {
                    player->SetVolume(0, vol);
                    player->SetVolume(1, vol);
                    player->SetVolume(2, vol);
                }

                slidera->SetValue(vol_int);

                sliderb->SetValue(vol_int);
                sliderc->SetValue(vol_int);
            }
            else
                if (player)
                {
                    switch (id)
                    {
                        case SLIDER_VOLA_ID:
                            player->SetVolume(0, vol);
                            break;
                        case SLIDER_VOLB_ID:
                            player->SetVolume(1, vol);
                            break;
                        case SLIDER_VOLC_ID:
                            player->SetVolume(2, vol);
                            break;
                        default:
                            break;
                    }
                }

            wxString vol_str;

            vol_str.Printf(wxT("%d dB"), slidera->GetValue());
            txta->SetLabel(vol_str);
            vol_str.Printf(wxT("%d dB"), sliderb->GetValue());
            txtb->SetLabel(vol_str);
            vol_str.Printf(wxT("%d dB"), sliderc->GetValue());
            txtc->SetLabel(vol_str);

        }

        break;
        case wxID_POSSLIDER:
        {
            if (event.GetEventType() == wxEVT_SCROLL_THUMBTRACK)
            {
                bTracking = true;
            }
            else
            {
                unsigned long pos = event.GetPosition();
                unsigned long timeCurrent = timeElapsed;
                timeElapsed = pos;

                if (timeElapsed < timeCurrent)
                {
                    timeCurrent = 0;
                    OpenFile(*currentSong->FilePath);
                }
                else
                {
                    wxCommandEvent evt;
                    OnPlay(evt);
                }

                timeElapsed = timeCurrent;

                z80Memory[7] = 0xfb;

                while (timeElapsed != pos)
                {
                    z80ex_step(ctx);

                    if (z80ex_get_reg(ctx, regPC) == 4)
                        timeElapsed++;
                }

                wxCommandEvent evt;

                OnPlay(evt);
                z80Memory[7] = 0x76;
                bTracking = false;
            }

        }

        break;
        default:
            break;
    }

}

void AyflyFrame::OnSelectSong(wxListEvent &event)
{
    currentIndex = event.GetIndex();
    currentSong = (SongInfo *) playListView->GetItemData(currentIndex);
    wxString *filePath = currentSong->FilePath;

    if (filePath)
    {
        if (OpenFile(*filePath))
        {
            wxCommandEvent evt;
            OnPlay(evt);
        }
    }
}

void AyflyFrame::OnListKeyDown(wxListEvent &event)
{
    int key = event.GetKeyCode();

    if (key == WXK_DELETE)
    {
        long cnt_sel = playListView->GetSelectedItemCount();

        if (cnt_sel > 0)
        {
            long index = playListView->GetFirstSelected();

            while (index != -1)
            {
                SongInfo *deletedSong = (SongInfo *) playListView->GetItemData(index);
                wxString *filePath = deletedSong->FilePath;

                if (deletedSong == currentSong)
                {
                    wxCommandEvent evt;
                    OnStop(evt);
                    currentIndex = 0;
                    currentPath == wxT("");
                    currentSong = 0;
                }

                delete filePath;

                delete deletedSong;
                playListView->DeleteItem(index);
                index--;

                if (index == -1)
                    index = playListView->GetFirstSelected();
                else
                    index = playListView->GetNextSelected(index);
            }
        }
    }
}

void AyflyFrame::OnSelectAll(wxCommandEvent &event)
{
    for (int i = 0; i < playListView->GetItemCount(); i++)
    {
        playListView->Select(i, true);
    }
}

void AyflyFrame::OnSetRepeat(wxCommandEvent &event)
{
    if(toolBar)
        toolBar->ToggleTool(wxID_REPEAT, 1 - toolBar->GetToolState(wxID_REPEAT));
}

void AyflyFrame::OnKeyBindings(wxCommandEvent &event)
{
	AyflyBindingsDlg kdlg(this);
	kdlg.SetIcon(wxIcon(Icon_xpm));
	kdlg.ShowModal();
}


void AyflyFrame::RecreateToolbar()
{
    //SetToolBar(NULL);
    bool bCreate = toolBar != 0 ? false : true;
    bool a_muted = player ? player->ChnlMuted(0) : true;
    bool b_muted = player ? player->ChnlMuted(1) : true;
    bool c_muted = player ? player->ChnlMuted(2) : true;
    bool started = player ? player->Started() : false;

    if (toolBar == 0)
    {
        toolBar = CreateToolBar(wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT);
        toolBar->SetToolBitmapSize(wxSize(40, 32));
		toolBar->SetMargins(0, 0);
		toolBar->SetToolSeparation(0);

        wxBitmap bmpOpen(Open_xpm);
        toolBar->AddTool(wxID_OPEN, bmpOpen, wxT("Open"));
        toolBar->AddSeparator();

        if (started)
        {
            wxBitmap bmpPlay(Pause_xpm);
            toolBar->AddTool(wxID_PLAY, wxT("Play"), bmpPlay, wxT("Pause song"), wxITEM_CHECK);
        }
        else
        {
            wxBitmap bmpPlay(Play_xpm);
            toolBar->AddTool(wxID_PLAY, wxT("Play"), bmpPlay, wxT("Play song"), wxITEM_CHECK);
        }

        wxBitmap bmpStop(Stop_xpm);

        wxBitmap bmpRewind(Rewind_xpm);
        wxBitmap bmpPrev(Prev_xpm);
        wxBitmap bmpNext(Next_xpm);
        wxBitmap bmpRepeat(Repeat_xpm);

        toolBar->AddTool(wxID_REWIND, bmpRewind, wxT("Rewind"));
        toolBar->AddTool(wxID_PREV, bmpPrev, wxT("Previous"));
        toolBar->AddTool(wxID_NEXT, bmpNext, wxT("Next"));
        toolBar->AddTool(wxID_STOP, bmpStop, wxT("Stop"));
        toolBar->AddTool(wxID_REPEAT, wxT("Repeat"), bmpRepeat, wxT("Repeat current song"), wxITEM_CHECK);

        toolBar->AddSeparator();

        wxBitmap bmpA(A_xpm);
        wxBitmap bmpB(B_xpm);
        wxBitmap bmpC(C_xpm);

        toolBar->AddTool(wxID_AMUTE, wxT("Mute A"), bmpA, a_muted ? wxT("Enable A channel") : wxT("Mute A channel"), wxITEM_CHECK);
        toolBar->AddTool(wxID_BMUTE, wxT("Mute B"), bmpB, b_muted ? wxT("Enable B channel") : wxT("Mute B channel"), wxITEM_CHECK);
        toolBar->AddTool(wxID_CMUTE, wxT("Mute C"), bmpC, c_muted ? wxT("Enable C channel") : wxT("Mute C channel"), wxITEM_CHECK);

        toolBar->AddSeparator();

		wxBitmap bmpKeys(Keys_xpm);
        toolBar->AddTool(wxID_KEYS, bmpKeys, wxT("Key bindings"));

		toolBar->AddSeparator();

        wxBitmap bmpAbout(About_xpm);
        toolBar->AddTool(wxID_ABOUT, bmpAbout, wxT("About"));

        toolBar->SetToolShortHelp(wxID_OPEN, wxT("Open song"));
        toolBar->SetToolLongHelp(wxID_OPEN, wxT("Open song"));

        toolBar->SetToolShortHelp(wxID_REWIND, wxT("Rewind song"));
        toolBar->SetToolLongHelp(wxID_REWIND, wxT("Rewind song"));

        toolBar->SetToolShortHelp(wxID_PREV, wxT("Previous song"));
        toolBar->SetToolLongHelp(wxID_PREV, wxT("Previous song"));

        toolBar->SetToolShortHelp(wxID_NEXT, wxT("Next song"));
        toolBar->SetToolLongHelp(wxID_NEXT, wxT("Next song"));

        toolBar->SetToolShortHelp(wxID_STOP, wxT("Stop song"));
        toolBar->SetToolLongHelp(wxID_STOP, wxT("Stop song"));

        toolBar->SetToolShortHelp(wxID_REPEAT, wxT("Repeat current song"));
        toolBar->SetToolLongHelp(wxID_REPEAT, wxT("Repeat current song"));

        toolBar->SetToolShortHelp(wxID_ABOUT, wxT("About Ayfly.."));
        toolBar->SetToolLongHelp(wxID_ABOUT, wxT("About Ayfly.."));

		toolBar->SetToolShortHelp(wxID_KEYS, wxT("Configure key bindings"));
        toolBar->SetToolLongHelp(wxID_KEYS, wxT("Configure key bindings"));

    }
    else
    {

        if (started)
        {
            wxBitmap bmpPlay(Pause_xpm);
            toolBar->SetToolNormalBitmap(wxID_PLAY, bmpPlay);
            toolBar->SetToolShortHelp(wxID_PLAY, wxT("Pause"));
            toolBar->SetToolLongHelp(wxID_PLAY, wxT("Pause song"));
        }
        else
        {
            wxBitmap bmpPlay(Play_xpm);
            toolBar->SetToolNormalBitmap(wxID_PLAY, bmpPlay);
            toolBar->SetToolShortHelp(wxID_PLAY, wxT("Play"));
            toolBar->SetToolLongHelp(wxID_PLAY, wxT("Play song"));
        }

    }

    toolBar->SetToolShortHelp(wxID_AMUTE, a_muted ? wxT("Enable A") : wxT("Mute A"));

    toolBar->SetToolLongHelp(wxID_AMUTE, a_muted ? wxT("Enable A channel") : wxT("Mute A channel"));

    toolBar->SetToolShortHelp(wxID_BMUTE, b_muted ? wxT("Enable B") : wxT("Mute B"));
    toolBar->SetToolLongHelp(wxID_BMUTE, b_muted ? wxT("Enable B channel") : wxT("Mute B channel"));

    toolBar->SetToolShortHelp(wxID_CMUTE, b_muted ? wxT("Enable C") : wxT("Mute C"));
    toolBar->SetToolLongHelp(wxID_CMUTE, c_muted ? wxT("Enable C channel") : wxT("Mute C channel"));

    toolBar->EnableTool(wxID_PLAY, playListView->GetItemCount());
    toolBar->EnableTool(wxID_REWIND, playListView->GetItemCount());
    toolBar->EnableTool(wxID_PREV, true);
    toolBar->EnableTool(wxID_NEXT, true);
    toolBar->EnableTool(wxID_STOP, playListView->GetItemCount());

    toolBar->ToggleTool(wxID_PLAY, started);

    toolBar->EnableTool(wxID_AMUTE, true);
    toolBar->EnableTool(wxID_BMUTE, true);
    toolBar->EnableTool(wxID_CMUTE, true);

    toolBar->ToggleTool(wxID_AMUTE, !a_muted);
    toolBar->ToggleTool(wxID_BMUTE, !b_muted);
    toolBar->ToggleTool(wxID_CMUTE, !c_muted);

    if (bCreate)
    {
        toolBar->Realize();
        SetToolBar(toolBar);
    }
}

double AyflyFrame::CalculateVolume(double volume_int)
{
    return pow(10, volume_int / 20);
}

bool AyflyFrame::OpenFile(const wxString &filePath)
{
    if (player)
    {
        fileOpened = false;

        if (timer.IsRunning())
            timer.Stop();

        player->Stop();

        timeElapsed = 0;

        maxElapsed = 4 * 60 * 50;

        if (currentSong)
            if (currentSong->Length)
                maxElapsed = currentSong->Length;

        posslider->SetRange(0, maxElapsed);

        shutdownSpeccy();

        initSpeccy();

        currentPath = filePath;

        if (readFile(filePath))
        {
            fileOpened = true;
            wxString fileName;
            wxString fileExt;
            wxSplitPath(filePath, NULL, &fileName, &fileExt);
            wxString frameTitle = wxString(wxT(WINDOW_TEXT)) + wxString(wxT(" - ")) + fileName + wxT(".") + fileExt;
            SetTitle(frameTitle.c_str());
            RecreateToolbar();
            return true;
        }
    }

    return false;
}

bool AyflyFrame::AddFile(const wxString &filePath)
{
    wxString fileName;
    wxString fileExt;
    wxSplitPath(filePath, NULL, &fileName, &fileExt);
    wxString buf = fileName + wxT(".") + fileExt;
    SongInfo *info = new SongInfo;
    info->FilePath = new wxString(filePath);
    getSongInfo(info);

    if (info->Length == 0)
    {
        return false;
    }

    long index = playListView->GetItemCount();

    playListView->InsertItem(index, buf);
    unsigned long seconds = info->Length / 50;
    unsigned long minutes = seconds / 60;
    seconds = seconds % 60;
    buf.Printf(wxT("00:%.2lu:%.2lu"), minutes, seconds);
    playListView->SetItemData(index, (long) info);
    playListView->SetItem(index, 1, buf);
    toolBar->EnableTool(wxID_PLAY, true);
    toolBar->EnableTool(wxID_REWIND, true);
    toolBar->EnableTool(wxID_STOP, true);
    return true;
}
