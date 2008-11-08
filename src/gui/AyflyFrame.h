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

#ifndef _AYFLYFRAME_H
#define	_AYFLYFRAME_H

#define WINDOW_TEXT "Ayfly AY891x player v." AYFLY_VERSION_TEXT

struct CurrentSong
{
    wxString FilePath;
    AYSongInfo *info;
};

class DnDFiles;

class AyflyFrame : public wxFrame
{
public:
    AyflyFrame(const wxString &title, wxArrayString &filenames);
    ~AyflyFrame();

    bool AddFile(const wxString &filePath);
    void Next();

    static bool ElapsedCallback(void *arg);
    static void StopCallback(void *arg);

private:
    wxString defaultDir;
    wxString defaultFileName;
    wxString path;
    wxToolBar *toolBar;
    wxTimer timer;
    
    DnDFiles *dndFiles;

    wxStaticText* chnlATxt;
    wxStaticText* chnlBTxt;
    wxStaticText* chnlCTxt;
    wxSlider* slidera;
    wxStaticText* txta;
    wxSlider* sliderb;
    wxStaticText* txtb;
    wxSlider* sliderc;
    wxStaticText* txtc;
    wxToggleButton* btnLink;
    wxListView* playListView;
    wxSlider* posslider;
    wxListCtrl* playListCtrl;
    wxSlider* intfreqSlider;
    wxStaticText* txtintfreq;
    wxRadioButton* chipTypeAY;
    wxRadioButton* chipTypeYM;

    CurrentSong *currentSong;

    bool bTracking;

    long currentIndex;
    wxString currentPath;
    bool songEnd;

    void RecreateToolbar();
    float CalculateVolume(float volume_int);
    bool OpenFile();

    void OnQuit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnPlay(wxCommandEvent &event);
    void OnRewind(wxCommandEvent &event);
    void OnPrev(wxCommandEvent &event);
    void OnNext(wxCommandEvent &event);
    void OnStop(wxCommandEvent &event);
    void OnChnlMute(wxCommandEvent &event);
    void OnTimer(wxTimerEvent &event);
    void OnScroll(wxScrollEvent &event);
    void OnSelectSong(wxListEvent &event);
    void OnListKeyDown(wxListEvent &event);
    void OnSelectAll(wxCommandEvent &event);
    void OnSetRepeat(wxCommandEvent &event);
	void OnKeyBindings(wxCommandEvent &event);
	void OnChipSelect(wxCommandEvent &event);
	void OnChar(wxKeyEvent &event);

    DECLARE_EVENT_TABLE()
};

#endif	/* _AYFLYFRAME_H */

