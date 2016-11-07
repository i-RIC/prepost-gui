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

void case_CalcCondRead()
{
	int fid;
	int ier = cg_open("case_init.cgn", CG_MODE_MODIFY, &fid);

	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	int cond_int;

	ier = cg_iRIC_Read_Integer_Mul(fid, const_cast<char*>("intval"), &cond_int);
	VERIFY_LOG("cg_iRIC_Read_Integer_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Integer_Mul() val == 1", cond_int == 1);

	ier = cg_iRIC_Read_Integer_Mul(fid, const_cast<char*>("intval2"), &cond_int);
	VERIFY_LOG("cg_iRIC_Read_Integer_Mul() ier != 0 for invalid value", ier != 0);

	double cond_double;

	ier = cg_iRIC_Read_Real_Mul(fid, const_cast<char*>("doubleval"), &cond_double);
	VERIFY_LOG("cg_iRIC_Read_Real_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Real_Mul() val == 1", cond_double == 4.21);

	ier = cg_iRIC_Read_Real_Mul(fid, const_cast<char*>("doubleval2"), &cond_double);
	VERIFY_LOG("cg_iRIC_Read_Real_Mul() ier != 0 for invalid value", ier != 0);

	int string_len;
	std::vector<char> strbuffer;

	ier = cg_iRIC_Read_StringLen_Mul(fid, const_cast<char*>("stringval"), &string_len);
	VERIFY_LOG("cg_iRIC_Read_StringLen_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_StringLen_Mul() val == 10", string_len == 10);

	strbuffer.assign(string_len + 1, ' ');
	ier = cg_iRIC_Read_String_Mul(fid, const_cast<char*>("stringval"), strbuffer.data());
	VERIFY_LOG("cg_iRIC_Read_String_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_String_Mul() val == TESTSTRING", std::string("TESTSTRING") == std::string(strbuffer.data()));

	ier = cg_iRIC_Read_StringLen_Mul(fid, const_cast<char*>("stringval2"), &string_len);
	VERIFY_LOG("cg_iRIC_Read_StringLen_Mul() ier != 0 for invalid value", ier != 0);

	ier = cg_iRIC_Read_String_Mul(fid, const_cast<char*>("stringval2"), strbuffer.data());
	VERIFY_LOG("cg_iRIC_Read_String_Mul() ier != 0 for invalid value", ier != 0);

	cgsize_t fsize;
	ier = cg_iRIC_Read_FunctionalSize_Mul(fid, const_cast<char*>("func1"), &fsize);
	VERIFY_LOG("cg_iRIC_Read_FunctionalSize_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_FunctionalSize_Mul() val == 4", fsize == 4);

	std::vector<double> params, values;
	params.assign(fsize, 0);
	values.assign(fsize, 0);

	ier = cg_iRIC_Read_Functional_Mul(fid, const_cast<char*>("func1"), params.data(), values.data());
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() param value[0]", params.at(0) == 0);
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() param value[1]", params.at(1) == 1);
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() param value[2]", params.at(2) == 2);
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() param value[3]", params.at(3) == 3);
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() value value[0]", values.at(0) == 0);
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() value value[1]", values.at(1) == 3);
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() value value[2]", values.at(2) == 4.5);
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() value value[3]", values.at(3) == 2.8);

	ier = cg_iRIC_Read_FunctionalSize_Mul(fid, const_cast<char*>("func100"), &fsize);
	VERIFY_LOG("cg_iRIC_Read_FunctionalSize_Mul() ier != 0 for invalid value", ier != 0);

	ier = cg_iRIC_Read_Functional_Mul(fid, const_cast<char*>("func100"), params.data(), values.data());
	VERIFY_LOG("cg_iRIC_Read_Functional_Mul() ier != 0 for invalid value", ier != 0);

	ier = cg_iRIC_Read_FunctionalSize_Mul(fid, const_cast<char*>("func2"), &fsize);
	VERIFY_LOG("cg_iRIC_Read_FunctionalSize_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_FunctionalSize_Mul() val == 5", fsize == 5);

	params.assign(fsize, 0);
	values.assign(fsize, 0);

	ier = cg_iRIC_Read_FunctionalWithName_Mul(fid, const_cast<char*>("func2"), const_cast<char*>("time"), params.data());
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() ier == 0", ier == 0);
	ier = cg_iRIC_Read_FunctionalWithName_Mul(fid, const_cast<char*>("func2"), const_cast<char*>("value2"), values.data());
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() ier == 0", ier == 0);

	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() param value[0]", params.at(0) == 0);
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() param value[1]", params.at(1) == 3);
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() param value[2]", params.at(2) == 4);
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() param value[3]", params.at(3) == 6);
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() param value[4]", params.at(4) == 8);

	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() value value[0]", values.at(0) == 4.5);
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() value value[1]", values.at(1) == 8);
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() value value[2]", values.at(2) == 5);
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() value value[3]", values.at(3) == 4);
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() value value[4]", values.at(4) == 4.2);

	ier = cg_iRIC_Read_FunctionalWithName_Mul(fid, const_cast<char*>("func200"), const_cast<char*>("time"), params.data());
	VERIFY_LOG("cg_iRIC_Read_FunctionalWithName_Mul() ier != 0 for invalid value", ier != 0);

	float floatv;

	ier = cg_iRIC_Read_RealSingle_Mul(fid, const_cast<char*>("singleval"), &floatv);
	VERIFY_LOG("cg_iRIC_Read_RealSingle_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_RealSingle_Mul() val == 3.88", fabs(floatv - 3.88) < 0.001);

	ier = cg_iRIC_Read_RealSingle_Mul(fid, const_cast<char*>("singleval2"), &floatv);
	VERIFY_LOG("cg_iRIC_Read_RealSingle_Mul() ier != 0 for invalid name ", ier != 0);

	std::vector<float> floatparams, floatvalues;

	floatparams.assign(4, 0);
	floatvalues.assign(4, 0);

	ier = cg_iRIC_Read_Functional_RealSingle_Mul(fid, const_cast<char*>("func1"), floatparams.data(), floatvalues.data());
	VERIFY_LOG("cg_iRIC_Read_Functional_RealSingle_Mul() ier == 0", ier == 0);

	VERIFY_LOG("cg_iRIC_Read_Functional_RealSingle_Mul() param value[0]", floatparams.at(0) == 0);
	VERIFY_LOG("cg_iRIC_Read_Functional_RealSingle_Mul() param value[1]", floatparams.at(1) == 1);
	VERIFY_LOG("cg_iRIC_Read_Functional_RealSingle_Mul() param value[2]", floatparams.at(2) == 2);
	VERIFY_LOG("cg_iRIC_Read_Functional_RealSingle_Mul() param value[3]", floatparams.at(3) == 3);
	VERIFY_LOG("cg_iRIC_Read_Functional_RealSingle_Mul() value value[0]", floatvalues.at(0) == 0);
	VERIFY_LOG("cg_iRIC_Read_Functional_RealSingle_Mul() value value[1]", floatvalues.at(1) == 3);
	VERIFY_LOG("cg_iRIC_Read_Functional_RealSingle_Mul() value value[2]", floatvalues.at(2) == 4.5);
	VERIFY_LOG("cg_iRIC_Read_Functional_RealSingle_Mul() value value[3]", fabs(floatvalues.at(3) - 2.8) < 0.001);

	cg_close(fid);
}

void case_CalcCondWrite()
{
	QFile::copy("case_init.cgn", "case_ccwrite.cgn");

	int fid;
	int ier = cg_open("case_ccwrite.cgn", CG_MODE_MODIFY, &fid);

	int fid_wrong = 9999;

	ier = cg_iRIC_Init(fid);
	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	int write_int = 121;
	ier = cg_iRIC_Write_Integer_Mul(fid, const_cast<char*>("write_int"), write_int);
	VERIFY_LOG("cg_iRIC_Write_Integer_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_Integer_Mul(fid_wrong, const_cast<char*>("write_int"), write_int);
	VERIFY_LOG("cg_iRIC_Write_Integer_Mul() ier != 0 for invalid fid", ier != 0);

	double write_double = 3.14159;
	ier = cg_iRIC_Write_Real_Mul(fid, const_cast<char*>("write_double"), write_double);
	VERIFY_LOG("cg_iRIC_Write_Real_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_Real_Mul(fid_wrong, const_cast<char*>("write_double"), write_double);
	VERIFY_LOG("cg_iRIC_Write_Real_Mul() ier != 0 for invalid fid", ier != 0);


	remove("case_ccwrite.cgn");
}

} // extern "C"
