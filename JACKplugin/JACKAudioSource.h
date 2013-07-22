#pragma once

#include "OBSapi.h"

class JACKAudioSource : public AudioSource
{
private:
	UINT sampleSegmentSize, sampleFrameCount;
	List<BYTE> sampleBuffer;
	List<BYTE> outputBuffer;

	CRITICAL_SECTION sampleBufferLock;

public:
	JACKAudioSource(bool bFloat, UINT channels, UINT samplesPerSec, UINT bitsPerSample, UINT blockSize);
	~JACKAudioSource();

	void ReceiveAudio(LPBYTE lpData, UINT dataLength);
	void FlushSamples();

protected:
	bool GetNextBuffer(void **buffer, UINT *numFrames, QWORD *timestamp);
	void ReleaseBuffer();

	CTSTR GetDeviceName() const;
};