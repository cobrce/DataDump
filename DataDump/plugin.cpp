#include "plugin.h"
using namespace Script::Module;


const DWORD SizeOfImageSectionHeader = 0x28;
const DWORD SizeOfImageFileHeader = 0x14;
const DWORD SizeOfPeSignature = 4;

enum
{
	MENU_TEST,
	MENU_DISASM_ADLER32,
	MENU_DUMP_ADLER32,
	MENU_STACK_ADLER32
};

PLUG_EXPORT void CBINITDEBUG(CBTYPE cbType, PLUG_CB_INITDEBUG* info)
{
	//dprintf("Debugging of %s started!\n", info->szFileName);
	_plugin_registercallback(pluginHandle, CB_PAUSEDEBUG, callback);
}

WORD ReadWord(duint VA)
{
	return (WORD)ReadDword(VA);
}

DWORD ReadDword(duint VA)
{
	DWORD read;
	DbgMemRead(VA, &read, 4);
	return read;
}

PLUG_EXPORT void callback(CBTYPE cbType, void* callbackInfo)
{
	if (cbType == CB_PAUSEDEBUG)
	{
		duint base = GetMainModuleBase();

		DWORD e_lfanew = ReadDword(base + 0x3c);
		duint image_nt_headers = base + e_lfanew;
		WORD nsections = ReadWord(image_nt_headers + 6);

		if (nsections < 2)
			dprintf("Need atleast 2 sections to show show the second one in dump");
		else
		{
			WORD SizeOfOtionalHeader = ReadWord(image_nt_headers + 0x14);
			DWORD RVA = GetDataSectionRVA(image_nt_headers + SizeOfOtionalHeader + SizeOfImageFileHeader + SizeOfPeSignature, nsections);
			duint VA = base + RVA;
			GuiDumpAt(VA);
			dprintf("GuiDumpAt(%lX)", RVA);
		}
		_plugin_unregistercallback(pluginHandle, CB_PAUSEDEBUG);
	}
}

bool isWritable(duint vaSectionHeader)
{
	const int CHARACTERISTICS = 0x24;
	return ((ReadDword(vaSectionHeader + CHARACTERISTICS) & IMAGE_SCN_MEM_WRITE) == IMAGE_SCN_MEM_WRITE);
}

DWORD GetDataSectionRVA(duint vaFirstSectionHeader, duint nSections)
{
	DWORD sectionRVA = ReadDword(vaFirstSectionHeader + 1 * SizeOfImageSectionHeader + 0xc);

	for (int i = 0; i < nSections; i++)
		if (isWritable(vaFirstSectionHeader + i * SizeOfImageSectionHeader))
		{
			sectionRVA = ReadDword(vaFirstSectionHeader + i * SizeOfImageSectionHeader + 0xc);
			break;
		}

	return sectionRVA;
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
	switch (info->hEntry)
	{
	case MENU_TEST:
		MessageBoxA(hwndDlg, "DataDump written by COB\r\n\r\nWhen mainmodule break point is reached, this plugin shows the second section in dump\r\nThanks to ThunderCls for the valuable plugins source", PLUGIN_NAME, MB_ICONINFORMATION);
		break;

	default:
		break;
	}
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
	return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
}

//Do GUI/Menu related things here.
void pluginSetup()
{
	_plugin_menuaddentry(hMenu, MENU_TEST, "&about");
}
