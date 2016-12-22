#include "iriclib_cgnsfile_impl.h"
#include "iriclib_cgnsfile_solutionwriterstandard.h"
#include "../error_macros.h"

#include <cgnslib.h>

using namespace iRICLib;

CgnsFile::SolutionWriterStandard::SolutionWriterStandard(CgnsFile::Impl* impl) :
	CgnsFile::SolutionWriter(impl)
{}

int CgnsFile::SolutionWriterStandard::Sol_Write_Time(double time)
{
	Impl* i = impl();
	int ier = stdSolWriteTime(time, i);
	RETURN_IF_ERR;

	return Impl::addSolutionNode(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solId, &(i->m_solPointers));
}

int CgnsFile::SolutionWriterStandard::Sol_Write_Iteration(int index)
{
	return stdSolWriteIteration(index, impl());
}

int CgnsFile::SolutionWriterStandard::Sol_Write_GridCoord2d(double *x, double *y)
{
	Impl* i = impl();
	int ier = CgnsFile::Impl::addSolutionGridCoordNode(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solId, &(i->m_solGridCoordPointers));
	RETURN_IF_ERR;

	return stdSolWriteGridCoord2d(x, y, i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solId + 1, i);
}

int CgnsFile::SolutionWriterStandard::Sol_Write_GridCoord3d(double *x, double *y, double *z)
{
	Impl* i = impl();
	int ier = CgnsFile::Impl::addSolutionGridCoordNode(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solId, &(i->m_solGridCoordPointers));
	RETURN_IF_ERR;

	return stdSolWriteGridCoord3d(x, y, z, i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solId + 1, i);
}

int CgnsFile::SolutionWriterStandard::Sol_Write_Integer(const char *name, int* data)
{
	int F;
	return cg_field_write(impl()->m_fileId, impl()->m_baseId, impl()->m_zoneId, impl()->m_solId, Integer, name, data, &F);
}

int CgnsFile::SolutionWriterStandard::Sol_Write_Real(const char *name, double* data)
{
	int F;
	return cg_field_write(impl()->m_fileId, impl()->m_baseId, impl()->m_zoneId, impl()->m_solId, RealDouble, name, data, &F);
}

int CgnsFile::SolutionWriterStandard::Sol_Particle_Write_Pos2d(cgsize_t count, double* x, double* y)
{
	Impl* i = impl();
	return stdSolParticleWritePos2d(count, x, y, i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solId);
}

int CgnsFile::SolutionWriterStandard::Sol_Particle_Write_Pos3d(cgsize_t count, double* x, double* y, double* z)
{
	Impl* i = impl();
	return stdSolParticleWritePos3d(count, x, y, z, i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solId);
}

int CgnsFile::SolutionWriterStandard::Sol_Particle_Write_Real(char* name, double* value)
{
	Impl* i = impl();
	return stdSolParticleWriteReal(name, value, i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solId);
}

int CgnsFile::SolutionWriterStandard::Sol_Particle_Write_Integer(char* name, int* value)
{
	Impl* i = impl();
	return stdSolParticleWriteInteger(name, value, i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solId);
}

int CgnsFile::SolutionWriterStandard::Flush()
{
	return 0;
}

int CgnsFile::SolutionWriterStandard::stdSolWriteTime(double time, CgnsFile::Impl* impl)
{
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
	return cg_array_write("TimeValues", RealDouble, 1, &dimVec, impl->m_solTimes.data());
}

int CgnsFile::SolutionWriterStandard::stdSolWriteIteration(int index, CgnsFile::Impl* impl)
{
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
	return Impl::addSolutionNode(impl->m_fileId, impl->m_baseId, impl->m_zoneId, impl->m_solId, &(impl->m_solPointers));
}

int CgnsFile::SolutionWriterStandard::stdSolWriteGridCoord2d(double* x, double* y, int fid, int bid, int zid, int gcid, CgnsFile::Impl* impl)
{
	int ier = cg_goto(fid, bid, "Zone_t", zid, "GridCoordinates_t", gcid, NULL);

	cgsize_t dimVec[2];
	dimVec[0] = impl->m_zoneSize[0];
	dimVec[1] = impl->m_zoneSize[1];

	ier = cg_array_write("CoordinateX", RealDouble, 2, dimVec, x);
	RETURN_IF_ERR;
	ier = cg_array_write("CoordinateY", RealDouble, 2, dimVec, y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::SolutionWriterStandard::stdSolWriteGridCoord3d(double* x, double* y, double* z, int fid, int bid, int zid, int gcid, CgnsFile::Impl* impl)
{
	int ier = cg_goto(fid, bid, "Zone_t", zid, "GridCoordinates_t", gcid, NULL);

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
}

int CgnsFile::SolutionWriterStandard::stdSolParticleWritePos2d(cgsize_t count, double* x, double* y, int fid, int bid, int zid, int sid)
{
	int ier = Impl::addParticleSolutionNode(fid, bid, zid, sid);
	RETURN_IF_ERR;

	char particleNodeName[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(sid, particleNodeName);

	ier = cg_goto(fid, bid, "Zone_t", zid, particleNodeName, 0, NULL);
	RETURN_IF_ERR;
	ier = Impl::writeArray("CoordinateX", RealDouble, static_cast<size_t>(count), x);
	RETURN_IF_ERR;
	ier = Impl::writeArray("CoordinateY", RealDouble, static_cast<size_t>(count), y);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::SolutionWriterStandard::stdSolParticleWritePos3d(cgsize_t count, double* x, double* y, double* z, int fid, int bid, int zid, int sid)
{
	int ier = Impl::addParticleSolutionNode(fid, bid, zid, sid);
	RETURN_IF_ERR;

	char particleNodeName[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(sid, particleNodeName);

	ier = cg_goto(fid, bid, "Zone_t", zid, particleNodeName, 0, NULL);
	RETURN_IF_ERR;
	ier = Impl::writeArray("CoordinateX", RealDouble, static_cast<size_t>(count), x);
	RETURN_IF_ERR;
	ier = Impl::writeArray("CoordinateY", RealDouble, static_cast<size_t>(count), y);
	RETURN_IF_ERR;
	ier = Impl::writeArray("CoordinateZ", RealDouble, static_cast<size_t>(count), z);
	RETURN_IF_ERR;
	return 0;
}

int CgnsFile::SolutionWriterStandard::stdSolParticleWriteReal(char* name, double* value, int fid, int bid, int zid, int sid)
{
	char particleNodeName[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(sid, particleNodeName);
	int ier = cg_goto(fid, bid, "Zone_t", zid, particleNodeName, 0, NULL);
	RETURN_IF_ERR;

	char arrayname[32];
	DataType_t datatype;
	int dim;
	cgsize_t dimVec;
	ier = cg_array_info(1, arrayname, &datatype, &dim, &dimVec);
	RETURN_IF_ERR;

	return Impl::writeArray(name, RealDouble, static_cast<size_t> (dimVec), value);
}

int CgnsFile::SolutionWriterStandard::stdSolParticleWriteInteger(char* name, int* value, int fid, int bid, int zid, int sid)
{
	char particleNodeName[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(sid, particleNodeName);
	int ier = cg_goto(fid, bid, "Zone_t", zid, particleNodeName, 0, NULL);
	RETURN_IF_ERR;

	char arrayname[32];
	DataType_t datatype;
	int dim;
	cgsize_t dimVec;
	ier = cg_array_info(1, arrayname, &datatype, &dim, &dimVec);
	RETURN_IF_ERR;

	return Impl::writeArray(name, Integer, static_cast<size_t> (dimVec), value);
}
