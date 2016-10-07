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

	// ---------
	// Grid I/O
	// ---------

	int Grid_GotoCoord2d(cgsize_t* isize, cgsize_t* jsize);
	int Grid_GetCoord2d(double *x, double *y);
	int Grid_GotoCoord3d(cgsize_t* isize, cgsize_t* jsize, cgsize_t* ksize);
	int Grid_GetGridCoord3d(double *x, double *y, double *z);
	int Grid_Read_Real_Node(char* name, double* values);
	int Grid_Read_Integer_Node(char* name, int* values);
	int Grid_Read_Real_Cell(char *name, double* values);
	int Grid_Read_Integer_Cell(char *name, int* values);
	int Grid_Read_FunctionalDimensionSize(char* name, char* dimname, cgsize_t* count);
	int Grid_Read_FunctionalDimension_Integer(char* name, char* dimname, int* value);
	int Grid_Read_FunctionalDimension_Real(char* name, char* dimname, double* value);
	int Grid_Read_FunctionalTimeSize(char* name, cgsize_t* count);
	int Grid_Read_FunctionalTime(char* name, double* time);
	int Grid_Read_Functional_Integer_Node(char* name, int Grid_dimid, int* value);
	int Grid_Read_Functional_Real_Node(char* name, int Grid_dimid, double* value);
	int Grid_Read_Functional_Integer_Cell(char* name, int Grid_dimid, int* value);
	int Grid_Read_Functional_Real_Cell(char* name, int Grid_dimid, double* value);

	int Grid_Init(int Grid_zoneId);
	int Grid_WriteGridCoord1d(cgsize_t isize, double* x);
	int Grid_WriteGridCoord2d(cgsize_t isize, cgsize_t jsize, double* x, double* y);
	int Grid_WriteGridCoord3d(cgsize_t isize, cgsize_t jsize, cgsize_t ksize, double* x, double* y, double* z);
	int Grid_Write_Real_Node(char* name, double* values);
	int Grid_Write_Integer_Node(char* name, int* values);
	int Grid_Write_Real_Cell(char *name, double* values);
	int Grid_Write_Integer_Cell(char *name, int* values);

	// -----------------------
	// Boundary Condition I/O
	// -----------------------

	void BC_Init_Names();
	void BC_Read_Count(char* type, int* num);
	int BC_Read_IndicesSize(char* type, int num, cgsize_t* size);
	int BC_Read_Indices(char* type, int num, cgsize_t* indices);
	int BC_Read_Integer(char* type, int num, char* name, int* intvalue);
	int BC_Read_Real(char* type, int num, char* name, double* realvalue);
	int BC_Read_RealSingle(char* type, int num, char* name, float* realvalue);
	int BC_Read_StringLen(char* type, int num, char* name, int* length);
	int BC_Read_String(char* type, int num, char* name, char* strvalue);
	int BC_Read_FunctionalSize(char* type, int num, char* name, cgsize_t* size);
	int BC_Read_Functional(char* type, int num, char* name, double* x, double* y);
	int BC_Read_FunctionalWithName(char* type, int num, char* name, char* paramname, double* data);
	int BC_Read_Functional_RealSingle(char* type, int num, char* name, float* x, float* y);

	int BC_Clear();
	int BC_Write_Indices(char* type, int num, cgsize_t size, cgsize_t* indices);
	int BC_Write_Integer(char* type, int num, char* name, int intvalue);
	int BC_Write_Real(char* type, int num, char* name, double realvalue);
	int BC_Write_String(char* type, int num, char* name, char* strvalue);
	int BC_Write_Functional(char* type, int num, char* name, int length, double* realarray_x, double* realarray_y);
	int BC_Write_FunctionalWithName(char* type, int num, char* name, char* paramname, int length, double* data);

	// -------------
	// Solution I/O
	// -------------

	int Sol_Read_Count(int* count);
	int Sol_Read_Time(int step, double* time);
	int Sol_Read_Iteration(int step, int* index);
	int Sol_Read_BaseIterative_Integer(int step, char* name, int* value);
	int Sol_Read_BaseIterative_Real(int step, char* name, double* value);
	int Sol_Read_GridCoord2d(int step, double* x, double* y);
	int Sol_Read_GridCoord3d(int step, double* x, double* y, double* z);
	int Sol_Read_Integer(int step, char *name, int* data);
	int Sol_Read_Real(int step, char *name, double* data);

	int Sol_Write_Time(double time);
	int Sol_Write_Iteration(int index);
	int Sol_Write_BaseIterative_Integer(char *name, int value);
	int Sol_Write_BaseIterative_Real(char *name, double value);
	int Sol_Write_GridCoord2d(double *x, double *y);
	int Sol_Write_GridCoord3d(double *x, double *y, double *z);
	int Sol_Write_Integer(char *name, int* data);
	int Sol_Write_Real(char *name, double* data);

	// --------------
	// ErrorCode I/O
	// --------------

	int ErrorCode_Write(int errorcode);

	// ----------------------
	// Solution Particle I/O
	// ----------------------

	int Sol_Particle_Write_Pos2d(cgsize_t count, double* x, double* y);
	int Sol_Particle_Write_Pos3d(cgsize_t count, double* x, double* y, double* z);
	int Sol_Particle_Write_Real(char* name, double* value);
	int Sol_Particle_Write_Integer(char* name, int* value);

private:
	Impl* impl;
};

} // iRICLib

#endif // IRICLIB_CGNSFILE_H
