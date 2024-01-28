#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "Core/ntshengn_core.h"

int main() {
	NtshEngn::Core core;
	core.launch("assets/options/options.ntop");

	return 0;
}