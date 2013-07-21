#pragma once

#include "OBSapi.h"
#include "JACKAudioSource.h"

#define PLUGIN_FULLNAME "JACKplugin v1.0"

extern "C" __declspec(dllexport) bool LoadPlugin();
extern "C" __declspec(dllexport) void UnloadPlugin();
extern "C" __declspec(dllexport) void OnStartStream();
extern "C" __declspec(dllexport) void OnStopStream();
extern "C" __declspec(dllexport) CTSTR GetPluginName();
extern "C" __declspec(dllexport) CTSTR GetPluginDescription();
