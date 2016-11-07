#include "macros.h"

#include <stdio.h>

#include <cgnslib.h>
#include <iriclib.h>

#include <iostream>

extern "C" {

void case_InitOptionCheck()
{
	remove(".cancel_ok");

	iRIC_InitOption(IRIC_OPTION_CANCEL);

	FILE* f = fopen(".cancel_ok", "r");

	VERIFY_LOG("iRIC_InitOption(IRIC_OPTION_CANCEL) created .cancel_ok ", f != NULL);

	if (f != NULL) {
		fclose(f);
	}
	remove(".cancel_ok");
}

} // extern "C"
