/**
*
*                      PulseAim
*			 github.com/ytmcgamer/PulseAim
*                 made by YTMcGamer#1337
*
*/

#include "Header/util.h"
#include "Header/settings.h"
#include "Header/offsets.h"
#include "Header/core.h"
#include "Header/menu.h"
VOID Main() {
	// system("https://discord.gg/yyKQCjs");
	 //system("https://www.youtube.com/channel/UCGJuiskeVMeV-9yzMzxQgZw");
	SettingsHelper::Initialize();
	if (!Util::Initialize()) {
		return;
	}

	if (!Offsets::Initialize()) {
		return;
	}

	if (!Core::Initialize()) {
		return;
	}

	if (!Render::Initialize()) {
		return;
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		Main();
	}

	return TRUE;
}