#ifndef AYFLY_UIQ3PLAYLISTVIEW_H_
#define AYFLY_UIQ3PLAYLISTVIEW_H_

class Cayfly_uiq3PlayListView : public CQikViewBase, MQikListBoxObserver
{
public:
    static Cayfly_uiq3PlayListView* NewL(CQikAppUi& aAppUi, const TVwsViewId aParentViewId);
    ~Cayfly_uiq3PlayListView();

    // from CQikViewBase
    TVwsViewId ViewId() const;
    void HandleCommandL(CQikCommand& aCommand);
    void HandleListBoxEventL(CQikListBox* aListBox, TQikListBoxEvent aEventType, TInt aItemIndex, TInt aSlotId);
    //  from CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    //common
    void AddFile(TFileName filePath);
    void StartPlayer();
    void StopPlayer();
    void PrevSong();
    void NextSong();
    void UpVolume();
    void DownVolume();

protected:
    // from CQikViewBase
    void ViewConstructL();

private:

    Cayfly_uiq3PlayListView(CQikAppUi& aAppUi, const TVwsViewId aParentViewId);
    void ConstructL();
    Cayfly_s60AppUi *appui;
    CDesC16Array* iSongArray;
    void *currentSong;
    float volume;
    TInt currentIndex;
    static bool elapsedCallback(void *arg);
    Cayfly_s60Audio *player;
    static void stopCallback(void *arg);
    void StartSong(TInt index);
};

#endif /*AYFLY_UIQ3PLAYLISTVIEW_H_*/
