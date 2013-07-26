#include "JACKAudioSource.h"

JACKAudioSource::JACKAudioSource(UINT inputSamplesPerSec)
{
	InitializeCriticalSection(&sampleBufferLock);

	bool  bFloat = true;
	UINT  inputChannels = 2;
	UINT  inputBitsPerSample = sizeof(float)*8;
	UINT  inputBlockSize = sizeof(float) * inputChannels;

	sampleFrameCount   = inputSamplesPerSec/100;
	sampleSegmentSize  = inputBlockSize*sampleFrameCount;
	outputBuffer.SetSize(sampleSegmentSize);

	sampleBuffer = jack_ringbuffer_create(sampleSegmentSize * 5);

	assert(!jack_ringbuffer_mlock(sampleBuffer));

	InitAudioData(bFloat, inputChannels, inputSamplesPerSec, inputBitsPerSample, inputBlockSize, 0);

	API->AddAudioSource(this);
}

JACKAudioSource::~JACKAudioSource()
{
	API->RemoveAudioSource(this);
	jack_ringbuffer_free(sampleBuffer);
	DeleteCriticalSection(&sampleBufferLock);
}

bool JACKAudioSource::GetNextBuffer(void **buffer, UINT *numFrames, QWORD *timestamp)
{
	if(jack_ringbuffer_read_space(sampleBuffer) >= sampleSegmentSize)
	{
		EnterCriticalSection(&sampleBufferLock);
		jack_ringbuffer_read(sampleBuffer, (char *)outputBuffer.Array(), sampleSegmentSize);
		LeaveCriticalSection(&sampleBufferLock);

		*buffer = outputBuffer.Array();
		*numFrames = sampleFrameCount;
		*timestamp = API->GetAudioTime();

		return true;
	}

	return false;
}

void JACKAudioSource::ReleaseBuffer()
{

}

void JACKAudioSource::ReceiveAudio(float *inLeft, float *inRight, UINT frames)
{
	if (!inLeft || !inRight)
		return;
	
	EnterCriticalSection(&sampleBufferLock);
	for (UINT j = 0, i = 0; i < frames; i++)
	{
		if (jack_ringbuffer_write_space(sampleBuffer) < (sizeof(float)*2))
			break;

		jack_ringbuffer_write(sampleBuffer, (const char *)&inLeft[i], sizeof(float));
		jack_ringbuffer_write(sampleBuffer, (const char *)&inRight[i], sizeof(float));
	}

	LeaveCriticalSection(&sampleBufferLock);
}

CTSTR JACKAudioSource::GetDeviceName() const
{
	return NULL;
}

void JACKAudioSource::FlushSamples()
{
	EnterCriticalSection(&sampleBufferLock);
	jack_ringbuffer_reset(sampleBuffer);
	LeaveCriticalSection(&sampleBufferLock);
}

