#include "macros.h"

#include <cgnslib.h>
#include <iriclib.h>

#include <stdio.h>
#include <iostream>

extern "C" {

void case_CheckLock()
{
	FILE* f = fopen("Case1.cgn.lock", "w");
	fclose(f);

	int locked = iRIC_Check_Lock(const_cast<char*>("Case1.cgn"));
	VERIFY_LOG("iRIC_Check_Lock() checked lock file, and it existed.", locked == IRIC_LOCKED);

	remove("Case1.cgn.lock");

	locked = iRIC_Check_Lock(const_cast<char*>("Case1.cgn"));
	VERIFY_LOG("iRIC_Check_Lock() checked lock file, and it did not exist.", locked != IRIC_LOCKED);
}

void case_CheckCancel()
{
	FILE* f = fopen(".cancel", "w");
	fclose(f);

	int canceled = iRIC_Check_Cancel();
	VERIFY_LOG("iRIC_Check_Cancel() checked .cancel, and it existed.", canceled == 1);

	remove(".cancel");

	canceled = iRIC_Check_Cancel();
	VERIFY_LOG("iRIC_Check_Cancel() checked .cancel, and it did not exist.", canceled == 0);
}

} // extern "C"
