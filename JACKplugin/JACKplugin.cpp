#include "JACKplugin.h"

JACKPlugin* JACKPlugin::instance = NULL;

JACKPlugin::JACKPlugin()
{
	instance = this;

	client = NULL;
	inputLeft = NULL;
	inputRight = NULL;
	audioSource = NULL;
	
	ClientConnect();
}

JACKPlugin::~JACKPlugin()
{
	instance = NULL;

	ClientDisconnect();
}

bool JACKPlugin::ClientConnect()
{
	if (client)
		return true;

	try
	{
		jack_status_t status;
		
		client = jack_client_open (PLUGIN_FULLNAME, JackNoStartServer, &status, NULL);
				
		if (!client)
			throw "Could not connect to jack server";

		jack_set_process_callback(client, ProcessCallback, this);
		jack_on_shutdown (client, ShutdownCallback, this);

		inputLeft = jack_port_register (client, "inputLeft", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
		inputRight = jack_port_register (client, "inputRight", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

		if (!inputLeft || !inputRight)
			throw "No more JACK ports available";

		if (jack_activate(client))
			throw "Cannot activate client";
		
		if (!audioSource)
		{
			bool  bFloat = true;
			UINT  inputChannels = 2;
			UINT  inputSamplesPerSec = jack_get_sample_rate(client);
			UINT  inputBitsPerSample = sizeof(float)*8;
			UINT  inputBlockSize = sizeof(float) * inputChannels;

			audioSource = new JACKAudioSource(bFloat, inputChannels, inputSamplesPerSec, inputBitsPerSample, inputBlockSize);		
			API->AddAudioSource(audioSource);
		}
	}
	catch (const char *error)
	{
		ClientDisconnect();
		MessageBoxA(NULL, error, "Error - JACKPlugin", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

void JACKPlugin::ClientDisconnect()
{
	if (client)
			jack_client_close(client);
	
	client = NULL;
	inputLeft = NULL;
	inputRight = NULL;

	if (audioSource)
	{
		API->RemoveAudioSource(audioSource);

		delete audioSource;
		audioSource = NULL;
	}
}

bool JACKPlugin::ClientIsConnected()
{
	return client;
}

int JACKPlugin::ProcessCallback(jack_nframes_t nframes, void *arg)
{
	if (arg != instance || !instance)
		return 0;

	JACKPlugin* plugin = static_cast<JACKPlugin*>(arg);

	if (!plugin->inputLeft || !plugin->inputRight)
		return 0;

	float* inLeft = (float*)jack_port_get_buffer(plugin->inputLeft, nframes);
	float* inRight = (float*)jack_port_get_buffer(plugin->inputRight, nframes);

	if (!plugin->audioSource)
		return 0;

	float* buffer = new float[nframes * 2];

	for (UINT j = 0, i = 0; i < nframes; i++)
	{
		buffer[j++] = inLeft[i];
		buffer[j++] = inRight[i];
	}

	plugin->audioSource->ReceiveAudio((LPBYTE)buffer, sizeof(float) * nframes * 2);
	delete buffer;

	return 0;
}

void JACKPlugin::ShutdownCallback(void *arg)
{
	if (arg != instance || instance == NULL)
		return;

	JACKPlugin* plugin = static_cast<JACKPlugin*>(arg);

	plugin->ClientDisconnect();
}

JACKPlugin* JACKPlugin::Initialize()
{
	if (instance != NULL)
		return instance;

	return new JACKPlugin();
}

void JACKPlugin::Shutdown()
{
	if (instance != NULL)
		delete instance;
}


bool LoadPlugin()
{
	return true;
}

void UnloadPlugin()
{
	
}

void OnStartStream()
{
	JACKPlugin::Initialize();
}

void OnStopStream()
{
	JACKPlugin::Shutdown();
}

CTSTR GetPluginName()
{
	return TEXT(PLUGIN_FULLNAME);
}

CTSTR GetPluginDescription()
{
	return TEXT("FIXME: Fill me in ;)");
}