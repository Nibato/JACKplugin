#pragma once


#include "OBSapi.h"
#include "jack/jack.h"

#include "JACKplugin.h"

class JACKAudioSource : public AudioSource
{
private:
	jack_port_t *input_port;
	jack_client_t *client;

    UINT sampleSegmentSize, sampleFrameCount;
    List<BYTE> sampleBuffer;
    List<BYTE> outputBuffer;

	CRITICAL_SECTION sampleBufferLock;


	static int JACKProcessCallback(jack_nframes_t nframes, void *arg);
	static void JACKShutdownCallback(void *arg);

public:
	JACKAudioSource();
	~JACKAudioSource();

	void ReceiveAudio(LPBYTE lpData, UINT dataLength);
	void FlushSamples();

protected:
    bool GetNextBuffer(void **buffer, UINT *numFrames, QWORD *timestamp);
    void ReleaseBuffer();

	CTSTR GetDeviceName() const;
};

/*
class DeviceAudioSource : public AudioSource
{
    DeviceSource *device;

    UINT sampleSegmentSize, sampleFrameCount;

    HANDLE hAudioMutex;
    List<BYTE> sampleBuffer;
    List<BYTE> outputBuffer;

    int offset;

protected:
    virtual bool GetNextBuffer(void **buffer, UINT *numFrames, QWORD *timestamp);
    virtual void ReleaseBuffer();

    virtual CTSTR GetDeviceName() const;

public:
    bool Initialize(DeviceSource *parent);
    ~DeviceAudioSource();

    void ReceiveAudio(LPBYTE lpData, UINT dataLength);

    void FlushSamples();

    inline void SetAudioOffset(int offset) {this->offset = offset; SetTimeOffset(offset);}
};

*/