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

#ifndef AYFLY_S60GENTHREAD_H_INCLUDED
#define AYFLY_S60GENTHREAD_H_INCLUDED

_LIT(KThreadName, "GenThread"); // Name of the new thread
class CAsyncTask : public CActive
{
    // Active object class to wrap a long synchronous task
public:
    ~CAsyncTask();
    static CAsyncTask* NewLC(Cayfly_s60Audio *_player);
    static CAsyncTask* NewL(Cayfly_s60Audio *_player);
    // Asynchronous request function
    void DoAsyncTask(TRequestStatus& aStatus);
protected: // From base class
    virtual void DoCancel();
    virtual void RunL();
    virtual TInt RunError(TInt anError);
private:
    CAsyncTask();
    void ConstructL(Cayfly_s60Audio *_player);
    // Thread start function
    static TInt ThreadEntryPoint(TAny* aParameters);
private:
    TRequestStatus* iCaller; // Caller’s request status
    RThread iThread;         // Handle to created thread
    Cayfly_s60Audio *player;

};

#endif // AYFLY_S60GENTHREAD_H_INCLUDED
