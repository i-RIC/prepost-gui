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

void case_Complex()
{
	QFile::copy("case_init.cgn", "case_complex.cgn");

	int fid;
	int ier = cg_open("case_complex.cgn", CG_MODE_MODIFY, &fid);

	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	ier = cg_iRIC_Clear_Complex_Mul(fid);
	VERIFY_LOG("cg_iRIC_Clear_Complex_Mul() ier == 0", ier == 0);

	int intparam_write = 71;
	ier = cg_iRIC_Write_Complex_Integer_Mul(fid, const_cast<char*>("cellparam"), 1, const_cast<char*>("intparam"), intparam_write);
	VERIFY_LOG("cg_iRIC_Write_Complex_Integer_Mul() ier == 0", ier == 0);

	int intparam2_write = 48;
	ier = cg_iRIC_Write_Complex_Integer_Mul(fid, const_cast<char*>("cellparam"), 2, const_cast<char*>("intparam"), intparam2_write);
	VERIFY_LOG("cg_iRIC_Write_Complex_Integer_Mul() ier == 0", ier == 0);

	double realparam_write = 1.23;
	ier = cg_iRIC_Write_Complex_Real_Mul(fid, const_cast<char*>("cellparam"), 1, const_cast<char*>("realparam"), realparam_write);
	VERIFY_LOG("cg_iRIC_Write_Complex_Real_Mul() ier == 0", ier == 0);

	double realparam2_write = 8.92;
	ier = cg_iRIC_Write_Complex_Real_Mul(fid, const_cast<char*>("cellparam"), 2, const_cast<char*>("realparam"), realparam2_write);
	VERIFY_LOG("cg_iRIC_Write_Complex_Real_Mul() ier == 0", ier == 0);

	cgsize_t isize, jsize;
	ier = cg_iRIC_GotoGridCoord2d_Mul(fid, &isize, &jsize);
	VERIFY_LOG("cg_iRIC_GotoGridCoord2d_Mul() ier == 0", ier == 0);

	std::vector<int> complex_cell;
	complex_cell.assign((isize - 1) * (jsize - 1), 1);
	for (int i = 3; i < complex_cell.size(); i = i + 4) {
		complex_cell[i] = 2;
	}
	ier = cg_iRIC_Write_Grid_Complex_Cell_Mul(fid, const_cast<char*>("cellparam"), complex_cell.data());
	VERIFY_LOG("cg_iRIC_Write_Grid_Complex_Cell_Mul() ier == 0", ier == 0);

	std::string strparam_write = "iriclib1";
	ier = cg_iRIC_Write_Complex_String_Mul(fid, const_cast<char*>("nodeparam"), 1, const_cast<char*>("strparam"), const_cast<char*>(strparam_write.c_str()));
	VERIFY_LOG("cg_iRIC_Write_Complex_String_Mul() ier == 0", ier == 0);

	std::string strparam2_write = "cgnslib2";
	ier = cg_iRIC_Write_Complex_String_Mul(fid, const_cast<char*>("nodeparam"), 2, const_cast<char*>("strparam"), const_cast<char*>(strparam2_write.c_str()));
	VERIFY_LOG("cg_iRIC_Write_Complex_String_Mul() ier == 0", ier == 0);

	std::vector<double> params_write, vals_write;
	params_write.assign(5, 0);
	vals_write.assign(5, 0);

	for (int i = 0; i < 5; ++i){
		params_write[i] = 2.3 * i + 1.2;
		vals_write[i] = 5.1 * i + 0.8;
	}
	ier = cg_iRIC_Write_Complex_Functional_Mul(fid, const_cast<char*>("nodeparam"), 1, const_cast<char*>("funcparam"), 5, params_write.data(), vals_write.data());
	VERIFY_LOG("cg_iRIC_Write_Complex_Functional_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_Complex_FunctionalWithName_Mul(fid, const_cast<char*>("nodeparam"), 2, const_cast<char*>("funcparam"), const_cast<char*>("Time"), 5, params_write.data());
	VERIFY_LOG("cg_iRIC_Write_Complex_FunctionalWithName_Mul() ier == 0", ier == 0);
	ier = cg_iRIC_Write_Complex_FunctionalWithName_Mul(fid, const_cast<char*>("nodeparam"), 2, const_cast<char*>("funcparam"), const_cast<char*>("Elevation"), 5, vals_write.data());
	VERIFY_LOG("cg_iRIC_Write_Complex_FunctionalWithName_Mul() ier == 0", ier == 0);

	std::vector<int> complex_node;
	complex_node.assign(isize * jsize, 1);
	for (int i = 3; i < complex_node.size(); i = i + 5) {
		complex_node[i] = 2;
	}
	ier = cg_iRIC_Write_Grid_Complex_Node_Mul(fid, const_cast<char*>("nodeparam"), complex_node.data());
	VERIFY_LOG("cg_iRIC_Write_Grid_Complex_Node_Mul() ier == 0", ier == 0);

	cg_close(fid);

	ier = cg_open("case_complex.cgn", CG_MODE_MODIFY, &fid);

	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	int cellnum;
	ier = cg_iRIC_Read_Complex_Count_Mul(fid, const_cast<char*>("cellparam"), &cellnum);
	VERIFY_LOG("cg_iRIC_Read_Complex_Count_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Complex_Count_Mul() cellnum == 2", ier == 0);

	ier = cg_iRIC_Read_Complex_Count_Mul(fid, const_cast<char*>("dummy_param"), &cellnum);
	VERIFY_LOG("cg_iRIC_Read_Complex_Count_Mul() ier != 0 for invalid name", ier != 0);

	int intparam_read;
	ier = cg_iRIC_Read_Complex_Integer_Mul(fid, const_cast<char*>("cellparam"), 1, const_cast<char*>("intparam"), &intparam_read);
	VERIFY_LOG("cg_iRIC_Read_Complex_Integer_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Complex_Integer_Mul() value valid", intparam_read == intparam_write);

	double realparam_read;
	ier = cg_iRIC_Read_Complex_Real_Mul(fid, const_cast<char*>("cellparam"), 1, const_cast<char*>("realparam"), &realparam_read);
	VERIFY_LOG("cg_iRIC_Read_Complex_Real_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Complex_Real_Mul() value valid", realparam_read == realparam_write);

	std::vector<int> complex_cell_read;
	complex_cell_read.assign((isize - 1) * (jsize - 1), 0);
	ier = cg_iRIC_Read_Grid_Complex_Cell_Mul(fid, const_cast<char*>("cellparam"), complex_cell_read.data());
	VERIFY_LOG("cg_iRIC_Read_Grid_Complex_Cell_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Grid_Complex_Cell_Mul() value valid", complex_cell == complex_cell_read);

	int strparam_len_read;
	std::vector<char> strparam_read;

	ier = cg_iRIC_Read_Complex_StringLen_Mul(fid, const_cast<char*>("nodeparam"), 1, const_cast<char*>("strparam"), &strparam_len_read);
	VERIFY_LOG("cg_iRIC_Read_Complex_StringLen_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Complex_StringLen_Mul() strlen valid", strparam_len_read == 8);

	strparam_read.assign(strparam_len_read + 1, ' ');
	ier = cg_iRIC_Read_Complex_String_Mul(fid, const_cast<char*>("nodeparam"), 1, const_cast<char*>("strparam"), strparam_read.data());
	VERIFY_LOG("cg_iRIC_Read_Complex_String_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Complex_String_Mul() string valid", std::string(strparam_read.data()) == strparam_write);

	cgsize_t funclen;
	ier = cg_iRIC_Read_Complex_FunctionalSize_Mul(fid, const_cast<char*>("nodeparam"), 1, const_cast<char*>("funcparam"), &funclen);

	std::vector<double> params_read, vals_read;
	params_read.assign(funclen, 0);
	vals_read.assign(funclen, 0);
	ier = cg_iRIC_Read_Complex_Functional_Mul(fid, const_cast<char*>("nodeparam"), 1, const_cast<char*>("funcparam"), params_read.data(), vals_read.data());
	VERIFY_LOG("cg_iRIC_Read_Complex_Functional_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Complex_Functional_Mul() params valid", params_read == params_write);
	VERIFY_LOG("cg_iRIC_Read_Complex_Functional_Mul() vals valid", vals_read == vals_write);

	std::vector<double> times_read, elevs_read;
	times_read.assign(funclen, 0);
	elevs_read.assign(funclen, 0);

	ier = cg_iRIC_Read_Complex_FunctionalWithName_Mul(fid, const_cast<char*>("nodeparam"), 2, const_cast<char*>("funcparam"), const_cast<char*>("Time"), times_read.data());
	VERIFY_LOG("cg_iRIC_Read_Complex_FunctionalWithName_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Complex_FunctionalWithName_Mul() Time valid", times_read == params_write);

	ier = cg_iRIC_Read_Complex_FunctionalWithName_Mul(fid, const_cast<char*>("nodeparam"), 2, const_cast<char*>("funcparam"), const_cast<char*>("Elevation"), elevs_read.data());
	VERIFY_LOG("cg_iRIC_Read_Complex_FunctionalWithName_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Complex_FunctionalWithName_Mul() Elevation valid", elevs_read == vals_write);

	cg_close(fid);

	remove("case_complex.cgn");
}

} // extern "C"
