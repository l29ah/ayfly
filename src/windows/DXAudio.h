#ifndef DXAUDIO_H_
#define DXAUDIO_H_

#include "AbstractAudio.h"

class DXAudio : public AbstractAudio
{
public:
	DXAudio(unsigned long _sr);
	virtual ~DXAudio();
	virtual bool Start(void);
	virtual void Stop();
private:
	LPDIRECTSOUND8 lpds;
	LPDIRECTSOUNDBUFFER8 pDsb8;
	int buffer_size;
	HANDLE hNotifyEvent1;
	HANDLE hNotifyEvent2;
	HANDLE hSyncEvent;
	HANDLE hPlayingThread;
	bool dx_created;
	HRESULT CreateBasicBuffer();
	HRESULT SetNotificationPositions();
	static DWORD pascal PlayingThread(void *arg);
	void FillBuffer(DWORD dwOffset, DWORD dwSoundBytes);
	void ClearBuffer();

	DWORD DXProcess();

};

#endif /*DXAUDIO_H_*/

