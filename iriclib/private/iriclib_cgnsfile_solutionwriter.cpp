#include "iriclib_cgnsfile_solutionwriter.h"

using namespace iRICLib;

CgnsFile::SolutionWriter::SolutionWriter(CgnsFile::Impl *impl) :
	m_impl {impl}
{}

CgnsFile::SolutionWriter::~SolutionWriter()
{}

CgnsFile::Impl* CgnsFile::SolutionWriter::impl() const
{
	return m_impl;
}
