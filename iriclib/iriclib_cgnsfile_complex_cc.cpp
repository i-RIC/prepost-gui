#include "error_macros.h"
#include "iriclib_cgnsfile.h"
#include "private/iriclib_cgnsfile_impl.h"

using namespace iRICLib;

int CgnsFile::Complex_CC_Read_Count(const char *groupname, int* count)
{
	int ier = impl->gotoComplexGroup(groupname);
	RETURN_IF_ERR;
	return cg_nuser_data(count);
}

int CgnsFile::Complex_CC_Read_Integer(const char *groupname, int num, const char *name, int* intvalue)
{
	int ier = impl->gotoComplexChild(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::readArray("Value", Integer, 1, intvalue);
}

int CgnsFile::Complex_CC_Read_Real(const char *groupname, int num, const char *name, double* realvalue)
{
	int ier = impl->gotoComplexChild(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::readArray("Value", RealDouble, 1, realvalue);
}

int CgnsFile::Complex_CC_Read_RealSingle(const char *groupname, int num, const char *name, float* realvalue)
{
	int ier = impl->gotoComplexChild(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::readArray("Value", RealSingle, 1, realvalue);
}

int CgnsFile::Complex_CC_Read_StringLen(const char *groupname, int num, const char *name, int* length)
{
	int ier = impl->gotoComplexChild(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::readStringLen("Value", length);
}

int CgnsFile::Complex_CC_Read_String(const char *groupname, int num, const char *name, char* strvalue)
{
	int ier = impl->gotoComplexChild(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::readString("Value", -1, strvalue);
}

int CgnsFile::Complex_CC_Read_FunctionalSize(const char *groupname, int num, const char *name, cgsize_t* size)
{
	int ier = impl->gotoComplexChild(groupname, num, name);
	RETURN_IF_ERR;

	int narrays;
	ier = cg_narrays(&narrays);
	RETURN_IF_ERR;

	for (int i = 1; i <= narrays; ++i) {
		char arrayname[Impl::NAME_MAXLENGTH];
		DataType_t datatype;
		int dim;
		cgsize_t dimvec;
		ier = cg_array_info(i, arrayname, &datatype, &dim, &dimvec);
		*size = dimvec;
		return 0;
	}
	return 1;
}

int CgnsFile::Complex_CC_Read_Functional(const char *groupname, int num, const char *name, double* x, double* y)
{
	int ier;
	ier = Complex_CC_Read_FunctionalWithName(groupname, num, name, "Param", x);
	RETURN_IF_ERR;
	ier = Complex_CC_Read_FunctionalWithName(groupname, num, name, "Value", y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Complex_CC_Read_FunctionalWithName(const char *groupname, int num, const char *name, const char *paramname, double *data)
{
	int ier = impl->gotoComplexChild(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::readArray(paramname, RealDouble, -1, data);
}

int CgnsFile::Complex_CC_Read_Functional_RealSingle(const char *groupname, int num, const char *name, float* x, float* y)
{
	int ier = impl->gotoComplexChild(groupname, num, name);
	RETURN_IF_ERR;

	ier = Impl::readArrayAs("Param", RealSingle, -1, x);
	RETURN_IF_ERR;
	ier = Impl::readArrayAs("Value", RealSingle, -1, y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Complex_CC_Read_Grid_Node(const char *groupname, int* values)
{
	return Grid_Read_Integer_Node(groupname, values);
}

int CgnsFile::Complex_CC_Read_Grid_Cell(const char *groupname, int* values)
{
	return Complex_CC_Read_Grid_Node(groupname, values);
}

int CgnsFile::Complex_CC_Write_Integer(const char *groupname, int num, const char *name, int intvalue)
{
	int ier = impl->gotoComplexChildCreateIfNotExist(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::writeArray("Value", Integer, 1, &intvalue);
}

int CgnsFile::Complex_CC_Write_Real(const char *groupname, int num, const char *name, double realvalue)
{
	int ier = impl->gotoComplexChildCreateIfNotExist(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::writeArray("Value", RealDouble, 1, &realvalue);
}

int CgnsFile::Complex_CC_Write_String(const char *groupname, int num, const char *name, char* strvalue)
{
	int ier = impl->gotoComplexChildCreateIfNotExist(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::writeString("Value", strvalue);
}

int CgnsFile::Complex_CC_Write_Functional(const char *groupname, int num, const char *name, int length, double* realarray_x, double* realarray_y)
{
	int ier;
	ier = Complex_CC_Write_FunctionalWithName(groupname, num, name, "Param", length, realarray_x);
	RETURN_IF_ERR;
	ier = Complex_CC_Write_FunctionalWithName(groupname, num, name, "Value", length, realarray_y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Complex_CC_Write_FunctionalWithName(const char *groupname, int num, const char *name, const char* paramname, int length, double* data)
{
	int ier = impl->gotoComplexChildCreateIfNotExist(groupname, num, name);
	RETURN_IF_ERR;
	return Impl::writeArray(paramname, RealDouble, length, data);
}

int CgnsFile::Complex_CC_Write_Grid_Node(const char *groupname, int* values)
{
	return Grid_Write_Integer_Node(groupname, values);
}

int CgnsFile::Complex_CC_Write_Grid_Cell(const char *groupname, int* values)
{
	return Grid_Write_Integer_Cell(groupname, values);
}
