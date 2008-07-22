#ifndef S60AUDIO_H_
#define S60AUDIO_H_

#include <mda/common/audio.h>
#include <mdaaudiooutputstream.h>

class Cayfly_s60Audio : public AbstractAudio, MMdaAudioOutputStreamCallback
{
public:
	virtual ~Cayfly_s60Audio();
	bool Start();
	void Stop();
	static Cayfly_s60Audio* NewL(unsigned long _sr);
public:
	void Open(); // opens the stream

public:
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

protected:
	// this method fills the buffer and writes it into the stream
	void UpdateBuffer();
	CMdaAudioOutputStream* iStream; // handle to the stream
	TMdaAudioDataSettings iSettings; // stream settings
	TUint8* iSoundData; // sound buffer
	TPtr8* iSoundBuf; // descriptor for using our soundbuffer
private:
	void ConstructL();
	Cayfly_s60Audio(unsigned long _sr);
	bool bOpened;
};

#endif /*S60AUDIO_H_*/
