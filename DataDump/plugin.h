#pragma once

#include "pluginmain.h"

//plugin data
#define PLUGIN_NAME "DataDump"
#define PLUGIN_VERSION 1

//functions
bool pluginInit(PLUG_INITSTRUCT* initStruct);
void pluginStop();
void pluginSetup();
PLUG_EXPORT void callback(CBTYPE cbType, void* callbackInfo);
DWORD GetDataSectionRVA(duint vaFirstSectionHeader, duint nSections);
WORD ReadWord(duint VA);
DWORD ReadDword(duint VA);
bool isWritable(duint vaSectionHeader);
