#ifndef _AYFLYFRAME_H
#define	_AYFLYFRAME_H

#define WINDOW_TEXT "Ayfly player :-) v." AYFLY_VERSION_TEXT

class AyflyFrame : public wxFrame
{
public:
    AyflyFrame(const wxString &title);
    ~AyflyFrame();

    bool AddFile(const wxString &filePath);
    void Next();

    static void ElapsedCallback(void *arg);

private:
    AbstractAudio *player;
    bool fileOpened;
    wxString defaultDir;
    wxString defaultFileName;
    wxString path;
    wxToolBar *toolBar;
    wxTimer timer;

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

    SongInfo *currentSong;

    bool bTracking;

    long currentIndex;
    wxString currentPath;


    void RecreateToolbar();
    double CalculateVolume(double volume_int);
    bool OpenFile(const wxString &filePath);

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

    DECLARE_EVENT_TABLE()
};

#endif	/* _AYFLYFRAME_H */

