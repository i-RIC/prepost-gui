#include "iriclib_cgnsfile_solutionwriterdividesolutions.h"

using namespace iRICLib;

CgnsFile::SolutionWriterDivideSolutions::SolutionWriterDivideSolutions(CgnsFile::Impl* impl) :
	CgnsFile::SolutionWriter(impl)
{}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Time(double time)
{
	return 0;
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Iteration(int index)
{
	return 0;
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_GridCoord2d(double *x, double *y)
{
	return 0;
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_GridCoord3d(double *x, double *y, double *z)
{
	return 0;
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Integer(const char *name, int* data)
{
	return 0;
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Real(const char *name, double* data)
{
	return 0;
}

int CgnsFile::SolutionWriterDivideSolutions::Flush()
{
	return 0;
}
