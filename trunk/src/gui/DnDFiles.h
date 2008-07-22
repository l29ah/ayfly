/* 
 * File:   DndFiles.h
 * Author: andrew
 *
 * Created on 26 Май 2008 г., 23:46
 */

#ifndef _DNDFILES_H
#define	_DNDFILES_H

class DnDFiles : public wxFileDropTarget
{
public:
    DnDFiles(AyflyFrame *owner);
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);    
private:
    AyflyFrame *m_owner;
};


#endif	/* _DNDFILES_H */

