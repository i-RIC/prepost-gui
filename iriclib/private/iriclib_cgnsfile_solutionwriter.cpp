#include "iriclib_cgnsfile_solutionwriter.h"

using namespace iRICLib;

CgnsFile::SolutionWriter::SolutionWriter(CgnsFile* cgnsFile) :
	m_cgnsFile {cgnsFile}
{}

CgnsFile::SolutionWriter::~SolutionWriter()
{}

CgnsFile* CgnsFile::SolutionWriter::cgnsFile() const
{
	return m_cgnsFile;
}
