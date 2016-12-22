#include "error_macros.h"
#include "iriclib_cgnsfile.h"
#include "private/iriclib_cgnsfile_impl.h"

using namespace iRICLib;

int CgnsFile::Geo_Count(const char* name, int* count)
{
	int ier = cg_goto(impl->m_fileId, impl->m_baseId, Impl::RDNODE.c_str(), 0, name, 0, NULL);
	RETURN_IF_ERR;
	return cg_nuser_data(count);
}

int CgnsFile::Geo_Filename(const char* name, int geoid, char* filename, int* type)
{
	int ier = cg_goto(impl->m_fileId, impl->m_baseId, Impl::RDNODE.c_str(), 0, name, 0, "UserDefinedData_t", geoid, NULL);
	RETURN_IF_ERR;
	ier = Impl::readString("filename", -1, filename);
	RETURN_IF_ERR;
	ier = Impl::readArray("type", Integer, 1, type);
	RETURN_IF_ERR;
	return 0;
}
