#ifndef IRICLIB_CGNSFILE_SOLUTIONWRITER_H
#define IRICLIB_CGNSFILE_SOLUTIONWRITER_H

#include "../iriclib_cgnsfile.h"

namespace iRICLib {

class CgnsFile::SolutionWriter
{
public:
	SolutionWriter(CgnsFile* cgnsFile);
	virtual ~SolutionWriter();

	virtual int fileId() const = 0;
	virtual int baseId() const = 0;
	virtual int zoneId() const = 0;
	virtual int solId() const = 0;

protected:
	CgnsFile* cgnsFile() const;

private:
	CgnsFile* m_cgnsFile;
};

} // namespace iRICLib

#endif // IRICLIB_CGNSFILE_SOLUTIONWRITER_H
