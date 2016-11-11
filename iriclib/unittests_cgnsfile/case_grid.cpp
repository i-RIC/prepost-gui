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

void case_GridRead()
{
	QFile::copy("case_init.cgn", "case_grid.cgn");

	int fid;
	int ier = cg_open("case_grid.cgn", CG_MODE_MODIFY, &fid);

	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	cgsize_t isize, jsize;
	std::vector<double> x, y;

	ier = cg_iRIC_GotoGridCoord2d_Mul(fid, &isize, &jsize);
	VERIFY_LOG("cg_iRIC_GotoGridCoord2d_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_GotoGridCoord2d_Mul() isize == 11", isize == 11);
	VERIFY_LOG("cg_iRIC_GotoGridCoord2d_Mul() jsize == 11", jsize == 11);

	x.assign(isize * jsize, 0);
	y.assign(isize * jsize, 0);

	ier = cg_iRIC_GetGridCoord2d_Mul(fid, x.data(), y.data());
	VERIFY_LOG("cg_iRIC_GotoGridCoord2d_Mul() ier == 0", ier == 0);

	std::vector<double> doubleArr;
	std::vector<int> intArr;

	doubleArr.assign(isize * jsize, 0);
	intArr.assign(isize * jsize, 0);

	ier = cg_iRIC_Read_Grid_Real_Node_Mul(fid, const_cast<char*>("node_real"), doubleArr.data());
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[0] == 2", doubleArr[0] == 2);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[1] == 2", doubleArr[1] == 2);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[2] == 2", doubleArr[2] == 2);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[3] == 2", doubleArr[3] == 2);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[4] == 2", doubleArr[4] == 2);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[5] == 2", doubleArr[5] == 2);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[6] == 2", doubleArr[6] == 2);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[7] == 0", doubleArr[7] == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[8] == 0", doubleArr[8] == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Node_Mul() value[9] == 0", doubleArr[9] == 0);

	ier = cg_iRIC_Read_Grid_Integer_Node_Mul(fid, const_cast<char*>("node_int"), intArr.data());
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[0] == 0", intArr[0] == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[1] == 0", intArr[1] == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[2] == 0", intArr[2] == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[3] == 0", intArr[3] == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[4] == 0", intArr[4] == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[5] == 1", intArr[5] == 1);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[6] == 1", intArr[6] == 1);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[7] == 1", intArr[7] == 1);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[8] == 1", intArr[8] == 1);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Node_Mul() value[9] == 1", intArr[9] == 1);

	doubleArr.assign((isize - 1) * (jsize - 1), 0);
	intArr.assign((isize - 1) * (jsize - 1), 0);

	ier = cg_iRIC_Read_Grid_Real_Cell_Mul(fid, const_cast<char*>("cell_real"), doubleArr.data());
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Cell_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Cell_Mul() value[0] == 4.2", doubleArr[0] == 4.2);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Cell_Mul() value[1] == 4.2", doubleArr[1] == 4.2);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Cell_Mul() value[2] == 0", doubleArr[2] == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Real_Cell_Mul() value[3] == 0", doubleArr[3] == 0);

	ier = cg_iRIC_Read_Grid_Integer_Cell_Mul(fid, const_cast<char*>("cell_int"), intArr.data());
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Cell_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Cell_Mul() value[0] == 0", intArr[0] == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Cell_Mul() value[15] == 1", intArr[15] == 1);
	VERIFY_LOG("cg_iRIC_Read_Grid_Integer_Cell_Mul() value[16] == 0", intArr[16] == 0);

	cg_close(fid);

	remove("case_grid.cgn");
}

void case_GridWrite()
{
	QFile::copy("case_nogrid.cgn", "case_gridwrite1d.cgn");

	int fid;
	int ier = cg_open("case_gridwrite1d.cgn", CG_MODE_MODIFY, &fid);

	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);

	VERIFY_LOG("cg_iRIC_Init() ier == 1", ier == 1);

	cgsize_t isize, jsize, ksize;
	std::vector<double> x, y, z;

	isize = 10;
	x.assign(isize, 0);

	for (int i = 0; i < isize; ++i) {
		x[i] = i * 2;
	}
	ier = cg_iRIC_WriteGridCoord1d_Mul(fid, isize, x.data());
	VERIFY_LOG("cg_iRIC_WriteGridCoord1d_Mul() ier == 0", ier == 0);

	cg_close(fid);

	ier = cg_open("case_gridwrite1d.cgn", CG_MODE_READ, &fid);
	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_goto(fid, 2, "iRICZone", 0, "GridCoordinates", 0, NULL);
	VERIFY_LOG("cg_goto() ier == 0", ier == 0);
	int dim;
	char name[32];
	DataType_t datatype;
	cgsize_t dimVec[3];
	ier = cg_array_info(1, name, &datatype, &dim, dimVec);
	VERIFY_LOG("cg_array_info() ier == 0", ier == 0);
	VERIFY_LOG("cg_array_info() name == CoordinateX", std::string("CoordinateX") == name);
	VERIFY_LOG("cg_array_info() datatype == RealDouble", datatype == RealDouble);
	VERIFY_LOG("cg_array_info() dim == 1", dim == 1);
	VERIFY_LOG("cg_array_info() dimVec[0] == 10", dimVec[0] == 10);

	std::vector<double> read_x, read_y, read_z;
	read_x.assign(dimVec[0], 0);
	ier = cg_array_read(1, read_x.data());
	VERIFY_LOG("cg_array_read() ier == 0", ier == 0);
	VERIFY_LOG("cg_array_read() value valid", x == read_x);

	cg_close(fid);

	remove("case_gridwrite1d.cgn");

	QFile::copy("case_nogrid.cgn", "case_gridwrite2d.cgn");

	ier = cg_open("case_gridwrite2d.cgn", CG_MODE_MODIFY, &fid);

	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);

	VERIFY_LOG("cg_iRIC_Init() ier == 1", ier == 1);

	isize = 10; jsize = 10;
	x.assign(isize * jsize, 0);
	y.assign(isize * jsize, 0);

	for (int j = 0; j < jsize; ++j) {
		for (int i = 0; i < isize; ++i) {
			x[i + isize * j] = i;
			y[i + isize * j] = j;
		}
	}
	ier = cg_iRIC_WriteGridCoord2d_Mul(fid, isize, jsize, x.data(), y.data());
	VERIFY_LOG("cg_iRIC_WriteGridCoord2d_Mul() ier == 0", ier == 0);

	std::vector<double> real_node_write, real_cell_write;
	std::vector<int> int_node_write, int_cell_write;

	real_node_write.assign(isize * jsize, 0);
	int_node_write.assign(isize * jsize, 0);
	real_cell_write.assign((isize - 1) * (jsize - 1), 0);
	int_cell_write.assign((isize - 1) * (jsize - 1), 0);

	for (int i = 0; i < real_node_write.size(); ++i) {
		real_node_write[i] = 0.5 * i - 0.001 * i * (i - 50);
	}

	for (int i = 0; i < int_node_write.size(); ++i) {
		int_node_write[i] = i * i + i * 2 + 6;
	}

	for (int i = 0; i < real_cell_write.size(); ++i) {
		real_cell_write[i] = 0.8 * i - 0.0005 * i * (i - 10);
	}

	for (int i = 0; i < int_cell_write.size(); ++i) {
		int_cell_write[i] = i * i - i * 4 + 414;
	}

	ier = cg_iRIC_Write_Grid_Real_Node_Mul(fid, const_cast<char*>("realnode_test"), real_node_write.data());
	VERIFY_LOG("cg_iRIC_Write_Grid_Real_Node_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_Grid_Integer_Node_Mul(fid, const_cast<char*>("intnode_test"), int_node_write.data());
	VERIFY_LOG("cg_iRIC_Write_Grid_Integer_Node_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_Grid_Real_Cell_Mul(fid, const_cast<char*>("realcell_test"), real_cell_write.data());
	VERIFY_LOG("cg_iRIC_Write_Grid_Real_Cell_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_Grid_Integer_Cell_Mul(fid, const_cast<char*>("intcell_test"), int_cell_write.data());
	VERIFY_LOG("cg_iRIC_Write_Grid_Integer_Cell_Mul() ier == 0", ier == 0);

	cg_close(fid);

	ier = cg_open("case_gridwrite2d.cgn", CG_MODE_READ, &fid);
	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_goto(fid, 1, "iRICZone", 0, "GridCoordinates", 0, NULL);
	VERIFY_LOG("cg_goto() ier == 0", ier == 0);
	ier = cg_array_info(1, name, &datatype, &dim, dimVec);
	VERIFY_LOG("cg_array_info() ier == 0", ier == 0);
	VERIFY_LOG("cg_array_info() name == CoordinateX", std::string("CoordinateX") == name);
	VERIFY_LOG("cg_array_info() datatype == RealDouble", datatype == RealDouble);

	read_x.assign(dimVec[0] * dimVec[1], 0);
	read_y.assign(dimVec[0] * dimVec[1], 0);

	ier = cg_array_read(1, read_x.data());
	VERIFY_LOG("cg_array_read() ier == 0", ier == 0);
	VERIFY_LOG("cg_array_read() value valid", x == read_x);

	ier = cg_array_read(2, read_y.data());
	VERIFY_LOG("cg_array_read() ier == 0", ier == 0);
	VERIFY_LOG("cg_array_read() value valid", y == read_y);

	ier = cg_goto(fid, 1, "iRICZone", 0, "GridConditions", 0, "realnode_test", 0, NULL);
	VERIFY_LOG("cg_goto() for realnode_test ier == 0", ier == 0);
	ier = cg_array_info(1, name, &datatype, &dim, dimVec);
	std::vector<double> real_node_read;
	real_node_read.assign(dimVec[0], 0);
	ier = cg_array_read(1, real_node_read.data());
	VERIFY_LOG("cg_array_read() for realnode_test ier == 0", ier == 0);
	VERIFY_LOG("realnode_test value check", real_node_read == real_node_write);

	ier = cg_goto(fid, 1, "iRICZone", 0, "GridConditions", 0, "intnode_test", 0, NULL);
	VERIFY_LOG("cg_goto() for intnode_test ier == 0", ier == 0);
	ier = cg_array_info(1, name, &datatype, &dim, dimVec);
	std::vector<int> int_node_read;
	int_node_read.assign(dimVec[0], 0);
	ier = cg_array_read(1, int_node_read.data());
	VERIFY_LOG("cg_array_read() for intnode_test ier == 0", ier == 0);
	VERIFY_LOG("intnode_test value check", int_node_read == int_node_write);

	ier = cg_goto(fid, 1, "iRICZone", 0, "GridConditions", 0, "realcell_test", 0, NULL);
	VERIFY_LOG("cg_goto() for realcell_test ier == 0", ier == 0);
	ier = cg_array_info(1, name, &datatype, &dim, dimVec);
	std::vector<double> real_cell_read;
	real_cell_read.assign(dimVec[0], 0);
	ier = cg_array_read(1, real_cell_read.data());
	VERIFY_LOG("cg_array_read() for realcell_test ier == 0", ier == 0);
	VERIFY_LOG("realcell_test value check", real_cell_read == real_cell_write);

	ier = cg_goto(fid, 1, "iRICZone", 0, "GridConditions", 0, "intcell_test", 0, NULL);
	VERIFY_LOG("cg_goto() for intcell_test ier == 0", ier == 0);
	ier = cg_array_info(1, name, &datatype, &dim, dimVec);
	std::vector<int> int_cell_read;
	int_cell_read.assign(dimVec[0], 0);
	ier = cg_array_read(1, int_cell_read.data());
	VERIFY_LOG("cg_array_read() for intcell_test ier == 0", ier == 0);
	VERIFY_LOG("intcell_test value check", int_cell_read == int_cell_write);

	cg_close(fid);

	remove("case_gridwrite2d.cgn");

	QFile::copy("case_nogrid.cgn", "case_gridwrite3d.cgn");


	ier = cg_open("case_gridwrite3d.cgn", CG_MODE_MODIFY, &fid);

	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);

	VERIFY_LOG("cg_iRIC_Init() ier == 1", ier == 1);

	isize = 10; jsize = 10; ksize = 5;
	x.assign(isize * jsize * ksize, 0);
	y.assign(isize * jsize * ksize, 0);
	z.assign(isize * jsize * ksize, 0);

	for (int k = 0; k < ksize; ++k) {
		for (int j = 0; j < jsize; ++j) {
			for (int i = 0; i < isize; ++i) {
				x[i + isize * j + isize * jsize * k] = i;
				y[i + isize * j + isize * jsize * k] = j;
				z[i + isize * j + isize * jsize * k] = k;
			}
		}
	}
	ier = cg_iRIC_WriteGridCoord3d_Mul(fid, isize, jsize, ksize, x.data(), y.data(), z.data());
	VERIFY_LOG("cg_iRIC_WriteGridCoord3d_Mul() ier == 0", ier == 0);

	cg_close(fid);

	ier = cg_open("case_gridwrite3d.cgn", CG_MODE_READ, &fid);
	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_goto(fid, 2, "iRICZone", 0, "GridCoordinates", 0, NULL);
	VERIFY_LOG("cg_goto() ier == 0", ier == 0);
	ier = cg_array_info(1, name, &datatype, &dim, dimVec);
	VERIFY_LOG("cg_array_info() ier == 0", ier == 0);
	VERIFY_LOG("cg_array_info() name == CoordinateX", std::string("CoordinateX") == name);
	VERIFY_LOG("cg_array_info() datatype == RealDouble", datatype == RealDouble);

	read_x.assign(dimVec[0] * dimVec[1] * dimVec[2], 0);
	read_y.assign(dimVec[0] * dimVec[1] * dimVec[2], 0);
	read_z.assign(dimVec[0] * dimVec[1] * dimVec[2], 0);

	ier = cg_array_read(1, read_x.data());
	VERIFY_LOG("cg_array_read() ier == 0", ier == 0);
	VERIFY_LOG("cg_array_read() value valid", x == read_x);

	ier = cg_array_read(2, read_y.data());
	VERIFY_LOG("cg_array_read() ier == 0", ier == 0);
	VERIFY_LOG("cg_array_read() value valid", y == read_y);

	ier = cg_array_read(3, read_z.data());
	VERIFY_LOG("cg_array_read() ier == 0", ier == 0);
	VERIFY_LOG("cg_array_read() value valid", z == read_z);

	cg_close(fid);

	remove("case_gridwrite3d.cgn");
}

} // extern "C"
