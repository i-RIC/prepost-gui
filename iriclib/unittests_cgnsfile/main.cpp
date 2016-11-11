#include "cases.h"

#include <iostream>

int main(int argc, char* argv[])
{
	case_InitSuccess();
	case_InitFail();

	case_InitReadSuccess();
	case_InitReadFail();

	case_InitOptionCheck();

	case_CheckLock();
	case_CheckCancel();

	case_CalcCondRead();
	case_CalcCondWrite();

	case_BcRead();
	case_BcWrite();

	return 0;
}
