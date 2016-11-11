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

void writeSolution(int fid, bool iterMode)
{
	cgsize_t isize, jsize;

	int ier = cg_iRIC_GotoGridCoord2d_Mul(fid, &isize, &jsize);
	VERIFY_LOG("cg_iRIC_GotoGridCoord2d_Mul() ier == 0", ier == 0);

	std::vector<double> x, y;
	std::vector<double> vx, vy, depth;
	std::vector<double> wet;

	x.assign(isize * jsize, 0);
	y.assign(isize * jsize, 0);
	ier = cg_iRIC_GetGridCoord2d_Mul(fid, x.data(), y.data());

	vx.assign(isize * jsize, 1);
	vy.assign(isize * jsize, 0.3);
	depth.assign(isize * jsize, 4);
	wet.assign(isize * jsize, 0);

	for (int i = 0; i < 5; ++i) {
		if (iterMode) {
			double IterVal = i;
			ier = cg_iRIC_Write_Sol_Iteration_Mul(fid, IterVal);
			VERIFY_LOG("cg_iRIC_Write_Sol_Iteration_Mul() ier == 0", ier == 0);
		} else {
			double TimeVal = i * 2.13;
			ier = cg_iRIC_Write_Sol_Time_Mul(fid, TimeVal);
			VERIFY_LOG("cg_iRIC_Write_Sol_Time_Mul() ier == 0", ier == 0);
		}

		cg_iRIC_Write_Sol_GridCoord2d_Mul(fid, x.data(), y.data());

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("Depth"), depth.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for Depth ier == 0", ier == 0);

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("VelocityX"), vx.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for VelocityX ier == 0", ier == 0);

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("VelocityY"), vy.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for VelocityY ier == 0", ier == 0);

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("IBC"), wet.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for IBC ier == 0", ier == 0);

		double Dist = i * - 0.2 + 20;
		cg_iRIC_Write_Sol_BaseIterative_Real_Mul(fid, const_cast<char*>("Discharge"), Dist);
		VERIFY_LOG("cg_iRIC_Write_Sol_BaseIterative_Real_Mul() for IBC ier == 0", ier == 0);
		int DamOpen = i;
		cg_iRIC_Write_Sol_BaseIterative_Integer_Mul(fid, const_cast<char*>("DamOpen"), DamOpen);
		VERIFY_LOG("cg_iRIC_Write_Sol_BaseIterative_Integer_Mul() for IBC ier == 0", ier == 0);
	}
}

void writeSolution3d(int fid)
{
	cgsize_t isize, jsize;

	int ier = cg_iRIC_GotoGridCoord2d_Mul(fid, &isize, &jsize);
	VERIFY_LOG("cg_iRIC_GotoGridCoord2d_Mul() ier == 0", ier == 0);

	std::vector<double> x, y, z;
	std::vector<double> vx, vy, depth;
	std::vector<double> wet;

	x.assign(isize * jsize, 0);
	y.assign(isize * jsize, 0);
	z.assign(isize * jsize, 0);
	ier = cg_iRIC_GetGridCoord2d_Mul(fid, x.data(), y.data());

	cg_iRIC_WriteGridCoord3d_Mul(fid, isize, jsize, 1, x.data(), y.data(), z.data());

	vx.assign(isize * jsize, 1);
	vy.assign(isize * jsize, 0.3);
	depth.assign(isize * jsize, 4);
	wet.assign(isize * jsize, 0);

	for (int i = 0; i < 5; ++i) {
		double TimeVal = i * 2.13;
		ier = cg_iRIC_Write_Sol_Time_Mul(fid, TimeVal);
		VERIFY_LOG("cg_iRIC_Write_Sol_Time_Mul() ier == 0", ier == 0);

		cg_iRIC_Write_Sol_GridCoord3d_Mul(fid, x.data(), y.data(), z.data());

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("Depth"), depth.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for Depth ier == 0", ier == 0);

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("VelocityX"), vx.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for VelocityX ier == 0", ier == 0);

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("VelocityY"), vy.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for VelocityY ier == 0", ier == 0);

		cg_iRIC_Write_Sol_Real_Mul(fid, const_cast<char*>("IBC"), wet.data());
		VERIFY_LOG("cg_iRIC_Write_Sol_Real_Mul() for IBC ier == 0", ier == 0);

		double Dist = i * - 0.2 + 20;
		cg_iRIC_Write_Sol_BaseIterative_Real_Mul(fid, const_cast<char*>("Discharge"), Dist);
		VERIFY_LOG("cg_iRIC_Write_Sol_BaseIterative_Real_Mul() for IBC ier == 0", ier == 0);
		int DamOpen = i;
		cg_iRIC_Write_Sol_BaseIterative_Integer_Mul(fid, const_cast<char*>("DamOpen"), DamOpen);
		VERIFY_LOG("cg_iRIC_Write_Sol_BaseIterative_Integer_Mul() for IBC ier == 0", ier == 0);
	}
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
	cg_iRIC_SetFilename(fid, const_cast<char*>("case_solstd.cgn"));

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	writeSolution(fid, false);

	cg_close(fid);

//	remove("case_solstd.cgn");

	// @todo add codes to test

	QFile::copy("case_init.cgn", "case_solstd3d.cgn");

	ier = cg_open("case_solstd3d.cgn", CG_MODE_MODIFY, &fid);
	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);
	cg_iRIC_SetFilename(fid, const_cast<char*>("case_solstd3d.cgn"));

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	writeSolution3d(fid);

	cg_close(fid);

//	remove("case_solstd3d.cgn");

	// @todo add codes to test

	QFile::copy("case_init.cgn", "case_solstditer.cgn");

	ier = cg_open("case_solstditer.cgn", CG_MODE_MODIFY, &fid);
	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);
	cg_iRIC_SetFilename(fid, const_cast<char*>("case_solstditer.cgn"));

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	writeSolution(fid, true);

	cg_close(fid);

	remove("case_solstditer.cgn");

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
	cg_iRIC_SetFilename(fid, const_cast<char*>("case_soldivide.cgn"));

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	writeSolution(fid, false);

	cg_close(fid);

//	remove("case_soldivide.cgn");
//	remove("case_soldivide_Solution1.cgn");
//	remove("case_soldivide_Solution2.cgn");
//	remove("case_soldivide_Solution3.cgn");
//	remove("case_soldivide_Solution4.cgn");
//	remove("case_soldivide_Solution5.cgn");

	// @todo add codes to test

	QFile::copy("case_init.cgn", "case_soldivide3d.cgn");

	ier = cg_open("case_soldivide3d.cgn", CG_MODE_MODIFY, &fid);
	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);
	cg_iRIC_SetFilename(fid, const_cast<char*>("case_soldivide3d.cgn"));

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	writeSolution3d(fid);

	cg_close(fid);

	remove("case_soldivide3d.cgn");

	// @todo add codes to test

}

} // extern "C"
