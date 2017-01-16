#ifndef IRICLIB_CGNSFILE_H
#define IRICLIB_CGNSFILE_H

#include "iriclib_global.h"

#include <cgnslib.h>

namespace iRICLib {

class CgnsFile
{
public:
	CgnsFile();
	~CgnsFile();

	void setFileName(const char* fileName);
	void setFileId(int fileId);

	// -----------------------------------
	// Initialization and Basic functions
	// -----------------------------------
	int Init();
	int InitRead_Base(char* basename);
	int InitRead();

	void OptionDivideSolutions();

	int Flush();

	int GotoBase(int* B);
	int GotoCC();
	int GotoRawDataTop();

	int Set_ZoneId(int zoneid);

	// ------------------------------
	// Calculation Condition Reading
	// ------------------------------
	int CC_Read_Integer(const char* name, int* intvalue);
	int CC_Read_Real(const char* name, double* realvalue);
	int CC_Read_RealSingle(const char* name, float* realvalue);
	int CC_Read_StringLen(const char* name, int* length);
	int CC_Read_String(const char* name, char* strvalue);
	int CC_Read_FunctionalSize(const char* name, cgsize_t* size);
	int CC_Read_Functional(const char* name, double* x, double* y);
	int CC_Read_FunctionalWithName(const char* name, const char* paramname, double* data);
	int CC_Read_Functional_RealSingle(const char* name, float* x, float* y);
	int CC_Read_FunctionalWithName_RealSingle(const char* name, const char* paramname, float* data);

	int CC_Write_Integer(const char* name, int intvalue);
	int CC_Write_Real(const char* name, double realvalue);
	int CC_Write_String(const char* name, char* strvalue);
	int CC_Write_Functional(const char* name, int length, double* realarray_x, double* realarray_y);
	int CC_Write_FunctionalWithName(const char* name, char* paramname, int length, double* data);

	// --------------------------------------
	// Complex Calculation Condition Reading
	// --------------------------------------

	int Complex_CC_Read_Count(const char* groupname, int* count);
	int Complex_CC_Read_Integer(const char* groupname, int num, const char* name, int* intvalue);
	int Complex_CC_Read_Real(const char* groupname, int num, const char* name, double* realvalue);
	int Complex_CC_Read_RealSingle(const char* groupname, int num, const char* name, float* realvalue);
	int Complex_CC_Read_StringLen(const char* groupname, int num, const char* name, int* length);
	int Complex_CC_Read_String(const char* groupname, int num, const char* name, char* strvalue);
	int Complex_CC_Read_FunctionalSize(const char* groupname, int num, const char* name, cgsize_t* size);
	int Complex_CC_Read_Functional(const char* groupname, int num, const char* name, double* x, double* y);
	int Complex_CC_Read_FunctionalWithName(const char* groupname, int num, const char* name, const char *paramname, double* data);
	int Complex_CC_Read_Functional_RealSingle(const char* groupname, int num, const char* name, float* x, float* y);
	int Complex_CC_Read_Grid_Node(const char* groupname, int* values);
	int Complex_CC_Read_Grid_Cell(const char* groupname, int* values);

	int Complex_CC_Clear_Complex();
	int Complex_CC_Write_Integer(const char* groupname, int num, const char* name, int intvalue);
	int Complex_CC_Write_Real(const char* groupname, int num, const char* name, double realvalue);
	int Complex_CC_Write_String(const char* groupname, int num, const char* name, char* strvalue);
	int Complex_CC_Write_Functional(const char* groupname, int num, const char* name, int length, double* realarray_x, double* realarray_y);
	int Complex_CC_Write_FunctionalWithName(const char* groupname, int num, const char* name, const char *paramname, int length, double* data);
	int Complex_CC_Write_Grid_Node(const char* groupname, int* values);
	int Complex_CC_Write_Grid_Cell(const char* groupname, int* values);

	// ---------
	// Grid I/O
	// ---------

	int Grid_GotoCoord2d(cgsize_t* isize, cgsize_t* jsize);
	int Grid_GetCoord2d(double *x, double *y);
	int Grid_GotoCoord3d(cgsize_t* isize, cgsize_t* jsize, cgsize_t* ksize);
	int Grid_GetCoord3d(double *x, double *y, double *z);
	int Grid_Read_Real_Node(const char* name, double* values);
	int Grid_Read_Integer_Node(const char* name, int* values);
	int Grid_Read_Real_Cell(const char *name, double* values);
	int Grid_Read_Integer_Cell(const char *name, int* values);
	int Grid_Read_FunctionalDimensionSize(const char* name, const char* dimname, cgsize_t* count);
	int Grid_Read_FunctionalDimension_Integer(const char* name, const char* dimname, int* value);
	int Grid_Read_FunctionalDimension_Real(const char* name, const char* dimname, double* value);
	int Grid_Read_FunctionalTimeSize(const char* name, cgsize_t* count);
	int Grid_Read_FunctionalTime(const char* name, double* time);
	int Grid_Read_Functional_Integer_Node(const char* name, int dimid, int* value);
	int Grid_Read_Functional_Real_Node(const char* name, int dimid, double* value);
	int Grid_Read_Functional_Integer_Cell(const char* name, int dimid, int* value);
	int Grid_Read_Functional_Real_Cell(const char* name, int dimid, double* value);

	int Grid_Init(int zoneId);
	int Grid_WriteGridCoord1d(cgsize_t isize, double* x);
	int Grid_WriteGridCoord2d(cgsize_t isize, cgsize_t jsize, double* x, double* y);
	int Grid_WriteGridCoord3d(cgsize_t isize, cgsize_t jsize, cgsize_t ksize, double* x, double* y, double* z);
	int Grid_Write_Real_Node(const char* name, double* values);
	int Grid_Write_Integer_Node(const char* name, int* values);
	int Grid_Write_Real_Cell(const char* name, double* values);
	int Grid_Write_Integer_Cell(const char* name, int* values);

	// -----------------------
	// Boundary Condition I/O
	// -----------------------

	void BC_Init_Names();
	void BC_Read_Count(const char* typeName, int* num);
	int BC_Read_IndicesSize(const char* typeName, int num, cgsize_t* size);
	int BC_Read_Indices(const char* typeName, int num, cgsize_t* indices);
	int BC_Read_Integer(const char* typeName, int num, const char* name, int* intvalue);
	int BC_Read_Real(const char* typeName, int num, const char* name, double* realvalue);
	int BC_Read_RealSingle(const char* typeName, int num, const char* name, float* realvalue);
	int BC_Read_StringLen(const char* typeName, int num, const char* name, int* length);
	int BC_Read_String(const char* typeName, int num, const char* name, char* strvalue);
	int BC_Read_FunctionalSize(const char* typeName, int num, const char* name, cgsize_t* size);
	int BC_Read_Functional(const char* typeName, int num, const char* name, double* x, double* y);
	int BC_Read_FunctionalWithName(const char* typeName, int num, const char* name, const char *paramname, double* data);
	int BC_Read_Functional_RealSingle(const char* typeName, int num, const char* name, float* x, float* y);

	int BC_Clear();
	int BC_Write_Indices(const char* typeName, int num, cgsize_t size, cgsize_t* indices);
	int BC_Write_Integer(const char* typeName, int num, const char* name, int intvalue);
	int BC_Write_Real(const char* typeName, int num, const char* name, double realvalue);
	int BC_Write_String(const char* typeName, int num, const char* name, char* strvalue);
	int BC_Write_Functional(const char* typeName, int num, const char* name, int length, double* realarray_x, double* realarray_y);
	int BC_Write_FunctionalWithName(const char* typeName, int num, const char* name, const char *paramname, int length, double* data);

	// -------------
	// Solution I/O
	// -------------

	int Sol_Read_Count(int* count);
	int Sol_Read_Time(int step, double* time);
	int Sol_Read_Iteration(int step, int* index);
	int Sol_Read_BaseIterative_Integer(int step, const char* name, int* value);
	int Sol_Read_BaseIterative_Real(int step, const char* name, double* value);
	int Sol_Read_GridCoord2d(int step, double* x, double* y);
	int Sol_Read_GridCoord3d(int step, double* x, double* y, double* z);
	int Sol_Read_Integer(int step, const char *name, int* data);
	int Sol_Read_Real(int step, const char *name, double* data);

	int Sol_Write_Time(double time);
	int Sol_Write_Iteration(int index);
	int Sol_Write_BaseIterative_Integer(const char *name, int value);
	int Sol_Write_BaseIterative_Real(const char *name, double value);
	int Sol_Write_GridCoord2d(double *x, double *y);
	int Sol_Write_GridCoord3d(double *x, double *y, double *z);
	int Sol_Write_Integer(const char *name, int* data);
	int Sol_Write_Real(const char *name, double* data);

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

	// --------------------
	// Geographic data I/O
	// --------------------

	int Geo_Count(const char* name, int* count);
	int Geo_Filename(const char* name, int geoid, char* filename, int* type);

private:
	class Impl;
	class SolutionWriter;
	class SolutionWriterStandard;
	class SolutionWriterDivideSolutions;

	Impl* impl;
};

} // iRICLib

#ifdef _DEBUG
	#include "private/iriclib_cgnsfile_impl.h"
#endif // _DEBUG

#endif // IRICLIB_CGNSFILE_H
