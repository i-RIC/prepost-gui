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

void case_BcRead()
{
	remove("case_bc.cgn");
	QFile::copy("case_init.cgn", "case_bc.cgn");

	int fid;
	int ier = cg_open("case_bc.cgn", CG_MODE_MODIFY, &fid);

	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);

	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	int bc_count;

	cg_iRIC_Read_BC_Count_Mul(fid, const_cast<char*>("bctest"), &bc_count);
	VERIFY_LOG("cg_iRIC_Read_BC_Count_Mul() bc_count == 2", bc_count == 2);

	cgsize_t bc_indicessize;

	ier = cg_iRIC_Read_BC_IndicesSize_Mul(fid, const_cast<char*>("bctest"), 1, &bc_indicessize);
	VERIFY_LOG("cg_iRIC_Read_BC_IndicesSize_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_IndicesSize_Mul() bc_indicessize == 5", bc_indicessize == 5);

	ier = cg_iRIC_Read_BC_IndicesSize_Mul(fid, const_cast<char*>("bctestaa"), 1, &bc_indicessize);
	VERIFY_LOG("cg_iRIC_Read_BC_IndicesSize_Mul() ier != 0 if name is invalid", ier != 0);

	ier = cg_iRIC_Read_BC_IndicesSize_Mul(fid, const_cast<char*>("bctest"), 3, &bc_indicessize);
	VERIFY_LOG("cg_iRIC_Read_BC_IndicesSize_Mul() ier != 0 if num is invalid", ier != 0);

	std::vector<cgsize_t> indices;
	indices.assign(10, 0);

	ier = cg_iRIC_Read_BC_Indices_Mul(fid, const_cast<char*>("bctest"), 1, indices.data());
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[0] == 1", indices[0] == 1);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[1] == 1", indices[1] == 1);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[2] == 1", indices[2] == 1);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[3] == 2", indices[3] == 2);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[4] == 1", indices[4] == 1);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[5] == 3", indices[5] == 3);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[6] == 1", indices[6] == 1);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[7] == 4", indices[7] == 4);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[8] == 1", indices[8] == 1);
	VERIFY_LOG("cg_iRIC_Read_BC_Indices_Mul() indices[9] == 5", indices[9] == 5);

	int cond_int;

	ier = cg_iRIC_Read_BC_Integer_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("intparam"), &cond_int);
	VERIFY_LOG("cg_iRIC_Read_BC_Integer_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_Integer_Mul() val == 1", cond_int == 1);

	ier = cg_iRIC_Read_BC_Integer_Mul(fid, const_cast<char*>("bctestaa"), 1, const_cast<char*>("intparam"), &cond_int);
	VERIFY_LOG("cg_iRIC_Read_BC_Integer_Mul() ier != 0 for invalid value", ier != 0);

	ier = cg_iRIC_Read_BC_Integer_Mul(fid, const_cast<char*>("bctest"), 4, const_cast<char*>("intparam"), &cond_int);
	VERIFY_LOG("cg_iRIC_Read_BC_Integer_Mul() ier != 0 for invalid value", ier != 0);

	ier = cg_iRIC_Read_BC_Integer_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("intparam2"), &cond_int);
	VERIFY_LOG("cg_iRIC_Read_BC_Integer_Mul() ier != 0 for invalid value", ier != 0);

	double cond_double;

	ier = cg_iRIC_Read_BC_Real_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("realparam"), &cond_double);
	VERIFY_LOG("cg_iRIC_Read_BC_Real_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_Real_Mul() val == 4.2", cond_double == 4.2);

	ier = cg_iRIC_Read_BC_Real_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("realparam2"), &cond_double);
	VERIFY_LOG("cg_iRIC_Read_BC_Real_Mul() ier != 0 for invalid value", ier != 0);

	int string_len;
	std::vector<char> strbuffer;

	ier = cg_iRIC_Read_BC_StringLen_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("stringparam"), &string_len);
	VERIFY_LOG("cg_iRIC_Read_BC_StringLen_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_StringLen_Mul() val == 3", string_len == 3);

	strbuffer.assign(string_len + 1, ' ');
	ier = cg_iRIC_Read_BC_String_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("stringparam"), strbuffer.data());
	VERIFY_LOG("cg_iRIC_Read_BC_String_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_String_Mul() val == AAA", std::string("AAA") == std::string(strbuffer.data()));

	ier = cg_iRIC_Read_BC_StringLen_Mul(fid, const_cast<char*>("bctest_dummy"), 1, const_cast<char*>("stringparam"), &string_len);
	VERIFY_LOG("cg_iRIC_Read_BC_StringLen_Mul() ier != 0", ier != 0);

	ier = cg_iRIC_Read_BC_StringLen_Mul(fid, const_cast<char*>("bctest"), 4, const_cast<char*>("stringparam"), &string_len);
	VERIFY_LOG("cg_iRIC_Read_BC_StringLen_Mul() ier != 0", ier != 0);

	ier = cg_iRIC_Read_BC_StringLen_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("stringparam2"), &string_len);
	VERIFY_LOG("cg_iRIC_Read_BC_StringLen_Mul() ier != 0", ier != 0);

	ier = cg_iRIC_Read_BC_String_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("stringval2"), strbuffer.data());
	VERIFY_LOG("cg_iRIC_Read_BC_String_Mul() ier != 0 for invalid value", ier != 0);

	cgsize_t fsize;
	ier = cg_iRIC_Read_BC_FunctionalSize_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("funcparam"), &fsize);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalSize_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalSize_Mul() val == 4", fsize == 3);

	std::vector<double> params, values;
	params.assign(fsize, 0);
	values.assign(fsize, 0);

	ier = cg_iRIC_Read_BC_Functional_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("funcparam"), params.data(), values.data());
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() param value[0]", params.at(0) == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() param value[1]", params.at(1) == 1);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() param value[2]", params.at(2) == 2);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() value value[0]", values.at(0) == 5);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() value value[1]", values.at(1) == 4);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() value value[2]", values.at(2) == 2);

	ier = cg_iRIC_Read_BC_FunctionalSize_Mul(fid, const_cast<char*>("bctest_dmmy"), 1, const_cast<char*>("funcparam"), &fsize);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalSize_Mul() ier != 0 for invalid value", ier != 0);

	ier = cg_iRIC_Read_BC_FunctionalSize_Mul(fid, const_cast<char*>("bctest"), 5, const_cast<char*>("funcparam"), &fsize);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalSize_Mul() ier != 0 for invalid value", ier != 0);

	ier = cg_iRIC_Read_BC_FunctionalSize_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("funcpareem"), &fsize);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalSize_Mul() ier != 0 for invalid value", ier != 0);

	ier = cg_iRIC_Read_BC_Functional_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("func100"), params.data(), values.data());
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() ier != 0 for invalid value", ier != 0);

	ier = cg_iRIC_Read_BC_FunctionalWithName_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("funcparam"), const_cast<char*>("Param"), params.data());
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalWithName_Mul() ier == 0", ier == 0);
	ier = cg_iRIC_Read_BC_FunctionalWithName_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("funcparam"), const_cast<char*>("Value"), values.data());
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalWithName_Mul() ier == 0", ier == 0);

	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalWithName_Mul() param value[0]", params.at(0) == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalWithName_Mul() param value[1]", params.at(1) == 1);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalWithName_Mul() param value[2]", params.at(2) == 2);

	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalWithName_Mul() value value[0]", values.at(0) == 5);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalWithName_Mul() value value[1]", values.at(1) == 4);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalWithName_Mul() value value[2]", values.at(2) == 2);

	float floatv;

	ier = cg_iRIC_Read_BC_RealSingle_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("realparam"), &floatv);
	VERIFY_LOG("cg_iRIC_Read_BC_RealSingle_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_RealSingle_Mul() val == 4.2", fabs(floatv - 4.2) < 0.001);

	ier = cg_iRIC_Read_BC_RealSingle_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("realparam2"), &floatv);
	VERIFY_LOG("cg_iRIC_Read_BC_RealSingle_Mul() ier != 0 for invalid name ", ier != 0);

	std::vector<float> floatparams, floatvalues;

	floatparams.assign(3, 0);
	floatvalues.assign(3, 0);

	ier = cg_iRIC_Read_BC_Functional_RealSingle_Mul(fid, const_cast<char*>("bctest"), 1, const_cast<char*>("funcparam"), floatparams.data(), floatvalues.data());
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_RealSingle_Mul() ier == 0", ier == 0);

	VERIFY_LOG("cg_iRIC_Read_BC_Functional_RealSingle_Mul() param value[0]", floatparams.at(0) == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_RealSingle_Mul() param value[1]", floatparams.at(1) == 1);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_RealSingle_Mul() param value[2]", floatparams.at(2) == 2);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_RealSingle_Mul() value value[0]", floatvalues.at(0) == 5);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_RealSingle_Mul() value value[1]", floatvalues.at(1) == 4);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_RealSingle_Mul() value value[2]", floatvalues.at(2) == 2);

	cg_close(fid);

	remove("case_bc.cgn");
}

void case_BcWrite()
{
	QFile::copy("case_init.cgn", "case_bcwrite.cgn");

	int fid;
	int fid_wrong = 9999;

	int ier = cg_open("case_bcwrite.cgn", CG_MODE_MODIFY, &fid);
	VERIFY_LOG("cg_open() ier == 0", ier == 0);
	VERIFY_LOG("cg_open() fid != 0", fid != 0);

	ier = cg_iRIC_Init(fid);
	VERIFY_LOG("cg_iRIC_Init() ier == 0", ier == 0);

	ier = cg_iRIC_Clear_BC_Mul(fid);
	VERIFY_LOG("cg_iRIC_Clear_BC_Mul() ier == 0", ier == 0);

	std::vector<cgsize_t> indices;
	indices.assign(10, 1);
	for (int i = 0; i < 5; ++i) {
		indices[i * 2 + 1] = i + 3;
	};
	ier = cg_iRIC_Write_BC_Indices_Mul(fid, const_cast<char*>("testbc"), 1, 5, indices.data());
	VERIFY_LOG("cg_iRIC_Write_BC_Indices_Mul() ier == 0", ier == 0);

	int write_int = 121;
	ier = cg_iRIC_Write_BC_Integer_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_int"), write_int);
	VERIFY_LOG("cg_iRIC_Write_BC_Integer_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_BC_Integer_Mul(fid_wrong, const_cast<char*>("testbc"), 1, const_cast<char*>("write_int"), write_int);
	VERIFY_LOG("cg_iRIC_Write_BC_Integer_Mul() ier != 0 for invalid fid", ier != 0);

	double write_double = 3.14159;
	ier = cg_iRIC_Write_BC_Real_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_double"), write_double);
	VERIFY_LOG("cg_iRIC_Write_BC_Real_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_BC_Real_Mul(fid_wrong, const_cast<char*>("testbc"), 1, const_cast<char*>("write_double"), write_double);
	VERIFY_LOG("cg_iRIC_Write_BC_Real_Mul() ier != 0 for invalid fid", ier != 0);

	std::string write_str = "TEST_STRING_3.14";
	ier = cg_iRIC_Write_BC_String_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_string"), const_cast<char*>(write_str.c_str()));
	VERIFY_LOG("cg_iRIC_Write_BC_String_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_BC_String_Mul(fid_wrong, const_cast<char*>("testbc"), 1, const_cast<char*>("write_string"), const_cast<char*>(write_str.c_str()));
	VERIFY_LOG("cg_iRIC_Write_BC_String_Mul() ier != 0 for invalid fid", ier != 0);

	std::vector<double> params, vals;
	params.assign(5, 0);
	vals.assign(5, 0);

	for (int i = 0; i < 5; ++i){
		params[i] = 2.3 * i + 1.2;
		vals[i] = 5.1 * i + 0.8;
	}
	ier = cg_iRIC_Write_BC_Functional_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_func"), 5, params.data(), vals.data());
	VERIFY_LOG("cg_iRIC_BC_Write_Functional_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_BC_Functional_Mul(fid_wrong, const_cast<char*>("testbc"), 1, const_cast<char*>("write_func"), 5, params.data(), vals.data());
	VERIFY_LOG("cg_iRIC_BC_Write_Functional_Mul() ier != 0 for invalid fid", ier != 0);

	ier = cg_iRIC_Write_BC_FunctionalWithName_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_func_withname"), const_cast<char*> ("time"), params.size(), params.data());
	VERIFY_LOG("cg_iRIC_BC_Write_FunctionalWithName_Mul() ier == 0", ier == 0);
	ier = cg_iRIC_Write_BC_FunctionalWithName_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_func_withname"), const_cast<char*> ("elev"), vals.size(), vals.data());
	VERIFY_LOG("cg_iRIC_BC_Write_FunctionalWithName_Mul() ier == 0", ier == 0);

	ier = cg_iRIC_Write_BC_FunctionalWithName_Mul(fid_wrong, const_cast<char*>("testbc"), 1, const_cast<char*>("write_func_withname"), const_cast<char*> ("time"), params.size(), params.data());
	VERIFY_LOG("cg_iRIC_Write_FunctionalWithName_Mul() ier != 0 for invalid fid", ier != 0);

	int read_num;
	cg_iRIC_Read_BC_Count_Mul(fid, const_cast<char*>("testbc"), &read_num);
	VERIFY_LOG("cg_iRIC_Read_BC_Count_Mul() read_num == 1", read_num == 1);

	cgsize_t read_indicessize;
	ier = cg_iRIC_Read_BC_IndicesSize_Mul(fid, const_cast<char*>("testbc"), 1, &read_indicessize);
	VERIFY_LOG("cg_iRIC_Read_BC_IndicesSize_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_IndicesSize_Mul() read_indicessize == 5", read_indicessize == 5);

	int read_int;
	ier = cg_iRIC_Read_BC_Integer_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_int"), &read_int);
	VERIFY_LOG("cg_iRIC_Read_BC_Integer_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_Integer_Mul() value match", read_int == write_int);

	double read_double;
	ier = cg_iRIC_Read_BC_Real_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_double"), &read_double);
	VERIFY_LOG("cg_iRIC_Read_BC_Real_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_Real_Mul() value match", read_double == read_double);

	int read_strlen;
	std::vector<char> read_str;
	ier = cg_iRIC_Read_BC_StringLen_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_string"), &read_strlen);
	read_str.assign(read_strlen + 1, ' ');
	ier = cg_iRIC_Read_BC_String_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_string"), read_str.data());
	VERIFY_LOG("cg_iRIC_Read_BC_String_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_String_Mul() value match", std::string(read_str.data()) == write_str);

	int func_len;
	std::vector<double> read_params, read_vals;
	ier = cg_iRIC_Read_BC_FunctionalSize_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_func"), &func_len);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalSize_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_FunctionalSize_Mul() value == 5", func_len == 5);

	read_params.assign(func_len, 0);
	read_vals.assign(func_len, 0);
	ier = cg_iRIC_Read_BC_Functional_Mul(fid, const_cast<char*>("testbc"), 1, const_cast<char*>("write_func"), read_params.data(), read_vals.data());
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() ier == 0", ier == 0);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() param match", params == read_params);
	VERIFY_LOG("cg_iRIC_Read_BC_Functional_Mul() value match", vals == read_vals);

	cg_close(fid);

	remove("case_bcwrite.cgn");
}

} // extern "C"
