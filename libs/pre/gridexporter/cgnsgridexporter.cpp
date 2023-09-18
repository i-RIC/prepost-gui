#include "cgnsgridexporter.h"

#include <guicore/grid/v4grid.h>
#include <guicore/pre/base/preprocessorgriddataitemi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/grid/v4inputgridio.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
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

bool CgnsGridExporter::doExport(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	// actually this implementation is never used.
	return true;
}
