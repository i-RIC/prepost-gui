#ifndef IRICLIB_CGNSFILE_SOLUTIONWRITERSTANDARD_H
#define IRICLIB_CGNSFILE_SOLUTIONWRITERSTANDARD_H

#include "iriclib_cgnsfile_solutionwriter.h"

namespace iRICLib {

class CgnsFile::SolutionWriterStandard : public CgnsFile::SolutionWriter
{
public:
	SolutionWriterStandard(CgnsFile::Impl* impl);

	int Sol_Write_Time(double time) override;
	int Sol_Write_Iteration(int index) override;
	int Sol_Write_GridCoord2d(double *x, double *y) override;
	int Sol_Write_GridCoord3d(double *x, double *y, double *z) override;
	int Sol_Write_Integer(const char* name, int* data) override;
	int Sol_Write_Real(const char* name, double* data) override;

	int Flush() override;

	static int stdSolWriteTime(double time, CgnsFile::Impl* impl);
	static int stdSolWriteIteration(int index, CgnsFile::Impl* impl);
	static int stdSolWriteGridCoord2d(double* x, double* y, int fid, int bid, int zid, int gcid, CgnsFile::Impl* impl);
	static int stdSolWriteGridCoord3d(double* x, double* y, double* z, int fid, int bid, int zid, int gcid, CgnsFile::Impl* impl);
};

} // namespace iRICLib

#endif // IRICLIB_CGNSFILE_SOLUTIONWRITERSTANDARD_H
