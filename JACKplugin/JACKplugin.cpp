#include "JACKplugin.h"

JACKAudioSource* instance = NULL;

bool LoadPlugin()
{
	
	if (instance != NULL)
		return false;

	instance = new JACKAudioSource();

	return true;
}

void UnloadPlugin()
{
	/*
	if (instance == NULL)
		return;

	delete instance;
	instance = NULL;
	*/
}

void OnStartStream()
{

}

void OnStopStream()
{

}

CTSTR GetPluginName()
{
    return TEXT(PLUGIN_FULLNAME);
}

CTSTR GetPluginDescription()
{
    return TEXT("FIXME: Fill me in ;)");
}