#ifndef IRICLIB_CGNSFILE_SOLUTIONWRITER_H
#define IRICLIB_CGNSFILE_SOLUTIONWRITER_H

#include "../iriclib_cgnsfile.h"

namespace iRICLib {

class CgnsFile::SolutionWriter
{
public:
	SolutionWriter(CgnsFile::Impl* impl);
	virtual ~SolutionWriter();

	virtual int Sol_Write_Time(double time) = 0;
	virtual int Sol_Write_Iteration(int index) = 0;
	virtual int Sol_Write_GridCoord2d(double *x, double *y) = 0;
	virtual int Sol_Write_GridCoord3d(double *x, double *y, double *z) = 0;
	virtual int Sol_Write_Integer(const char *name, int* data) = 0;
	virtual int Sol_Write_Real(const char *name, double* data) = 0;
	virtual int Sol_Particle_Write_Pos2d(cgsize_t count, double* x, double* y) = 0;
	virtual int Sol_Particle_Write_Pos3d(cgsize_t count, double* x, double* y, double* z) = 0;
	virtual int Sol_Particle_Write_Real(char* name, double* value) = 0;
	virtual int Sol_Particle_Write_Integer(char* name, int* value) = 0;

	virtual int Flush() = 0;

protected:
	CgnsFile::Impl* impl() const;

private:
	CgnsFile::Impl* m_impl;
};

} // namespace iRICLib

#endif // IRICLIB_CGNSFILE_SOLUTIONWRITER_H
