#include "macros.h"

#include <QFile>

#include <cgnslib.h>
#include <iriclib.h>

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

extern "C" {

void case_SolStartEnd()
{
	int ier = iRIC_Write_Sol_Start(const_cast<char*>("Case1.cgn"));
	VERIFY_LOG("iRIC_Write_Sol_Start() ier == 0", ier == 0);

	FILE* f = fopen("Case1.cgn.lock", "r");
	VERIFY_LOG("iRIC_Write_Sol_Start() created Case1.cgn.lock", f != NULL);

	if (f != NULL) {
		fclose(f);
	}

	ier = iRIC_Write_Sol_End(const_cast<char*>("Case1.cgn"));
	VERIFY_LOG("iRIC_Write_Sol_End() ier == 0", ier == 0);

	f = fopen("Case1.cgn.lock", "r");
	VERIFY_LOG("iRIC_Write_Sol_End() removed Case1.cgn.lock", f == NULL);
}

} // extern "C"
