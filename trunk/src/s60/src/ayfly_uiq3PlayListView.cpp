#include "s60.h"

const TUid KUidCayfly_uiq3PlayListView =
{
0x00000001
};

Cayfly_uiq3PlayListView* Cayfly_uiq3PlayListView::NewL(CQikAppUi& aAppUi, const TVwsViewId aParentViewId)
{
    Cayfly_uiq3PlayListView* self = new (ELeave) Cayfly_uiq3PlayListView(aAppUi, aParentViewId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

Cayfly_uiq3PlayListView::Cayfly_uiq3PlayListView(CQikAppUi& aAppUi, const TVwsViewId aParentViewId) :
    CQikViewBase(aAppUi, aParentViewId)
{
    appui = (Cayfly_s60AppUi *)&aAppUi;
}

Cayfly_uiq3PlayListView::~Cayfly_uiq3PlayListView()
{
    if (currentSong)
    {
        ay_closesong(&currentSong);
    }
    if (player)
    {
        delete player;
        player = 0;
    }
    delete iSongArray;
}

void Cayfly_uiq3PlayListView::ConstructL()
{
    // Calls ConstructL that initialises the standard values. 
    // This should always be called in the concrete view implementations.
    BaseConstructL();
    player = new Cayfly_s60Audio;
    iSongArray = new (ELeave) CDesC16ArrayFlat(5);
}

TVwsViewId Cayfly_uiq3PlayListView::ViewId() const
{
    return TVwsViewId(KUidayfly_s60App, KUidCayfly_uiq3PlayListView);
}

void Cayfly_uiq3PlayListView::ViewConstructL()
{
    // Loads information about the UI configurations this view supports
    // together with definition of each view.   
    ViewConstructFromResourceL(R_AYFLY_LISTBOX_CONFIGURATIONS);

    // Get a pointer to the list box
    CQikListBox* listBox = LocateControlByUniqueHandle<CQikListBox>(EAyflyListViewListCtrl);
    // To be able to handle the list box events, HandleListBoxEventL. 
    listBox->SetListBoxObserver(this);

}

void Cayfly_uiq3PlayListView::AddFile(TFileName filePath)
{
    TParse tfp;
    tfp.Set(filePath, NULL, NULL);
    // Get the list box and the list box model
    CQikListBox* listBox = LocateControlByUniqueHandle<CQikListBox>(EAyflyListViewListCtrl);
    MQikListBoxModel& model(listBox->Model());
    model.ModelBeginUpdateLC();

    // Create a listBoxData item, the data returned is Open().
    MQikListBoxData* listBoxData = model.NewDataL(MQikListBoxModel::EDataNormal);
    // Pushes the data onto the cleanup stack. 
    // When CleanupStack::PopAndDestroy() is called, the data will be closed.
    CleanupClosePushL(*listBoxData);

    iSongArray->AppendL(filePath);
    listBoxData->AddTextL(tfp.NameAndExt(), EQikListBoxSlotText1);
    CleanupStack::PopAndDestroy(listBoxData);
    model.ModelEndUpdateL();

}

void Cayfly_uiq3PlayListView::StartPlayer()
{
    if (currentSong && ay_songstarted(currentSong))
    {
        return;
    }
    CQikListBox* listBox = LocateControlByUniqueHandle<CQikListBox>(EAyflyListViewListCtrl);
    MQikListBoxModel& model(listBox->Model());
    if (model.Count() < 1)
        return;
    if (currentIndex < 0)
        currentIndex = 0;
    listBox->SetCurrentItemIndexL(currentIndex, ETrue, ENoDrawNow);
    listBox->DrawNow();
    HandleListBoxEventL(listBox, EEventItemConfirmed, currentIndex, 0);

}

void Cayfly_uiq3PlayListView::StopPlayer()
{

}

void Cayfly_uiq3PlayListView::HandleCommandL(CQikCommand& aCommand)
{

    appui->HandleCommandL(aCommand.Id());

    /*switch(aCommand.Id())
     {
     
     default:
     CQikViewBase::HandleCommandL(aCommand);
     break;
     }*/
}

void Cayfly_uiq3PlayListView::HandleListBoxEventL(CQikListBox* aListBox, TQikListBoxEvent aEventType, TInt aItemIndex, TInt /*aSlotId*/)
{
    switch (aEventType)
    {
    case EEventItemConfirmed:
    case EEventItemTapped:
        {
            if (iSongArray->Count() < 1)
                return;
            currentIndex = aItemIndex;
            StartSong(currentIndex);
            break;
        }
    default:
        break;
    }
}

TKeyResponse Cayfly_uiq3PlayListView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    CQikListBox* listBox = LocateControlByUniqueHandle<CQikListBox>(EAyflyListViewListCtrl);
    
    if(aType == EEventKeyDown)
    {
        if(aKeyEvent.iScanCode == EStdKeyNkp2)
        {
            UpVolume();
            return EKeyWasConsumed;
        }
        else if(aKeyEvent.iScanCode == EStdKeyNkp8)
        {
            DownVolume();
            return EKeyWasConsumed;
        }
        else if(aKeyEvent.iScanCode == EStdKeyBackspace)
        {
            MQikListBoxModel& model(listBox->Model());
            if(model.Count() < 1)
                return EKeyWasConsumed;
            model.ModelBeginUpdateLC();
            
            if(currentIndex == listBox->CurrentItemIndex())
            {
                ay_closesong(&currentSong);
            }
            if(currentIndex >= listBox->CurrentItemIndex())
            {
                currentIndex--;
            }
            iSongArray->Delete(listBox->CurrentItemIndex());
            model.RemoveDataL(listBox->CurrentItemIndex());
            model.ModelEndUpdateL();
            listBox->DrawDeferred();
            return EKeyWasConsumed;

        }
    }
    return (listBox->OfferKeyEventL(aKeyEvent, aType));
}

void Cayfly_uiq3PlayListView::NextSong()
{
    if (currentSong)
    {
        ay_stopsong(currentSong);
        ay_closesong(&currentSong);
        currentIndex++;
        CQikListBox* listBox = LocateControlByUniqueHandle<CQikListBox>(EAyflyListViewListCtrl);
        MQikListBoxModel& model(listBox->Model());

        if (model.Count() <= currentIndex)
        {
            currentIndex = 0;
            return;
        }
        if (currentIndex < 0)
            currentIndex = 0;
        StartSong(currentIndex);
    }
}

void Cayfly_uiq3PlayListView::UpVolume()
{
    if(currentSong && player)
    {
        player->UpDeviceVolume();
    }
}

void Cayfly_uiq3PlayListView::DownVolume()
{
    if(currentSong && player)
    {
        player->DownDeviceVolume();
    }
}

void Cayfly_uiq3PlayListView::stopCallback(void *arg)
{
    Cayfly_uiq3PlayListView *me = (Cayfly_uiq3PlayListView *)arg;
    me->NextSong();
}

bool Cayfly_uiq3PlayListView::elapsedCallback(void *arg)
{
    return true;
}

void Cayfly_uiq3PlayListView::StartSong(TInt index)
{
    TFileName filePath = iSongArray->operator [](index);
    if (currentSong)
    {
        ay_closesong(&currentSong);
    }
    currentSong = ay_initsong(filePath, 44100, player);
    ay_setelapsedcallback(currentSong, Cayfly_uiq3PlayListView::elapsedCallback, this);
    ay_setstoppedcallback(currentSong, Cayfly_uiq3PlayListView::stopCallback, this);
    ay_startsong(currentSong);

}