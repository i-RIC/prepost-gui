#ifndef IRICLIB_CGNSFILE_SOLUTIONWRITERDIVIDESOLUTIONS_H
#define IRICLIB_CGNSFILE_SOLUTIONWRITERDIVIDESOLUTIONS_H

#include "iriclib_cgnsfile_solutionwriter.h"

#include <string>

namespace iRICLib {

class CgnsFile::SolutionWriterDivideSolutions : public CgnsFile::SolutionWriter
{
public:
	SolutionWriterDivideSolutions(CgnsFile::Impl* impl);

	int Sol_Write_Time(double time) override;
	int Sol_Write_Iteration(int index) override;
	int Sol_Write_GridCoord2d(double *x, double *y) override;
	int Sol_Write_GridCoord3d(double *x, double *y, double *z) override;
	int Sol_Write_Integer(const char *name, int* data) override;
	int Sol_Write_Real(const char *name, double* data) override;

	int Sol_Particle_Write_Pos2d(cgsize_t count, double* x, double* y) override;
	int Sol_Particle_Write_Pos3d(cgsize_t count, double* x, double* y, double* z) override;
	int Sol_Particle_Write_Real(char* name, double* value) override;
	int Sol_Particle_Write_Integer(char* name, int* value) override;

	int Flush() override;

private:
	int closeFileIfOpen();

	static int setupSolutionFile(const std::string& solFileName, CgnsFile::Impl* i, int* fileId, int* baseId, int* zoneId);
	static int linkParticleSolution(const char* filename_src, int fid_src, int bid_src, int zid_src, int sid_src, int fid_tgt, int bid_tgt, int zid_tgt, char* linkname = nullptr);

	std::string m_fileName;
	int m_fileId;
	int m_baseId;
	int m_zoneId;
};

} // namespace iRICLib

#endif // IRICLIB_CGNSFILE_SOLUTIONWRITERDIVIDESOLUTIONS_H
