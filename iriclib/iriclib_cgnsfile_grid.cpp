#include "error_macros.h"
#include "iriclib_cgnsfile.h"
#include "private/iriclib_cgnsfile_impl.h"

#include <string>

using namespace iRICLib;

namespace {

static const std::string iRICBASE1D = "iRIC1D";
static const std::string iRICBASE2D = "iRIC2D";
static const std::string iRICBASE3D = "iRIC3D";

} // namespace

int CgnsFile::Grid_GotoCoord2d(cgsize_t* isize, cgsize_t* jsize)
{
	char zoneName[Impl::NAME_MAXLENGTH];
	ZoneType_t zonetype;

	int ier = cg_zone_read(impl->m_fileId, impl->m_baseId, impl->m_zoneId, zoneName, impl->m_zoneSize);
	RETURN_IF_ERR;
	cg_zone_type(impl->m_fileId, impl->m_baseId, impl->m_zoneId, &zonetype);

	if (zonetype != Structured) {return 1;}

	*isize = impl->m_zoneSize[0];
	*jsize = impl->m_zoneSize[1];

	return impl->gotoGridCondition();
}

int CgnsFile::Grid_GetCoord2d(double *x, double *y)
{
	int ncoords;
	cgsize_t rangemin[3], rangemax[3];

	int ier = cg_ncoords(impl->m_fileId, impl->m_baseId, impl->m_zoneId, &ncoords);
	RETURN_IF_ERR;
	for (int i = 0; i < 3; ++i) {rangemin[i] = 1;}
	for (int i = 0; i < 2; ++i) {rangemax[i] = impl->m_zoneSize[i];}
	rangemax[2] = 1;

	// read x
	ier = cg_coord_read(impl->m_fileId, impl->m_baseId, impl->m_zoneId, "CoordinateX", RealDouble, rangemin, rangemax, x);
	RETURN_IF_ERR;
	// read y
	ier = cg_coord_read(impl->m_fileId, impl->m_baseId, impl->m_zoneId, "CoordinateY", RealDouble, rangemin, rangemax, y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Grid_GotoCoord3d(cgsize_t* isize, cgsize_t* jsize, cgsize_t* ksize)
{
	char zoneName[Impl::NAME_MAXLENGTH];
	ZoneType_t zonetype;

	int ier = cg_zone_read(impl->m_fileId, impl->m_baseId, impl->m_zoneId, zoneName, impl->m_zoneSize);
	RETURN_IF_ERR;
	cg_zone_type(impl->m_fileId, impl->m_baseId, impl->m_zoneId, &zonetype);

	if (zonetype != Structured) {return 1;}

	*isize = impl->m_zoneSize[0];
	*jsize = impl->m_zoneSize[1];
	*ksize = impl->m_zoneSize[2];

	return impl->gotoGridCondition();
}

int CgnsFile::Grid_GetCoord3d(double *x, double *y, double *z)
{
	int ncoords;
	cgsize_t rangemin[3], rangemax[3];

	int ier = cg_ncoords(impl->m_fileId, impl->m_baseId, impl->m_zoneId, &ncoords);
	RETURN_IF_ERR;
	for (int i = 0; i < 3; ++i) {rangemin[i] = 1;}
	for (int i = 0; i < 2; ++i) {rangemax[i] = impl->m_zoneSize[i];}
	rangemax[2] = 1;

	// read x
	ier = cg_coord_read(impl->m_fileId, impl->m_baseId, impl->m_zoneId, "CoordinateX", RealDouble, rangemin, rangemax, x);
	RETURN_IF_ERR;
	// read y
	ier = cg_coord_read(impl->m_fileId, impl->m_baseId, impl->m_zoneId, "CoordinateY", RealDouble, rangemin, rangemax, y);
	RETURN_IF_ERR;
	// read z
	ier = cg_coord_read(impl->m_fileId, impl->m_baseId, impl->m_zoneId, "CoordinateZ", RealDouble, rangemin, rangemax, z);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Grid_Read_Real_Node(const char* name, double* values)
{
	int ier = impl->gotoGridConditionChild(name);
	RETURN_IF_ERR;
	return Impl::readArrayAs("Value", RealDouble, -1, values);
}

int CgnsFile::Grid_Read_Integer_Node(const char* name, int* values)
{
	int ier = impl->gotoGridConditionChild(name);
	RETURN_IF_ERR;
	return Impl::readArrayAs("Value", Integer, -1, values);
}

int CgnsFile::Grid_Read_Real_Cell(const char *name, double* values)
{
	return Grid_Read_Real_Node(name, values);
}

int CgnsFile::Grid_Read_Integer_Cell(const char *name, int* values)
{
	return Grid_Read_Integer_Node(name, values);
}

int CgnsFile::Grid_Read_FunctionalDimensionSize(const char* name, const char* dimname, cgsize_t* count)
{
	int ier = impl->gotoGridConditionChild(name);
	RETURN_IF_ERR;

	char dimArrayName[Impl::NAME_MAXLENGTH];
	Impl::getDimensionArrayName(dimname, dimArrayName);

	int index;
	DataType_t datatype;
	int dim;
	cgsize_t dimvec[3];
	ier = Impl::findArray(dimArrayName, &index, &datatype, &dim, &(dimvec[0]));
	RETURN_IF_ERR;

	*count = dimvec[0];
	return 0;
}

int CgnsFile::Grid_Read_FunctionalDimension_Integer(const char *name, const char *dimname, int* value)
{
	int ier = impl->gotoGridConditionChild(name);
	RETURN_IF_ERR;

	char dimArrayName[Impl::NAME_MAXLENGTH];
	Impl::getDimensionArrayName(dimname, dimArrayName);

	return Impl::readArrayAs(dimArrayName, Integer, -1, value);
}

int CgnsFile::Grid_Read_FunctionalDimension_Real(const char *name, const char *dimname, double* value)
{
	int ier = impl->gotoGridConditionChild(name);
	RETURN_IF_ERR;

	char dimArrayName[Impl::NAME_MAXLENGTH];
	Impl::getDimensionArrayName(dimname, dimArrayName);

	return Impl::readArrayAs(dimArrayName, RealDouble, -1, value);
}

int CgnsFile::Grid_Read_FunctionalTimeSize(const char *name, cgsize_t* count)
{
	return Grid_Read_FunctionalDimensionSize(name, "Time", count);
}

int CgnsFile::Grid_Read_FunctionalTime(const char *name, double* time)
{
	return Grid_Read_FunctionalDimension_Real(name, "Time", time);
}

int CgnsFile::Grid_Read_Functional_Integer_Node(const char *name, int dimid, int* value)
{
	int ier = impl->gotoGridConditionChild(name);
	RETURN_IF_ERR;

	char dimArrayName[Impl::NAME_MAXLENGTH];
	Impl::getFunctionalDataName(dimid, dimArrayName);

	return Impl::readArrayAs(dimArrayName, Integer, -1, value);
}

int CgnsFile::Grid_Read_Functional_Real_Node(const char *name, int dimid, double* value)
{
	int ier = impl->gotoGridConditionChild(name);
	RETURN_IF_ERR;

	char dimArrayName[Impl::NAME_MAXLENGTH];
	Impl::getFunctionalDataName(dimid, dimArrayName);

	return Impl::readArrayAs(dimArrayName, RealDouble, -1, value);
}

int CgnsFile::Grid_Read_Functional_Integer_Cell(const char *name, int dimid, int* value)
{
	return Grid_Read_Functional_Integer_Node(name, dimid, value);
}

int CgnsFile::Grid_Read_Functional_Real_Cell(const char *name, int dimid, double* value)
{
	return Grid_Read_Functional_Real_Node(name, dimid, value);
}

int CgnsFile::Grid_Init(int zoneId)
{
	impl->m_zoneId = zoneId;
	int ier = impl->addGridConditionNodeIfNotExist();
	RETURN_IF_ERR;
	ier = impl->addComplexNodeIfNotExist();
	RETURN_IF_ERR;
	ier = impl->clearBaseIter();
	RETURN_IF_ERR;
	ier = impl->clearZoneIter();
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Grid_WriteGridCoord1d(cgsize_t isize, double* x)
{
	if (impl->m_baseCellDim != 1) {
		// create new 1D base!
		impl->m_baseCellDim = 1;
		impl->m_basePhysDim = 1;
		int ier = cg_base_write(impl->m_fileId, iRICBASE1D.c_str(), impl->m_baseCellDim, impl->m_basePhysDim, &(impl->m_baseId));
		RETURN_IF_ERR;
	}
	impl->m_zoneSize[0] = isize;
	impl->m_zoneSize[1] = isize - 1;
	int ier = cg_zone_write(impl->m_fileId, impl->m_baseId, Impl::IRICZONE.c_str(), impl->m_zoneSize, Structured, &(impl->m_zoneId));
	RETURN_IF_ERR;
	int G, C;
	ier = cg_grid_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, "GridCoordinates", &G);
	RETURN_IF_ERR;
	ier = cg_coord_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, RealDouble, "CoordinateX", x, &C);
	RETURN_IF_ERR;
	ier = impl->clearSolution();
	RETURN_IF_ERR;

	ier = impl->addGridConditionNodeIfNotExist();
	RETURN_IF_ERR;
	ier = impl->addComplexNodeIfNotExist();
	RETURN_IF_ERR;
	ier = impl->clearBaseIter();
	RETURN_IF_ERR;
	ier = impl->clearZoneIter();
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Grid_WriteGridCoord2d(cgsize_t isize, cgsize_t jsize, double* x, double* y)
{
	if (impl->m_baseCellDim != 2) {
		// create new 2D base!
		impl->m_baseCellDim = 2;
		impl->m_basePhysDim = 2;
		int ier = cg_base_write(impl->m_fileId, iRICBASE2D.c_str(), impl->m_baseCellDim, impl->m_basePhysDim, &(impl->m_baseId));
		RETURN_IF_ERR;
	}
	impl->m_zoneSize[0] = isize;
	impl->m_zoneSize[1] = jsize;
	impl->m_zoneSize[2] = isize - 1;
	impl->m_zoneSize[3] = jsize - 1;
	int ier = cg_zone_write(impl->m_fileId, impl->m_baseId, Impl::IRICZONE.c_str(), impl->m_zoneSize, Structured, &(impl->m_zoneId));
	RETURN_IF_ERR;
	int G, C;
	ier = cg_grid_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, "GridCoordinates", &G);
	RETURN_IF_ERR;
	ier = cg_coord_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, RealDouble, "CoordinateX", x, &C);
	RETURN_IF_ERR;
	ier = cg_coord_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, RealDouble, "CoordinateY", y, &C);
	RETURN_IF_ERR;
	ier = impl->clearSolution();
	RETURN_IF_ERR;

	ier = impl->addGridConditionNodeIfNotExist();
	RETURN_IF_ERR;
	ier = impl->addComplexNodeIfNotExist();
	RETURN_IF_ERR;
	ier = impl->clearBaseIter();
	RETURN_IF_ERR;
	ier = impl->clearZoneIter();
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Grid_WriteGridCoord3d(cgsize_t isize, cgsize_t jsize, cgsize_t ksize, double* x, double* y, double* z)
{
	if (impl->m_baseCellDim != 3) {
		// create new 3D base!
		impl->m_baseCellDim = 3;
		impl->m_basePhysDim = 3;
		int ier = cg_base_write(impl->m_fileId, iRICBASE3D.c_str(), impl->m_baseCellDim, impl->m_basePhysDim, &(impl->m_baseId));
		RETURN_IF_ERR;
	}
	impl->m_zoneSize[0] = isize;
	impl->m_zoneSize[1] = jsize;
	impl->m_zoneSize[2] = ksize;
	impl->m_zoneSize[3] = isize - 1;
	impl->m_zoneSize[4] = jsize - 1;
	impl->m_zoneSize[5] = ksize - 1;
	int ier = cg_zone_write(impl->m_fileId, impl->m_baseId, Impl::IRICZONE.c_str(), impl->m_zoneSize, Structured, &(impl->m_zoneId));
	RETURN_IF_ERR;
	int G, C;
	ier = cg_grid_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, "GridCoordinates", &G);
	RETURN_IF_ERR;
	ier = cg_coord_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, RealDouble, "CoordinateX", x, &C);
	RETURN_IF_ERR;
	ier = cg_coord_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, RealDouble, "CoordinateY", y, &C);
	RETURN_IF_ERR;
	ier = cg_coord_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, RealDouble, "CoordinateZ", z, &C);
	RETURN_IF_ERR;
	ier = impl->clearSolution();
	RETURN_IF_ERR;

	ier = impl->addGridConditionNodeIfNotExist();
	RETURN_IF_ERR;
	ier = impl->addComplexNodeIfNotExist();
	RETURN_IF_ERR;
	ier = impl->clearBaseIter();
	RETURN_IF_ERR;
	ier = impl->clearZoneIter();
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Grid_Write_Real_Node(const char* name, double* values)
{
	cgsize_t dim = impl->gridNodeValueCount();

	int ier = impl->gotoGridConditionNewChild(name);
	RETURN_IF_ERR;
	return cg_array_write("Value", RealDouble, 1, &dim, values);
}

int CgnsFile::Grid_Write_Integer_Node(const char* name, int* values)
{
	cgsize_t dim = impl->gridNodeValueCount();

	int ier = impl->gotoGridConditionNewChild(name);
	RETURN_IF_ERR;
	return cg_array_write("Value", Integer, 1, &dim, values);
}

int CgnsFile::Grid_Write_Real_Cell(const char *name, double* values)
{
	cgsize_t dim = impl->gridCellValueCount();

	int ier = impl->gotoGridConditionNewChild(name);
	RETURN_IF_ERR;
	return cg_array_write("Value", RealDouble, 1, &dim, values);
}

int CgnsFile::Grid_Write_Integer_Cell(const char *name, int* values)
{
	cgsize_t dim = impl->gridCellValueCount();

	int ier = impl->gotoGridConditionNewChild(name);
	RETURN_IF_ERR;
	return cg_array_write("Value", Integer, 1, &dim, values);
}
