#include "JACKAudioSource.h"

JACKAudioSource::JACKAudioSource(bool bFloat, UINT channels, UINT samplesPerSec, UINT bitsPerSample, UINT blockSize)
{
	InitializeCriticalSection(&sampleBufferLock);

	sampleFrameCount   = samplesPerSec/100;
	sampleSegmentSize  = blockSize*sampleFrameCount;
	outputBuffer.SetSize(sampleSegmentSize);

	InitAudioData(bFloat, channels, samplesPerSec, bitsPerSample, blockSize, 0);

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

