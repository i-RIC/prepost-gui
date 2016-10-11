#include "error_macros.h"
#include "iriclib_cgnsfile.h"
#include "private/iriclib_cgnsfile_impl.h"

using namespace iRICLib;

void CgnsFile::BC_Init_Names()
{
	impl->loadBcNames();
}

void CgnsFile::BC_Read_Count(const char *typeName, int* num)
{
	int count = 0;
	while (1) {
		int BC;
		impl->getBcIndex(typeName, count + 1, &BC);
		if (BC == 0) {
			*num = count;
			return;
		}
		++ count;
	}
}

int CgnsFile::BC_Read_IndicesSize(const char *typeName, int num, cgsize_t* size)
{
	int BC;
	impl->getBcIndex(typeName, num, &BC);
	if (BC == 0) {
		return 1;
	}

	char bcname[Impl::NAME_MAXLENGTH];
	BCType_t bctype;
	PointSetType_t pstype;
	cgsize_t npnts;
	int nindex[3];
	cgsize_t nlistflag;
	DataType_t ndtype;
	int ndataset;

	int ier = cg_boco_info(impl->m_fileId, impl->m_baseId, impl->m_zoneId, BC, bcname, &bctype,
												 &pstype, &npnts, nindex, &nlistflag, &ndtype, &ndataset);
	RETURN_IF_ERR;
	*size = npnts;
	return 0;
}

int CgnsFile::BC_Read_Indices(const char *typeName, int num, cgsize_t* indices)
{
	int BC;
	impl->getBcIndex(typeName, num, &BC);
	if (BC == 0) {
		return 1;
	}
	int normallist;
	return cg_boco_read(impl->m_fileId, impl->m_baseId, impl->m_zoneId, BC, indices, &normallist);
}

int CgnsFile::BC_Read_Integer(const char *typeName, int num, const char *name, int* intvalue)
{
	int ier = impl->gotoBcChild(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::readArray("Value", Integer, 1, intvalue);
}

int CgnsFile::BC_Read_Real(const char *typeName, int num, const char *name, double* realvalue)
{
	int ier = impl->gotoBcChild(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::readArray("Value", RealDouble, 1, realvalue);
}

int CgnsFile::BC_Read_RealSingle(const char *typeName, int num, const char *name, float* realvalue)
{
	int ier = impl->gotoBcChild(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::readArrayAs("Value", RealSingle, 1, realvalue);
}

int CgnsFile::BC_Read_StringLen(const char *typeName, int num, const char *name, int* length)
{
	int ier = impl->gotoBcChild(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::readStringLen("Value", length);
}

int CgnsFile::BC_Read_String(const char *typeName, int num, const char *name, char* strvalue)
{
	int ier = impl->gotoBcChild(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::readString("Value", -1, strvalue);
}

int CgnsFile::BC_Read_FunctionalSize(const char *typeName, int num, const char *name, cgsize_t* size)
{
	int ier = impl->gotoBcChild(typeName, num, name);
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

int CgnsFile::BC_Read_Functional(const char *typeName, int num, const char *name, double* x, double* y)
{
	int ier;
	ier = BC_Read_FunctionalWithName(typeName, num, name, "Param", x);
	RETURN_IF_ERR;
	ier = BC_Read_FunctionalWithName(typeName, num, name, "Value", y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::BC_Read_FunctionalWithName(const char *typeName, int num, const char *name, char* paramname, double* data)
{
	int ier = impl->gotoBcChild(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::readArray(paramname, RealDouble, -1, data);
}

int CgnsFile::BC_Read_Functional_RealSingle(const char *typeName, int num, const char *name, float* x, float* y)
{
	int ier = impl->gotoBcChild(typeName, num, name);
	RETURN_IF_ERR;

	ier = Impl::readArrayAs("Param", RealSingle, -1, x);
	RETURN_IF_ERR;
	ier = Impl::readArrayAs("Value", RealSingle, -1, y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::BC_Clear()
{
	int ier = impl->gotoZone();
	RETURN_IF_ERR;

	cg_delete_node("ZoneBC");
	impl->m_bcNames.clear();
	return 0;
}

int CgnsFile::BC_Write_Indices(const char *typeName, int num, cgsize_t size, cgsize_t* indices)
{
	char tmpname[Impl::NAME_MAXLENGTH];
	Impl::getBcName(typeName, num, tmpname);

	int BC;
	int ier = cg_boco_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, tmpname, BCTypeUserDefined, PointList, size, indices, &BC);
	RETURN_IF_ERR;
	impl->m_bcNames.push_back(std::string(tmpname));
	return 0;
}

int CgnsFile::BC_Write_Integer(const char *typeName, int num, const char *name, int intvalue)
{
	int ier = impl->gotoBcChildCreateIfNotExist(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::writeArray("Value", Integer, 1, &intvalue);
}

int CgnsFile::BC_Write_Real(const char *typeName, int num, const char *name, double realvalue)
{
	int ier = impl->gotoBcChildCreateIfNotExist(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::writeArray("Value", RealDouble, 1, &realvalue);
}

int CgnsFile::BC_Write_String(const char *typeName, int num, const char *name, char* strvalue)
{
	int ier = impl->gotoBcChildCreateIfNotExist(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::writeString("Value", strvalue);
}

int CgnsFile::BC_Write_Functional(const char *typeName, int num, const char *name, int length, double* realarray_x, double* realarray_y)
{
	int ier;
	ier = BC_Write_FunctionalWithName(typeName, num, name, "Param", length, realarray_x);
	RETURN_IF_ERR;
	ier = BC_Write_FunctionalWithName(typeName, num, name, "Value", length, realarray_y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::BC_Write_FunctionalWithName(const char *typeName, int num, const char *name, char* paramname, int length, double* data)
{
	int ier = impl->gotoBcChildCreateIfNotExist(typeName, num, name);
	RETURN_IF_ERR;
	return Impl::writeArray(paramname, RealDouble, length, data);
}
