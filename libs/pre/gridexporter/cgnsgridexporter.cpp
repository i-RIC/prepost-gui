#include "cgnsgridexporter.h"

#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/stringtool.h>

#include <string>

#include <cgnslib.h>
#include <iriclib.h>

#include <QFile>
#include <QStringList>

CgnsGridExporter::CgnsGridExporter() :
	GridInternalExporter {}
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

bool CgnsGridExporter::doExport(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	QString tmpname;
	int fn, B;
	// create temporary CGNS file.
	bool ret = createTempCgns(grid, tmpname, fn, B);
	if (! ret) {return false;}

	// write to the iRICZone.
	std::string zoneName{"iRICZone"};

	ret = grid->saveToCgnsFile(fn, B, zoneName.c_str());

	ret = closeAndMoveCgns(tmpname, fn, filename);
	return ret;
}

bool CgnsGridExporter::createTempCgns(Grid* grid, QString& tmpname, int& fn, int& B)
{
	tmpname = getProjectData(grid)->tmpFileName();
	ProjectCgnsFile::createNewFile(tmpname, 2, 2);
	const SolverDefinitionAbstract* solverDef = &(getProjectData(grid)->solverDefinition()->abstract());
	ProjectCgnsFile::writeSolverInfo(tmpname, solverDef);

	int iret;
	iret = cg_open(iRIC::toStr(tmpname).c_str(), CG_MODE_MODIFY, &fn);
	if (iret != 0) {return false;}
	iret = cg_iRIC_GotoBase(fn, &B);
	if (iret != 0) {
		cg_close(fn);
		return false;
	}
	return true;
}

bool CgnsGridExporter::closeAndMoveCgns(const QString& tmpname, int fn, const QString& cgnsname)
{
	cg_close(fn);

	// if the target file already exists, remove it.
	if (QFile::exists(cgnsname)) {
		bool deleteOk = QFile::remove(cgnsname);
		if (! deleteOk) {
			QFile::remove(tmpname);
			return false;
		}
	}
	// if it already exists, remove it first.
	QFile cgnsfile(cgnsname);
	if (cgnsfile.exists()) {
		cgnsfile.remove();
	}
	// copy to the specified file.
	bool ret = QFile::copy(tmpname, cgnsname);
	if (! ret) {return false;}
	// Delete the temporary file
	QFile::remove(tmpname);

	return true;
}
