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

void writeSolution(int fid)
{
	cgsize_t isize, jsize;

	int ier = cg_iRIC_GotoGridCoord2d_Mul(fid, &isize, &jsize);
	VERIFY_LOG("cg_iRIC_GotoGridCoord2d_Mul() ier == 0", ier == 0);

	std::vector<double> vx, vy, depth;

	vx.assign(isize * jsize, 1);
	vy.assign(isize * jsize, 0.3);
	depth.assign(isize * jsize, 4);

	for (int i = 0; i < 5; ++i) {
		double TimeVal = i * 2.13;
		ier = cg_iRIC_Write_Sol_Time_Mul(fid, TimeVal);
		VERIFY_LOG("cg_iRIC_Write_Sol_Time_Mul() ier == 0", ier == 0);

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("Depth"), depth.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for Depth ier == 0", ier == 0);

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("VelocityX"), vx.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for VelocityX ier == 0", ier == 0);

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("VelocityY"), vy.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for VelocityY ier == 0", ier == 0);

		double Dist = i * - 0.2 + 20;
		cg_iRIC_Write_Sol_BaseIterative_Real_Mul(fid, const_cast<char*>("Discharge"), Dist);
	}

	return;
}

void case_SolWriteStd()
{
	iRIC_InitOption(IRIC_OPTION_STDSOLUTION);

	QFile::copy("case_init.cgn", "case_solstd.cgn");

	int fid;
	int ier = cg_open("case_solstd.cgn", CG_MODE_MODIFY, &fid);
	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);
	cg_iRIC_SetFilename(fid, "case_solstd.cgn");

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	writeSolution(fid);

	cg_close(fid);

//	remove("case_solstd.cgn");

	// @todo add codes to test
}

void case_SolWriteDivide()
{
	iRIC_InitOption(IRIC_OPTION_DIVIDESOLUTIONS);

	QFile::copy("case_init.cgn", "case_soldivide.cgn");

	int fid;
	int ier = cg_open("case_soldivide.cgn", CG_MODE_MODIFY, &fid);
	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);
	cg_iRIC_SetFilename(fid, "case_soldivide.cgn");

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	writeSolution(fid);

	cg_close(fid);

//	remove("case_soldivide.cgn");

	// @todo add codes to test
}

} // extern "C"
