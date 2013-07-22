#pragma once

#include "OBSapi.h"
#include "jack/jack.h"
#include "JACKAudioSource.h"

#define PLUGIN_FULLNAME "JACKplugin v1.0"

class JACKPlugin
{
private:
	static JACKPlugin* instance;

	jack_port_t* inputLeft;
	jack_port_t* inputRight;
	jack_client_t* client;

	JACKAudioSource* audioSource;

	JACKPlugin();

	static int ProcessCallback(jack_nframes_t nframes, void *arg);
	static void ShutdownCallback(void *arg);

	bool JACKConnect();
	void JACKDisconnect();

public:
	~JACKPlugin();

	static JACKPlugin* Initialize();
	static void Shutdown();
};


extern "C" __declspec(dllexport) bool LoadPlugin();
extern "C" __declspec(dllexport) void UnloadPlugin();
extern "C" __declspec(dllexport) void OnStartStream();
extern "C" __declspec(dllexport) void OnStopStream();
extern "C" __declspec(dllexport) CTSTR GetPluginName();
extern "C" __declspec(dllexport) CTSTR GetPluginDescription();
