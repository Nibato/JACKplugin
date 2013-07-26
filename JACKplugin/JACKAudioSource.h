#pragma once

#include "OBSapi.h"
#include "jack/ringbuffer.h"

class JACKAudioSource : public AudioSource
{
private:
	UINT sampleSegmentSize, sampleFrameCount;

	jack_ringbuffer_t* sampleBuffer;
	List<BYTE> outputBuffer;

	CRITICAL_SECTION sampleBufferLock;

public:
	JACKAudioSource(UINT inputSamplesPerSec);
	~JACKAudioSource();

	void ReceiveAudio(float *inLeft, float *inRight, UINT frames);
	void FlushSamples();

protected:
	bool GetNextBuffer(void **buffer, UINT *numFrames, QWORD *timestamp);
	void ReleaseBuffer();

	CTSTR GetDeviceName() const;
};