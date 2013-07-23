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

	InitAudioData(bFloat, inputChannels, inputSamplesPerSec, inputBitsPerSample, inputBlockSize, 0);

	API->AddAudioSource(this);
}

JACKAudioSource::~JACKAudioSource()
{
	API->RemoveAudioSource(this);
	DeleteCriticalSection(&sampleBufferLock);
}

bool JACKAudioSource::GetNextBuffer(void **buffer, UINT *numFrames, QWORD *timestamp)
{
	if(sampleBuffer.Num() >= sampleSegmentSize)
	{
		EnterCriticalSection(&sampleBufferLock);
		mcpy(outputBuffer.Array(), sampleBuffer.Array(), sampleSegmentSize);
		sampleBuffer.RemoveRange(0, sampleSegmentSize);
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

	List<float> buffer;
	buffer.SetSize(frames * 2);
	
	for (UINT j = 0, i = 0; i < frames; i++)
	{
		buffer[j++] = inLeft[i];
		buffer[j++] = inRight[i];
	}

	EnterCriticalSection(&sampleBufferLock);
	sampleBuffer.AppendArray((LPBYTE)buffer.Array(), buffer.Num() * sizeof(float));
	LeaveCriticalSection(&sampleBufferLock);
}

CTSTR JACKAudioSource::GetDeviceName() const
{
	return NULL;
}

void JACKAudioSource::FlushSamples()
{
	EnterCriticalSection(&sampleBufferLock);
	sampleBuffer.Clear();
	LeaveCriticalSection(&sampleBufferLock);
}

