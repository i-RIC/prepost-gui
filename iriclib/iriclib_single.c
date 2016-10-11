#include "iriclib.h"

extern int lastfileid;

/*********************************************************/
/* The following function implementations are generated  */
/* using make_iriclib_singlefuncs.php                    */
/*********************************************************/

int cg_iRIC_Set_ZoneId(int zoneid)
{
	return cg_iRIC_Set_ZoneId_Mul(lastfileid, zoneid);
}

int cg_iRIC_Read_Integer(char* name, int* intvalue)
{
	return cg_iRIC_Read_Integer_Mul(lastfileid, name, intvalue);
}

int cg_iRIC_Read_Real(char* name, double* realvalue)
{
	return cg_iRIC_Read_Real_Mul(lastfileid, name, realvalue);
}

int cg_iRIC_Read_RealSingle(char* name, float* realvalue)
{
	return cg_iRIC_Read_RealSingle_Mul(lastfileid, name, realvalue);
}

int cg_iRIC_Read_StringLen(char* name, int* length)
{
	return cg_iRIC_Read_StringLen_Mul(lastfileid, name, length);
}

int cg_iRIC_Read_String(char* name, char* strvalue)
{
	return cg_iRIC_Read_String_Mul(lastfileid, name, strvalue);
}

int cg_iRIC_Read_FunctionalSize(char* name, cgsize_t* size)
{
	return cg_iRIC_Read_FunctionalSize_Mul(lastfileid, name, size);
}

int cg_iRIC_Read_Functional(char* name, double* x, double* y)
{
	return cg_iRIC_Read_Functional_Mul(lastfileid, name, x, y);
}

int cg_iRIC_Read_FunctionalWithName(char* name, char* paramname, double* data)
{
	return cg_iRIC_Read_FunctionalWithName_Mul(lastfileid, name, paramname, data);
}

int cg_iRIC_Read_Functional_RealSingle(char* name, float* x, float* y)
{
	return cg_iRIC_Read_Functional_RealSingle_Mul(lastfileid, name, x, y);
}

int cg_iRIC_Write_Integer(char* name, int intvalue)
{
	return cg_iRIC_Write_Integer_Mul(lastfileid, name, intvalue);
}

int cg_iRIC_Write_Real(char* name, double realvalue)
{
	return cg_iRIC_Write_Real_Mul(lastfileid, name, realvalue);
}

int cg_iRIC_Write_String(char* name, char* strvalue)
{
	return cg_iRIC_Write_String_Mul(lastfileid, name, strvalue);
}

int cg_iRIC_Write_Functional(char* name, int length, double* realarray_x, double* realarray_y)
{
	return cg_iRIC_Write_Functional_Mul(lastfileid, name, length, realarray_x, realarray_y);
}

int cg_iRIC_Write_FunctionalWithName(char* name, char* paramname, int length, double* data)
{
	return cg_iRIC_Write_FunctionalWithName_Mul(lastfileid, name, paramname, length, data);
}

int cg_iRIC_Read_Complex_Count(char* groupname, int* num)
{
	return cg_iRIC_Read_Complex_Count_Mul(lastfileid, groupname, num);
}

int cg_iRIC_Read_Complex_Integer(char* groupname, int num, char* name, int* intvalue)
{
	return cg_iRIC_Read_Complex_Integer_Mul(lastfileid, groupname, num, name, intvalue);
}

int cg_iRIC_Read_Complex_Real(char* groupname, int num, char* name, double* realvalue)
{
	return cg_iRIC_Read_Complex_Real_Mul(lastfileid, groupname, num, name, realvalue);
}

int cg_iRIC_Read_Complex_RealSingle(char* groupname, int num, char* name, float* realvalue)
{
	return cg_iRIC_Read_Complex_RealSingle_Mul(lastfileid, groupname, num, name, realvalue);
}

int cg_iRIC_Read_Complex_StringLen(char* groupname, int num, char* name, int* length)
{
	return cg_iRIC_Read_Complex_StringLen_Mul(lastfileid, groupname, num, name, length);
}

int cg_iRIC_Read_Complex_String(char* groupname, int num, char* name, char* strvalue)
{
	return cg_iRIC_Read_Complex_String_Mul(lastfileid, groupname, num, name, strvalue);
}

int cg_iRIC_Read_Complex_FunctionalSize(char* groupname, int num, char* name, cgsize_t* size)
{
	return cg_iRIC_Read_Complex_FunctionalSize_Mul(lastfileid, groupname, num, name, size);
}

int cg_iRIC_Read_Complex_Functional(char* groupname, int num, char* name, double* x, double* y)
{
	return cg_iRIC_Read_Complex_Functional_Mul(lastfileid, groupname, num, name, x, y);
}

int cg_iRIC_Read_Complex_FunctionalWithName(char* groupname, int num, char* name, char* paramname, double* data)
{
	return cg_iRIC_Read_Complex_FunctionalWithName_Mul(lastfileid, groupname, num, name, paramname, data);
}

int cg_iRIC_Read_Complex_Functional_RealSingle(char* groupname, int num, char* name, float* x, float* y)
{
	return cg_iRIC_Read_Complex_Functional_RealSingle_Mul(lastfileid, groupname, num, name, x, y);
}

int cg_iRIC_Read_Grid_Complex_Node(char* groupname, int* values)
{
	return cg_iRIC_Read_Grid_Complex_Node_Mul(lastfileid, groupname, values);
}

int cg_iRIC_Read_Grid_Complex_Cell(char* groupname, int* values)
{
	return cg_iRIC_Read_Grid_Complex_Cell_Mul(lastfileid, groupname, values);
}

int cg_iRIC_Clear_Complex()
{
	return cg_iRIC_Clear_Complex_Mul(lastfileid);
}

int cg_iRIC_Write_Complex_Integer(char* groupname, int num, char* name, int intvalue)
{
	return cg_iRIC_Write_Complex_Integer_Mul(lastfileid, groupname, num, name, intvalue);
}

int cg_iRIC_Write_Complex_Real(char* groupname, int num, char* name, double realvalue)
{
	return cg_iRIC_Write_Complex_Real_Mul(lastfileid, groupname, num, name, realvalue);
}

int cg_iRIC_Write_Complex_String(char* groupname, int num, char* name, char* strvalue)
{
	return cg_iRIC_Write_Complex_String_Mul(lastfileid, groupname, num, name, strvalue);
}

int cg_iRIC_Write_Complex_Functional(char* groupname, int num, char* name, int length, double* realarray_x, double* realarray_y)
{
	return cg_iRIC_Write_Complex_Functional_Mul(lastfileid, groupname, num, name, length, realarray_x, realarray_y);
}

int cg_iRIC_Write_Complex_FunctionalWithName(char* groupname, int num, char* name, char* paramname, int length, double* data)
{
	return cg_iRIC_Write_Complex_FunctionalWithName_Mul(lastfileid, groupname, num, name, paramname, length, data);
}

int cg_iRIC_Write_Grid_Complex_Node(char* groupname, int* values)
{
	return cg_iRIC_Write_Grid_Complex_Node_Mul(lastfileid, groupname, values);
}

int cg_iRIC_Write_Grid_Complex_Cell(char* groupname, int* values)
{
	return cg_iRIC_Write_Grid_Complex_Cell_Mul(lastfileid, groupname, values);
}

int cg_iRIC_GotoGridCoord2d(cgsize_t* isize, cgsize_t* jsize)
{
	return cg_iRIC_GotoGridCoord2d_Mul(lastfileid, isize, jsize);
}

int cg_iRIC_GetGridCoord2d(double* x, double* y)
{
	return cg_iRIC_GetGridCoord2d_Mul(lastfileid, x, y);
}

int cg_iRIC_GotoGridCoord3d(cgsize_t* isize, cgsize_t* jsize, cgsize_t* ksize)
{
	return cg_iRIC_GotoGridCoord3d_Mul(lastfileid, isize, jsize, ksize);
}

int cg_iRIC_GetGridCoord3d(double* x, double* y, double* z)
{
	return cg_iRIC_GetGridCoord3d_Mul(lastfileid, x, y, z);
}

int cg_iRIC_Read_Grid_Real_Node(char* name, double* values)
{
	return cg_iRIC_Read_Grid_Real_Node_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Grid_Integer_Node(char* name, int* values)
{
	return cg_iRIC_Read_Grid_Integer_Node_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Grid_Real_Cell(char* name, double* values)
{
	return cg_iRIC_Read_Grid_Real_Cell_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Grid_Integer_Cell(char* name, int* values)
{
	return cg_iRIC_Read_Grid_Integer_Cell_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Grid_FunctionalDimensionSize(char* name, char* dimname, cgsize_t* count)
{
	return cg_iRIC_Read_Grid_FunctionalDimensionSize_Mul(lastfileid, name, dimname, count);
}

int cg_iRIC_Read_Grid_FunctionalDimension_Integer(char* name, char* dimname, int* value)
{
	return cg_iRIC_Read_Grid_FunctionalDimension_Integer_Mul(lastfileid, name, dimname, value);
}

int cg_iRIC_Read_Grid_FunctionalDimension_Real(char* name, char* dimname, double* value)
{
	return cg_iRIC_Read_Grid_FunctionalDimension_Real_Mul(lastfileid, name, dimname, value);
}

int cg_iRIC_Read_Grid_FunctionalTimeSize(char* name, cgsize_t* count)
{
	return cg_iRIC_Read_Grid_FunctionalTimeSize_Mul(lastfileid, name, count);
}

int cg_iRIC_Read_Grid_FunctionalTime(char* name, double* time)
{
	return cg_iRIC_Read_Grid_FunctionalTime_Mul(lastfileid, name, time);
}

int cg_iRIC_Read_Grid_Functional_Integer_Node(char* name, int dimid, int* value)
{
	return cg_iRIC_Read_Grid_Functional_Integer_Node_Mul(lastfileid, name, dimid, value);
}

int cg_iRIC_Read_Grid_Functional_Real_Node(char* name, int dimid, double* value)
{
	return cg_iRIC_Read_Grid_Functional_Real_Node_Mul(lastfileid, name, dimid, value);
}

int cg_iRIC_Read_Grid_Functional_Integer_Cell(char* name, int dimid, int* value)
{
	return cg_iRIC_Read_Grid_Functional_Integer_Cell_Mul(lastfileid, name, dimid, value);
}

int cg_iRIC_Read_Grid_Functional_Real_Cell(char* name, int dimid, double* value)
{
	return cg_iRIC_Read_Grid_Functional_Real_Cell_Mul(lastfileid, name, dimid, value);
}

int cg_iRIC_WriteGridCoord1d(cgsize_t isize, double* x)
{
	return cg_iRIC_WriteGridCoord1d_Mul(lastfileid, isize, x);
}

int cg_iRIC_WriteGridCoord2d(cgsize_t isize, cgsize_t jsize, double* x, double* y)
{
	return cg_iRIC_WriteGridCoord2d_Mul(lastfileid, isize, jsize, x, y);
}

int cg_iRIC_WriteGridCoord3d(cgsize_t isize, cgsize_t jsize, cgsize_t ksize, double* x, double* y, double* z)
{
	return cg_iRIC_WriteGridCoord3d_Mul(lastfileid, isize, jsize, ksize, x, y, z);
}

int cg_iRIC_InitGrid(int zoneId)
{
	return cg_iRIC_InitGrid_Mul(lastfileid, zoneId);
}

int cg_iRIC_Write_Grid_Real_Node(char* name, double* values)
{
	return cg_iRIC_Write_Grid_Real_Node_Mul(lastfileid, name, values);
}

int cg_iRIC_Write_Grid_Integer_Node(char* name, int* values)
{
	return cg_iRIC_Write_Grid_Integer_Node_Mul(lastfileid, name, values);
}

int cg_iRIC_Write_Grid_Real_Cell(char* name, double* values)
{
	return cg_iRIC_Write_Grid_Real_Cell_Mul(lastfileid, name, values);
}

int cg_iRIC_Write_Grid_Integer_Cell(char* name, int* values)
{
	return cg_iRIC_Write_Grid_Integer_Cell_Mul(lastfileid, name, values);
}

int cg_iRIC_Read_Sol_Count(int* count)
{
	return cg_iRIC_Read_Sol_Count_Mul(lastfileid, count);
}

int cg_iRIC_Read_Sol_Time(int step, double* time)
{
	return cg_iRIC_Read_Sol_Time_Mul(lastfileid, step, time);
}

int cg_iRIC_Read_Sol_Iteration(int step, int* index)
{
	return cg_iRIC_Read_Sol_Iteration_Mul(lastfileid, step, index);
}

int cg_iRIC_Read_Sol_BaseIterative_Integer(int step, char* name, int* value)
{
	return cg_iRIC_Read_Sol_BaseIterative_Integer_Mul(lastfileid, step, name, value);
}

int cg_iRIC_Read_Sol_BaseIterative_Real(int step, char* name, double* value)
{
	return cg_iRIC_Read_Sol_BaseIterative_Real_Mul(lastfileid, step, name, value);
}

int cg_iRIC_Read_Sol_GridCoord2d(int step, double* x, double* y)
{
	return cg_iRIC_Read_Sol_GridCoord2d_Mul(lastfileid, step, x, y);
}

int cg_iRIC_Read_Sol_GridCoord3d(int step, double* x, double* y, double* z)
{
	return cg_iRIC_Read_Sol_GridCoord3d_Mul(lastfileid, step, x, y, z);
}

int cg_iRIC_Read_Sol_Integer(int step, char* name, int* data)
{
	return cg_iRIC_Read_Sol_Integer_Mul(lastfileid, step, name, data);
}

int cg_iRIC_Read_Sol_Real(int step, char* name, double* data)
{
	return cg_iRIC_Read_Sol_Real_Mul(lastfileid, step, name, data);
}

int cg_iRIC_Write_Sol_Time(double time)
{
	return cg_iRIC_Write_Sol_Time_Mul(lastfileid, time);
}

int cg_iRIC_Write_Sol_Iteration(int index)
{
	return cg_iRIC_Write_Sol_Iteration_Mul(lastfileid, index);
}

int cg_iRIC_Write_Sol_BaseIterative_Integer(char* name, int value)
{
	return cg_iRIC_Write_Sol_BaseIterative_Integer_Mul(lastfileid, name, value);
}

int cg_iRIC_Write_Sol_BaseIterative_Real(char* name, double value)
{
	return cg_iRIC_Write_Sol_BaseIterative_Real_Mul(lastfileid, name, value);
}

int cg_iRIC_Write_Sol_GridCoord2d(double* x, double* y)
{
	return cg_iRIC_Write_Sol_GridCoord2d_Mul(lastfileid, x, y);
}

int cg_iRIC_Write_Sol_GridCoord3d(double* x, double* y, double* z)
{
	return cg_iRIC_Write_Sol_GridCoord3d_Mul(lastfileid, x, y, z);
}

int cg_iRIC_Write_Sol_Integer(char* name, int* data)
{
	return cg_iRIC_Write_Sol_Integer_Mul(lastfileid, name, data);
}

int cg_iRIC_Write_Sol_Real(char* name, double* data)
{
	return cg_iRIC_Write_Sol_Real_Mul(lastfileid, name, data);
}

int cg_iRIC_Write_ErrorCode(int errorcode)
{
	return cg_iRIC_Write_ErrorCode_Mul(lastfileid, errorcode);
}

void cg_iRIC_Init_BC_Names()
{
	cg_iRIC_Init_BC_Names_Mul(lastfileid);
}

void cg_iRIC_Read_BC_Count(char* type, int* num)
{
	cg_iRIC_Read_BC_Count_Mul(lastfileid, type, num);
}

int cg_iRIC_Read_BC_IndicesSize(char* type, int num, cgsize_t* size)
{
	return cg_iRIC_Read_BC_IndicesSize_Mul(lastfileid, type, num, size);
}

int cg_iRIC_Read_BC_Indices(char* type, int num, cgsize_t* indices)
{
	return cg_iRIC_Read_BC_Indices_Mul(lastfileid, type, num, indices);
}

int cg_iRIC_Read_BC_Integer(char* type, int num, char* name, int* intvalue)
{
	return cg_iRIC_Read_BC_Integer_Mul(lastfileid, type, num, name, intvalue);
}

int cg_iRIC_Read_BC_Real(char* type, int num, char* name, double* realvalue)
{
	return cg_iRIC_Read_BC_Real_Mul(lastfileid, type, num, name, realvalue);
}

int cg_iRIC_Read_BC_RealSingle(char* type, int num, char* name, float* realvalue)
{
	return cg_iRIC_Read_BC_RealSingle_Mul(lastfileid, type, num, name, realvalue);
}

int cg_iRIC_Read_BC_StringLen(char* type, int num, char* name, int* length)
{
	return cg_iRIC_Read_BC_StringLen_Mul(lastfileid, type, num, name, length);
}

int cg_iRIC_Read_BC_String(char* type, int num, char* name, char* strvalue)
{
	return cg_iRIC_Read_BC_String_Mul(lastfileid, type, num, name, strvalue);
}

int cg_iRIC_Read_BC_FunctionalSize(char* type, int num, char* name, cgsize_t* size)
{
	return cg_iRIC_Read_BC_FunctionalSize_Mul(lastfileid, type, num, name, size);
}

int cg_iRIC_Read_BC_Functional(char* type, int num, char* name, double* x, double* y)
{
	return cg_iRIC_Read_BC_Functional_Mul(lastfileid, type, num, name, x, y);
}

int cg_iRIC_Read_BC_FunctionalWithName(char* type, int num, char* name, char* paramname, double* data)
{
	return cg_iRIC_Read_BC_FunctionalWithName_Mul(lastfileid, type, num, name, paramname, data);
}

int cg_iRIC_Read_BC_Functional_RealSingle(char* type, int num, char* name, float* x, float* y)
{
	return cg_iRIC_Read_BC_Functional_RealSingle_Mul(lastfileid, type, num, name, x, y);
}

int cg_iRIC_Clear_BC()
{
	return cg_iRIC_Clear_BC_Mul(lastfileid);
}

int cg_iRIC_Write_BC_Indices(char* type, int num, cgsize_t size, cgsize_t* indices)
{
	return cg_iRIC_Write_BC_Indices_Mul(lastfileid, type, num, size, indices);
}

int cg_iRIC_Write_BC_Integer(char* type, int num, char* name, int intvalue)
{
	return cg_iRIC_Write_BC_Integer_Mul(lastfileid, type, num, name, intvalue);
}

int cg_iRIC_Write_BC_Real(char* type, int num, char* name, double realvalue)
{
	return cg_iRIC_Write_BC_Real_Mul(lastfileid, type, num, name, realvalue);
}

int cg_iRIC_Write_BC_String(char* type, int num, char* name, char* strvalue)
{
	return cg_iRIC_Write_BC_String_Mul(lastfileid, type, num, name, strvalue);
}

int cg_iRIC_Write_BC_Functional(char* type, int num, char* name, int length, double* realarray_x, double* realarray_y)
{
	return cg_iRIC_Write_BC_Functional_Mul(lastfileid, type, num, name, length, realarray_x, realarray_y);
}

int cg_iRIC_Write_BC_FunctionalWithName(char* type, int num, char* name, char* paramname, int length, double* data)
{
	return cg_iRIC_Write_BC_FunctionalWithName_Mul(lastfileid, type, num, name, paramname, length, data);
}

int cg_iRIC_Write_Sol_Particle_Pos2d(cgsize_t count, double* x, double* y)
{
	return cg_iRIC_Write_Sol_Particle_Pos2d_Mul(lastfileid, count, x, y);
}

int cg_iRIC_Write_Sol_Particle_Pos3d(cgsize_t count, double* x, double* y, double* z)
{
	return cg_iRIC_Write_Sol_Particle_Pos3d_Mul(lastfileid, count, x, y, z);
}

int cg_iRIC_Write_Sol_Particle_Real(char* name, double* value)
{
	return cg_iRIC_Write_Sol_Particle_Real_Mul(lastfileid, name, value);
}

int cg_iRIC_Write_Sol_Particle_Integer(char* name, int* value)
{
	return cg_iRIC_Write_Sol_Particle_Integer_Mul(lastfileid, name, value);
}

int cg_iRIC_Read_Geo_Count(char* name, int* count)
{
	return cg_iRIC_Read_Geo_Count_Mul(lastfileid, name, count);
}

int cg_iRIC_Read_Geo_Filename(char* name, int geoid, char* filename, int* type)
{
	return cg_iRIC_Read_Geo_Filename_Mul(lastfileid, name, geoid, filename, type);
}
