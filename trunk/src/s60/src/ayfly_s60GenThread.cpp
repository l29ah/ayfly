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

#include "s60.h"

CAsyncTask::CAsyncTask()
        : CActive(EPriorityStandard) // Standard priority unless good reason
{
    // Add to the active scheduler
    CActiveScheduler::Add(this);
}
// Two-phase construction code omitted for clarity
CAsyncTask::~CAsyncTask()
{
    // Cancel any outstanding request before cleanup
    Cancel(); // Calls DoCancel()
    // The following is called by DoCancel() or RunL()
    // so is unnecessary here too
    // iThread.Close(); // Closes the handle on the thread
}
void CAsyncTask::DoAsyncTask(TRequestStatus& aStatus)
{
    if (IsActive())
    {
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrAlreadyExists);
        return;
    }
    // Save the caller’s TRequestStatus to notify them later
    iCaller = &aStatus;
    // Create a new thread, passing the thread function and stack sizes
    // No extra parameters are required in this example, so pass in NULL
    TInt res = iThread.Create(KThreadName, ThreadEntryPoint,
                              KDefaultStackSize, NULL, (TAny *)this);
    if (KErrNone != res)
    {
        // Complete the caller immediately
        User::RequestComplete(iCaller, res);
    }
    else
    {
        // Set active; resume new thread to make the synchronous call
        // (Change the priority of the thread here if required)
        // Set the caller and ourselves to KRequestPending
        // so the active scheduler notifies on completion
        *iCaller = KRequestPending;
        iStatus = KRequestPending;
        SetActive();
        iThread.Logon(iStatus); // Request notification when thread dies
        iThread.Resume();        // Start the thread
    }
}
TInt CAsyncTask::ThreadEntryPoint(TAny* aParameters/*aParameters*/)
{
    CAsyncTask *self = (CAsyncTask *)aParameters;
    // Perform a long synchronous task e.g. a lengthy calculation
    //TInt res = self->player->DoGenerate();
    // Task is complete so end this thread with returned error code
    RThread().Kill(res);
    return (KErrNone);      // This value is discarded
}
void CAsyncTask::DoCancel()
{
    // Kill the thread and complete with KErrCancel
    // ONLY if it is still running
    TExitType threadExitType = iThread.ExitType();
    if (EExitPending == threadExitType)
    {
        // Thread is still running
        TRequestStatus status;
        iThread.LogonCancel(status);
        iThread.Kill(KErrCancel);
        iThread.Close();
        // Complete the caller
        User::RequestComplete(iCaller, KErrCancel);
    }
}
void CAsyncTask::RunL()
{
    // Check in case thread is still running e.g. if Logon() failed
    TExitType threadExitType = iThread.ExitType();
    if (EExitPending == threadExitType) // Thread is still running, kill it
        iThread.Kill(KErrNone);
    // Complete the caller, passing iStatus value to RThread::Kill()
    User::RequestComplete(iCaller, iStatus.Int());
    iThread.Close(); // Close the thread handle, no need to LogonCancel()
}
TInt CAsyncTask::RunError(TInt anError)
{
    if (iCaller)
    {
        User::RequestComplete(iCaller, anError);
    }
    return (KErrNone);
}

CAsyncTask* CAsyncTask::NewLC(Cayfly_s60Audio *_player)
{
    CAsyncTask* self = new(ELeave) CAsyncTask;
    CleanupStack::PushL(self);
    self->ConstructL(_player);
    return self;
}

CAsyncTask* CAsyncTask::NewL(Cayfly_s60Audio *_player)
{
    CAsyncTask* self = CAsyncTask::NewLC(_player);
    CleanupStack::Pop(self);
}

void CAsyncTask::ConstructL(Cayfly_s60Audio *_player)
{
    player = _player;
}
