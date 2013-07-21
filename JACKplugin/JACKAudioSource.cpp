#include "JACKAudioSource.h"

//#define DMSG(A) MessageBoxA(NULL, A, "debug", MB_OK);

void DMSG(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

	char buf[512];
	vsprintf(buf, fmt, args);

	MessageBoxA(NULL, buf, "debug", MB_OK);
}

JACKAudioSource::JACKAudioSource()
{
	
    InitializeCriticalSection(&sampleBufferLock);

	try
	{
		jack_status_t status;
		client = jack_client_open (PLUGIN_FULLNAME, JackNullOption, &status, NULL);

		if (client == NULL)
			throw "Could not connect to jack server";
		
		jack_set_process_callback(client, JACKProcessCallback, this);
		jack_on_shutdown (client, JACKShutdownCallback, this);


	    bool  bFloat = true;
	    UINT  inputChannels = 1;
	    UINT  inputSamplesPerSec = jack_get_sample_rate(client);
	    UINT  inputBitsPerSample = sizeof(float)*8;
	    UINT  inputBlockSize = sizeof(float);
	    DWORD inputChannelMask = 0;
		
		DMSG("Samples Per Secs %u\n Bits Per Sample %u\nBlockSize %u\n", inputSamplesPerSec, inputBitsPerSample, inputBlockSize);


	    sampleFrameCount   = inputSamplesPerSec/100;
	    sampleSegmentSize  = inputBlockSize*sampleFrameCount;
		outputBuffer.SetSize(sampleSegmentSize);

		InitAudioData(bFloat, inputChannels, inputSamplesPerSec, inputBitsPerSample, inputBlockSize, inputChannelMask);

		input_port = jack_port_register (client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
		if (input_port == NULL)
			throw "No more JACK ports available";

		if (jack_activate(client))
			throw "Cannot activate client";

		/*
		const char **ports = jack_get_ports(client, NULL, NULL, JackPortIsOutput);

	    if (ports == NULL)
			throw "No JACK capture ports";

		if (jack_connect (client, ports[0], jack_port_name (input_port)))
			throw "Cannot connect input ports";
		//free(ports);
		*/

		API->AddAudioSource(this);
		
	}
	catch (CTSTR error)
	{
		MessageBox(NULL, error, TEXT("Error - JACKPlugin"), MB_OK | MB_ICONEXCLAMATION);
	}
}


JACKAudioSource::~JACKAudioSource()
{
	API->RemoveAudioSource(this);

	if (client != NULL)
		jack_client_close(client);

	DeleteCriticalSection(&sampleBufferLock);
}

int JACKAudioSource::JACKProcessCallback(jack_nframes_t nframes, void *arg)
{
	if (arg == NULL)
		return 0;

	JACKAudioSource* audioSource = static_cast<JACKAudioSource*>(arg);

	jack_default_audio_sample_t* inSamples = (jack_default_audio_sample_t*) jack_port_get_buffer(audioSource->input_port, nframes);

    audioSource->ReceiveAudio((LPBYTE)inSamples, sizeof(jack_default_audio_sample_t) * nframes);	

	return 0;
}

void JACKAudioSource::JACKShutdownCallback(void *arg)
{
	if (arg == NULL)
		return;

	delete static_cast<JACKAudioSource*>(arg);

	return;
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

	// FIXME: stub
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

