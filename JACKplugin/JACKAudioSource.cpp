#include "JACKAudioSource.h"

JACKAudioSource::JACKAudioSource(bool bFloat, UINT channels, UINT samplesPerSec, UINT bitsPerSample, UINT blockSize)
{
	InitializeCriticalSection(&sampleBufferLock);

	sampleFrameCount   = samplesPerSec/100;
	sampleSegmentSize  = blockSize*sampleFrameCount;
	outputBuffer.SetSize(sampleSegmentSize);

	InitAudioData(bFloat, channels, samplesPerSec, bitsPerSample, blockSize, 0);
}

JACKAudioSource::~JACKAudioSource()
{
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

void JACKAudioSource::ReceiveAudio(LPBYTE lpData, UINT dataLength)
{
	if(lpData)
	{
		EnterCriticalSection(&sampleBufferLock);
		sampleBuffer.AppendArray(lpData, dataLength);
		LeaveCriticalSection(&sampleBufferLock);
	}
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

