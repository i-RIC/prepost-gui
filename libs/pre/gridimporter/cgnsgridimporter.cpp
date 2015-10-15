#include "cgnsgridimporter.h"

#include <guibase/cgnszoneselectdialog.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>

#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVector2D>

#include <cgnslib.h>
#include <iriclib.h>
#include <sstream>

CgnsGridImporter::CgnsGridImporter() :
	GridInternalImporter {}
{}

QStringList CgnsGridImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("CGNS files (*.cgn)");
	return ret;
}

QString CgnsGridImporter::caption() const
{
	return tr("CGNS file");
}

bool CgnsGridImporter::import(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	// copy CGNS file to temporary file.
	QString tmpname;
	int fn, base, zoneid;

	bool bret = openCgnsFileForImporting(grid, filename, tmpname, fn, base, zoneid, parent);
	if (! bret) {goto ERROR_OPENING;}

	// load grid.
	bret = grid->loadFromCgnsFile(fn, base, zoneid);
	if (! bret) {
		QMessageBox::critical(parent, tr("Error"),
													tr("Error occured while importing grid."));
		goto ERROR_AFTER_OPENING;
	}
	closeAndRemoveTempCgnsFile(fn, tmpname);
	return true;

ERROR_AFTER_OPENING:
	closeAndRemoveTempCgnsFile(fn, tmpname);
ERROR_OPENING:
	return false;
}

bool CgnsGridImporter::openCgnsFileForImporting(Grid* grid, const QString& filename, QString& tmpname, int& fn, int& base, int& zoneid, QWidget* parent)
{
	// copy CGNS file to temporary file.
	tmpname = getProjectData(grid)->tmpFileName();
	// Copy to a temporary file.
	bool bret = QFile::copy(filename, tmpname);
	if (! bret) {return false;}
	QString solverName;
	VersionNumber versionNumber;
	QList<int> zoneids;
	QStringList zonenames;

	int iret;
	iret = cg_open(iRIC::toStr(tmpname).c_str(), CG_MODE_READ, &fn);
	if (iret != 0) {goto OPEN_ERROR_OPENING;}
	iret = cg_iRIC_GotoBase(fn, &base);
	if (iret != 0) {goto OPEN_ERROR_AFTER_OPENING;}
	// Count the number of zones in the base.
	int nzones;
	iret = cg_nzones(fn, base, &nzones);
	if (iret != 0) {goto OPEN_ERROR_AFTER_OPENING;}
	for (int Z = 1; Z <= nzones; ++Z) {
		char buffer[ProjectCgnsFile::BUFFERLEN];
		cgsize_t size[9];
		iret = cg_zone_read(fn, base, Z, buffer, size);
		if (iret != 0) {goto OPEN_ERROR_AFTER_OPENING;}
		if (isZoneCompatible(fn, base, Z)) {
			zonenames.append(QString(buffer));
			zoneids.append(Z);
		}
	}
	if (zonenames.count() == 0) {
		// there is no compatible zone.
		goto OPEN_ERROR_AFTER_OPENING;
	}
	if (zonenames.count() == 1) {
		// there is only one grid that can be imported.
		zoneid = zoneids.at(0);
	} else {
		// there are more than 1 zones that has compatible grid.
		// we have to select.
		CgnsZoneSelectDialog dialog(parent);
		dialog.setZones(zoneids, zonenames);
		iret = dialog.exec();
		if (iret == QDialog::Rejected) {goto OPEN_ERROR_AFTER_OPENING;}
		zoneid = dialog.zoneId();
	}
	cg_iRIC_Set_ZoneId_Mul(fn, zoneid);
	// Check the compatibility.
	bret = ProjectCgnsFile::readSolverInfo(fn, solverName, versionNumber);
	if (bret == true) {
		SolverDefinition* solverDef = getProjectData(grid)->solverDefinition();
		if (solverDef->name() != solverName || (! solverDef->version().compatibleWith(versionNumber))) {
			int ret = QMessageBox::warning(parent, tr("Error"),
																		 tr("This CGNS file is created for %1 version %2. It is not compatible with the current solver. Maybe only some part of the grid will be imported.\nDo you really want to import grid from this file?").arg(solverName).arg(versionNumber.toString()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (ret == QMessageBox::No) {goto OPEN_ERROR_AFTER_OPENING;}
		}
	} else {
		// error occured reading solver information.
		int ret = QMessageBox::warning(parent, tr("Warning"),
																	 tr("This CGNS file does not have solver information. "
																			"We can not check whether this CGNS file is compatible with the solver. If it is not compatible, maybe only some part of the grid will be imported.\nDo you really want to import grid from this file?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {goto OPEN_ERROR_AFTER_OPENING;}
	}
	return true;

OPEN_ERROR_AFTER_OPENING:
	closeAndRemoveTempCgnsFile(fn, tmpname);
OPEN_ERROR_OPENING:
	return false;
}

void CgnsGridImporter::closeAndRemoveTempCgnsFile(int fn, const QString& filename)
{
	cg_close(fn);
	// Delete the temporary file
	QFile::remove(filename);
}
