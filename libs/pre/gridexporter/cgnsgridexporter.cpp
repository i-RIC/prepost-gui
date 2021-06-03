#include "cgnsgridexporter.h"

#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/stringtool.h>

#include <string>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>

#include <QFile>
#include <QStringList>

CgnsGridExporter::CgnsGridExporter() :
	GridInternalExporter {}
{}

CgnsGridExporter::~CgnsGridExporter()
{}

QString CgnsGridExporter::caption() const
{
	return tr("CGNS file");
}

QStringList CgnsGridExporter::fileDialogFilters() const
{
	QStringList ret;
	ret << QObject::tr("CGNS files (*.cgns)");
	return ret;
}

bool CgnsGridExporter::isGridTypeSupported(SolverDefinitionGridType::GridType /*gt*/) const
{
	return true;
}

bool CgnsGridExporter::doExport(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	QString tmpname = getProjectData(grid)->tmpFileName();
	try {
		iRICLib::H5CgnsFile file(iRIC::toStr(tmpname), iRICLib::H5CgnsFile::Mode::Create);
		grid->saveToCgnsFile(file.ccBase(), "iRICZone");
	}  catch (...) {
		return false;
	}

	// if it already exists, remove it first.
	QFile cgnsfile(filename);
	if (cgnsfile.exists()) {
		cgnsfile.remove();
	}
	// copy to the specified file.
	bool ret = QFile::copy(tmpname, filename);
	if (! ret) {return false;}

	// Delete the temporary file
	QFile::remove(tmpname);
	return true;
}
