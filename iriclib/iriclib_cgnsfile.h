#ifndef IRICLIB_CGNSFILE_H
#define IRICLIB_CGNSFILE_H

#include "iriclib_global.h"

namespace iRICLib {

class CgnsFile
{
public:
	CgnsFile();
	~CgnsFile();

	void setFileId(int fileId);

	// -----------------------------------
	// Initialization and Basic functions
	// -----------------------------------
	int Init();
	int InitRead_Base(char* basename);
	int InitRead();

	int GotoBase(int* B);
	int GotoCC();
	int GotoRawDataTop();

	int Set_ZoneId(int zoneid);

	// ------------------------------
	// Calculation Condition Reading
	// ------------------------------
	int CC_CC_Read_Integer(char* name, int* intvalue);
	int CC_Read_Real(char* name, double* realvalue);
	int CC_Read_RealSingle(char* name, float* realvalue);
	int CC_Read_StringLen(char* name, int* length);
	int CC_Read_String(char* name, char* strvalue);
	int CC_Read_FunctionalSize(char* name, cgsize_t* size);
	int CC_Read_Functional(char* name, double* x, double* y);
	int CC_Read_FunctionalWithName(char* name, char* paramname, double* data);
	int CC_Read_Functional_RealSingle(char* name, float* x, float* y);

	int CC_Write_Integer(char* name, int intvalue);
	int CC_Write_Real(char* name, double realvalue);
	int CC_Write_String(char* name, char* strvalue);
	int CC_Write_Functional(char* name, int length, double* realarray_x, double* realarray_y);
	int CC_Write_FunctionalWithName(char* name, char* paramname, int length, double* data);

	// --------------------------------------
	// Complex Calculation Condition Reading
	// --------------------------------------

	int Complex_CC_Read_Count(char* groupname, int* num);
	int Complex_CC_Read_Integer(char* groupname, int num, char* name, int* intvalue);
	int Complex_CC_Read_Real(char* groupname, int num, char* name, double* realvalue);
	int Complex_CC_Read_RealSingle(char* groupname, int num, char* name, float* realvalue);
	int Complex_CC_Read_StringLen(char* groupname, int num, char* name, int* length);
	int Complex_CC_Read_String(char* groupname, int num, char* name, char* strvalue);
	int Complex_CC_Read_FunctionalSize(char* groupname, int num, char* name, cgsize_t* size);
	int Complex_CC_Read_Functional(char* groupname, int num, char* name, double* x, double* y);
	int Complex_CC_Read_FunctionalWithName(char* groupname, int num, char* name, char* paramname, double* data);
	int Complex_CC_Read_Functional_RealSingle(char* groupname, int num, char* name, float* x, float* y);
	int Complex_CC_Read_Grid_Complex_Node(char* groupname, int* values);
	int Complex_CC_Read_Grid_Complex_Cell(char *groupname, int* values);

	int Complex_CC_Clear_Complex(int fid);
	int Complex_CC_Write_Integer(char* groupname, int num, char* name, int intvalue);
	int Complex_CC_Write_Real(char* groupname, int num, char* name, double realvalue);
	int Complex_CC_Write_String(char* groupname, int num, char* name, char* strvalue);
	int Complex_CC_Write_Functional(char* groupname, int num, char* name, int length, double* realarray_x, double* realarray_y);
	int Complex_CC_Write_FunctionalWithName(char* groupname, int num, char* name, char* paramname, int length, double* data);
	int Complex_CC_Write_Grid_Complex_Node(char* groupname, int* values);
	int Complex_CC_Write_Grid_Complex_Cell(char *groupname, int* values);

private:
	Impl* impl;
};

} // iRICLib

#endif // IRICLIB_CGNSFILE_H
