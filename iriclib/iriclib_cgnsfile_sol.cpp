#include "error_macros.h"
#include "iriclib_cgnsfile.h"
#include "private/iriclib_cgnsfile_impl.h"
#include "private/iriclib_cgnsfile_baseiterativet.h"
#include "private/iriclib_cgnsfile_solutionwriter.h"

using namespace iRICLib;

namespace {

static const std::string ECNODE = "ErrorCode";

} // namespace

int CgnsFile::Sol_Read_Count(int* count)
{
	*count = impl->m_solId;
	return 0;
}

int CgnsFile::Sol_Read_Time(int step, double* time)
{
	if (step > impl->m_solId) {
		return 1;
	}
	*time = impl->m_solTimes.at(step - 1);
	return 0;
}

int CgnsFile::Sol_Read_Iteration(int step, int* index)
{
	if (step > impl->m_solId) {
		return 1;
	}
	*index = impl->m_solIndices.at(step - 1);
	return 0;
}

int CgnsFile::Sol_Read_BaseIterative_Integer(int step, const char *name, int* value)
{
	if (step > impl->m_solId) {
		return 1;
	}
	for (BaseIterativeT<int> bit : impl->m_solBaseIterInts) {
		if (bit.name() == name) {
			*value = bit.values().at(step - 1);
			return 0;
		}
	}
	return 2;
}

int CgnsFile::Sol_Read_BaseIterative_Real(int step, const char *name, double* value)
{
	if (step > impl->m_solId) {
		return 1;
	}
	for (BaseIterativeT<double> bit : impl->m_solBaseIterReals) {
		if (bit.name() == name) {
			*value = bit.values().at(step - 1);
			return 0;
		}
	}
	return 2;
}

int CgnsFile::Sol_Read_GridCoord2d(int step, double* x, double* y)
{
	int ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId,
										"GridCoordinates_t", step + 1, NULL);
	RETURN_IF_ERR;
	ier = Impl::readArray("CoordinateX", RealDouble, -1, x);
	RETURN_IF_ERR;
	ier = Impl::readArray("CoordinateY", RealDouble, -1, y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Sol_Read_GridCoord3d(int step, double* x, double* y, double* z)
{
	int ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId,
										"GridCoordinates_t", step + 1, NULL);
	RETURN_IF_ERR;
	ier = Impl::readArray("CoordinateX", RealDouble, -1, x);
	RETURN_IF_ERR;
	ier = Impl::readArray("CoordinateY", RealDouble, -1, y);
	RETURN_IF_ERR;
	ier = Impl::readArray("CoordinateZ", RealDouble, -1, z);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Sol_Read_Integer(int step, const char *name, int* data)
{
	int ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId,
										"FlowSolution_t", step, NULL);
	RETURN_IF_ERR;
	return Impl::readArray(name, Integer, -1, data);
}

int CgnsFile::Sol_Read_Real(int step, const char *name, double* data)
{
	int ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId,
										"FlowSolution_t", step, NULL);
	RETURN_IF_ERR;
	return Impl::readArray(name, RealDouble, -1, data);
}

int CgnsFile::Sol_Write_Time(double time)
{
	return impl->m_solutionWriter->Sol_Write_Time(time);
/*
	impl->m_solTimes.push_back(time);
	++ impl->m_solId;

	// write to default base (with calculation condition)
	int ier = cg_biter_write(impl->m_fileId, impl->m_ccBaseId, Impl::BINAME.c_str(), impl->m_solId);
	RETURN_IF_ERR;
	ier = impl->gotoCCBaseIter();
	RETURN_IF_ERR
	cgsize_t dimVec = impl->m_solId;
	ier = cg_array_write("TimeValues", RealDouble, 1, &dimVec, impl->m_solTimes.data());
	RETURN_IF_ERR;

	// write to current base
	ier = cg_biter_write(impl->m_fileId, impl->m_baseId, Impl::BINAME.c_str(), impl->m_solId);
	RETURN_IF_ERR;
	ier = impl->gotoBaseIter();
	RETURN_IF_ERR
	ier = cg_array_write("TimeValues", RealDouble, 1, &dimVec, impl->m_solTimes.data());
	RETURN_IF_ERR;

	// add solution node
	return impl->addSolutionNode();
*/
}

int CgnsFile::Sol_Write_Iteration(int index)
{
	return impl->m_solutionWriter->Sol_Write_Iteration(index);
/*
	impl->m_solIndices.push_back(index);
	++ impl->m_solId;

	// write to default base (with calculation condition)
	int ier = cg_biter_write(impl->m_fileId, impl->m_ccBaseId, Impl::BINAME.c_str(), impl->m_solId);
	RETURN_IF_ERR;
	ier = impl->gotoCCBaseIter();
	RETURN_IF_ERR
	cgsize_t dimVec = impl->m_solId;
	ier = cg_array_write("IterationValues", Integer, 1, &dimVec, impl->m_solIndices.data());
	RETURN_IF_ERR;

	// write to current base
	ier = cg_biter_write(impl->m_fileId, impl->m_baseId, Impl::BINAME.c_str(), impl->m_solId);
	RETURN_IF_ERR;
	ier = impl->gotoBaseIter();
	RETURN_IF_ERR
	ier = cg_array_write("IterationValues", Integer, 1, &dimVec, impl->m_solIndices.data());
	RETURN_IF_ERR;

	// add solution node
	return impl->addSolutionNode();
*/
}

int CgnsFile::Sol_Write_BaseIterative_Integer(const char *name, int value)
{
	bool found = false;
	BaseIterativeT<int> data = name;
	for (BaseIterativeT<int> bit : impl->m_solBaseIterInts) {
		if (bit.name() == name) {
			bit.addValue(value);
			data = bit;
			found = true;
		}
	}
	if (! found) {
		BaseIterativeT<int> newData = name;
		newData.addValue(value);
		impl->m_solBaseIterInts.push_back(newData);
		data = newData;
	}
	// write the value.
	cgsize_t dimVec = static_cast<cgsize_t> (data.values().size());
	impl->gotoBaseIter();
	return cg_array_write(data.name().c_str(), Integer, 1, &dimVec, data.values().data());
}

int CgnsFile::Sol_Write_BaseIterative_Real(const char *name, double value)
{
	bool found = false;
	BaseIterativeT<double> data = name;
	for (BaseIterativeT<double> bit : impl->m_solBaseIterReals) {
		if (bit.name() == name) {
			bit.addValue(value);
			data = bit;
			found = true;
		}
	}
	if (! found) {
		BaseIterativeT<double> newData = name;
		newData.addValue(value);
		impl->m_solBaseIterReals.push_back(newData);
		data = newData;
	}
	// write the value.
	cgsize_t dimVec = static_cast<cgsize_t> (data.values().size());
	impl->gotoBaseIter();
	return cg_array_write(data.name().c_str(), RealDouble, 1, &dimVec, data.values().data());
}

int CgnsFile::Sol_Write_GridCoord2d(double *x, double *y)
{
	return impl->m_solutionWriter->Sol_Write_GridCoord2d(x, y);
/*
	int ier = impl->addSolutionGridCoordNode();
	RETURN_IF_ERR;

	ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId, "GridCoordinates_t", impl->m_solId + 1, NULL);

	cgsize_t dimVec[2];
	dimVec[0] = impl->m_zoneSize[0];
	dimVec[1] = impl->m_zoneSize[1];

	ier = cg_array_write("CoordinateX", RealDouble, 2, dimVec, x);
	RETURN_IF_ERR;
	ier = cg_array_write("CoordinateY", RealDouble, 2, dimVec, y);
	RETURN_IF_ERR;
	return 0;
*/
}

int CgnsFile::Sol_Write_GridCoord3d(double *x, double *y, double *z)
{
	return impl->m_solutionWriter->Sol_Write_GridCoord3d(x, y, z);
/*
	int ier = impl->addSolutionGridCoordNode();
	RETURN_IF_ERR;

	ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId, "GridCoordinates_t", impl->m_solId + 1, NULL);

	cgsize_t dimVec[3];
	dimVec[0] = impl->m_zoneSize[0];
	dimVec[1] = impl->m_zoneSize[1];
	dimVec[2] = impl->m_zoneSize[2];

	ier = cg_array_write("CoordinateX", RealDouble, 3, dimVec, x);
	RETURN_IF_ERR;
	ier = cg_array_write("CoordinateY", RealDouble, 3, dimVec, y);
	RETURN_IF_ERR;
	ier = cg_array_write("CoordinateZ", RealDouble, 3, dimVec, z);
	RETURN_IF_ERR;
	return 0;
*/
}

int CgnsFile::Sol_Write_Integer(const char *name, int* data)
{
	return impl->m_solutionWriter->Sol_Write_Integer(name, data);
/*
	int F;
	return cg_field_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, impl->m_solId, Integer, name, data, &F);
*/
}

int CgnsFile::Sol_Write_Real(const char *name, double* data)
{
	return impl->m_solutionWriter->Sol_Write_Real(name, data);
/*
	int F;
	return cg_field_write(impl->m_fileId, impl->m_baseId, impl->m_zoneId, impl->m_solId, RealDouble, name, data, &F);
*/
}

int CgnsFile::ErrorCode_Write(int errorcode)
{
	int ier = impl->gotoBase();
	RETURN_IF_ERR;

	cg_delete_node(ECNODE.c_str());
	ier = cg_user_data_write(ECNODE.c_str());
	RETURN_IF_ERR;
	ier = cg_goto(impl->m_fileId, impl->m_baseId, ECNODE.c_str(), 0, NULL);
	RETURN_IF_ERR;
	return Impl::writeArray("Value", Integer, 1, &errorcode);
}

int CgnsFile::Sol_Particle_Write_Pos2d(cgsize_t count, double* x, double* y)
{
	int ier = impl->addParticleSolutionNode();
	RETURN_IF_ERR;

	char particleNodeName[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(impl->m_solId, particleNodeName);

	ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId, particleNodeName, 0, NULL);

	ier = Impl::writeArray("CoordinateX", RealDouble, static_cast<size_t>(count), x);
	RETURN_IF_ERR;
	ier = Impl::writeArray("CoordinateY", RealDouble, static_cast<size_t>(count), y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Sol_Particle_Write_Pos3d(cgsize_t count, double* x, double* y, double* z)
{
	int ier = impl->addParticleSolutionNode();
	RETURN_IF_ERR;

	char particleNodeName[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(impl->m_solId, particleNodeName);

	ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId, particleNodeName, 0, NULL);

	ier = Impl::writeArray("CoordinateX", RealDouble, static_cast<size_t>(count), x);
	RETURN_IF_ERR;
	ier = Impl::writeArray("CoordinateY", RealDouble, static_cast<size_t>(count), y);
	RETURN_IF_ERR;
	ier = Impl::writeArray("CoordinateZ", RealDouble, static_cast<size_t>(count), z);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::Sol_Particle_Write_Real(char* name, double* value)
{
	char particleNodeName[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(impl->m_solId, particleNodeName);
	int ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId, particleNodeName, 0, NULL);
	RETURN_IF_ERR;

	char arrayname[32];
	DataType_t datatype;
	int dim;
	cgsize_t dimVec;
	ier = cg_array_info(1, arrayname, &datatype, &dim, &dimVec);
	RETURN_IF_ERR;

	return impl->writeArray(name, RealDouble, static_cast<size_t> (dimVec), value);
}

int CgnsFile::Sol_Particle_Write_Integer(char* name, int* value)
{
	char particleNodeName[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(impl->m_solId, particleNodeName);
	int ier = cg_goto(impl->m_fileId, impl->m_baseId, "Zone_t", impl->m_zoneId, particleNodeName, 0, NULL);
	RETURN_IF_ERR;

	char arrayname[32];
	DataType_t datatype;
	int dim;
	cgsize_t dimVec;
	ier = cg_array_info(1, arrayname, &datatype, &dim, &dimVec);
	RETURN_IF_ERR;

	return impl->writeArray(name, Integer, static_cast<size_t> (dimVec), value);
}
