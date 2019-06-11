/*
	Hello World example made by Aurelio Mannara for ctrulib
	This code was modified for the last time on: 12/12/2014 21:00 UTC+1
*/

#include "config.h"

#include <3ds.h>
#include <stdio.h>

#include "doomtype.h"
#include "i_system.h"
#include "m_argv.h"

u32 __stacksize__ = 512 * 1024;

int waithere(char *str) {
#if 0
	printf("%s ...", str);
	do {
		hidScanInput();
	} while ((hidKeysHeld() & KEY_A) == 0);
	do {
		hidScanInput();
	} while ((hidKeysHeld() & KEY_A) != 0);
	printf("done.\n");
#endif
	return 0;
}

static void shutdown_3ds() {
	printf("shutting down graphics\n");
	osSetSpeedupEnable(false);
	gfxExit();
}

int main(int argc, char **argv)
{
	gfxInitDefault();
	osSetSpeedupEnable(true);

	consoleInit(GFX_BOTTOM, NULL);

	I_AtExit(shutdown_3ds, true);

	waithere("M_FindResponseFile");
	M_FindResponseFile();

	waithere("D_DoomMain");
	D_DoomMain();

	return 0;
}
