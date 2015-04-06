#include "cgnslib.h"
#include "iriclib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv) {
	int fin, ier;
	int isize, jsize;
	double *grid_x, *grid_y;

	int bctestcount;
	int indexlenmax;
	int funcsizemax;
	int condid;

	int* condindexlen;
	int* condindices;
	int* intparam;
	double* realparam;
	float* realsingleparam;
	char** stringparam;
	char tmpstr[200];
	int* func_size;
	double* func_param;
	double* func_value;
	double* func_withname_param;
	double* func_withname_value;
	float* func_real_param;
	float* func_real_value;

	int i, j, idx;

	// CGNS ファイルのオープン
	ier = cg_open("test.cgn", CG_MODE_MODIFY, &fin);
	if (ier != 0){
		printf("*** Open error of CGNS file ***\n");
		return 1;
	}

	// iRIClib の初期化
	ier = cg_iRIC_Init(fin);

	isize = 10;
	jsize = 8;

	grid_x = (double*) malloc(sizeof(double) * isize * jsize);
	grid_y = (double*) malloc(sizeof(double) * isize * jsize);

	for (j = 0; j < jsize; ++j){
		for (i = 0; i < isize; ++i){
			*(grid_x + i + j * isize) = i;
			*(grid_y + i + j * isize) = j;
		}
	}

	ier = cg_iRIC_WriteGridCoord2d(isize, jsize, grid_x, grid_y);
	// 境界条件の値を作る。

	bctestcount = 2;
	indexlenmax = 5;
	funcsizemax = 10;
	
	condindexlen = (int*)malloc(sizeof(int) * bctestcount);
	condindices = (int*)malloc(sizeof(int) * bctestcount * 2 * indexlenmax);
	intparam = (int*)malloc(sizeof(int) * bctestcount);
	realparam = (double*)malloc(sizeof(double) * bctestcount);
	stringparam = (char**)malloc(sizeof(char*) * bctestcount);
	func_size = (int*)malloc(sizeof(int) * bctestcount);
	func_param = (double*)malloc(sizeof(double) * bctestcount * funcsizemax);
	func_value = (double*)malloc(sizeof(double) * bctestcount * funcsizemax);
	func_withname_param = 
		(double*)malloc(sizeof(double) * bctestcount * funcsizemax);
	func_withname_value = 
		(double*)malloc(sizeof(double) * bctestcount * funcsizemax);

	for (condid = 0; condid < bctestcount; ++condid){
		*(condindexlen + condid) = condid + 4;
		for (idx = 0; idx < *(condindexlen + condid); ++idx){
			*(condindices + condid * (2 * indexlenmax) + idx * 2) = 
				idx + condid + 1;
			*(condindices + condid * (2 * indexlenmax) + idx * 2 + 1) = 
				1;
		}
		*(intparam + condid) = (condid + 1) * 2;
		*(realparam + condid) = (condid + 1) * 3.8;
		*(stringparam + condid) = (char*)malloc(sizeof(char) * 5);
		strcpy(*(stringparam + condid), "test");
		*(func_size + condid) = condid + 8;
		for (idx = 0; idx < *(func_size + condid); ++idx){
			*(func_param + condid * funcsizemax + idx) = idx + 1;
			*(func_value + condid * funcsizemax + idx) = idx + 3;
			*(func_withname_param + condid * funcsizemax + idx) = idx + 2;
			*(func_withname_value + condid * funcsizemax + idx) = 14 - idx;
		}
	}

	ier = cg_iRIC_Clear_BC();

	for (condid = 0; condid < bctestcount; ++condid){
		ier = cg_iRIC_Write_BC_Indices(
			"bctest", condid + 1, *(condindexlen + condid), 
			condindices + condid * (2 * indexlenmax));
		ier = cg_iRIC_Write_BC_Integer(
			"bctest", condid + 1, "intparam", *(intparam + condid));
		ier = cg_iRIC_Write_BC_Real(
			"bctest", condid + 1, "realparam", *(realparam + condid));
		ier = cg_iRIC_Write_BC_String(
			"bctest", condid + 1, "stringparam", *(stringparam + condid));
		ier = cg_iRIC_Write_BC_Functional(
			"bctest", condid + 1, "funcparam", *(func_size + condid), 
			func_param + condid * funcsizemax, func_value + condid * funcsizemax);
		ier = cg_iRIC_Write_BC_FunctionalWithName(
			"bctest", condid + 1, "func_withname", "Param", *(func_size + condid), 
			func_withname_param + condid * funcsizemax);
		ier = cg_iRIC_Write_BC_FunctionalWithName(
			"bctest", condid + 1, "func_withname", "Value", *(func_size + condid), 
			func_withname_value + condid * funcsizemax);
	}
	ier = cg_close(fin);

	return 0;
}
