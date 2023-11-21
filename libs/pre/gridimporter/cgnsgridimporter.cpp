#include "cgnsgridimporter.h"
#include "../datamodel/preprocessorbcgroupdataitem.h"
#include "../datamodel/preprocessorgriddataitem.h"

#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/fileremover.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>

#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVector2D>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>
#include <iriclib_errorcodes.h>

#include <sstream>

CgnsGridImporter::CgnsGridImporter() :
	GridInternalImporter {},
	m_gridDataItem {nullptr},
	m_disableWarning {false}
{}

QStringList CgnsGridImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("CGNS files (*.cgn *.cgns)");
	return ret;
}

QString CgnsGridImporter::caption() const
{
	return tr("CGNS file");
}

bool CgnsGridImporter::import(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	auto projectData = getProjectData(grid);
	auto tmpName = projectData->tmpFileName();
	QFile::copy(filename, tmpName);
	FileRemover remover(tmpName);

	try {
		iRICLib::H5CgnsFile file(iRIC::toStr(tmpName), iRICLib::H5CgnsFile::Mode::OpenReadOnly);
		iRICLib::H5CgnsZone* zone;

		bool ok = selectZoneForImporting(file, &zone, parent);
		if (! ok) {return false;}

		// Check the compatibility.
		std::string solverName;
		VersionNumber versionNumber;
		ok = (ProjectCgnsFile::readSolverInfo(file, &solverName, &versionNumber) == IRIC_NO_ERROR);
		if (ok == true) {
			SolverDefinition* solverDef = projectData->solverDefinition();
			if (solverDef->name() != solverName || (! solverDef->version().compatibleWith(versionNumber))) {
				int ret = QMessageBox::warning(parent, tr("Error"),
						tr("This CGNS file is created for %1 version %2. It is not compatible with the current solver. "
						"Maybe only some part of the grid will be imported.\nDo you really want to import grid from this file?").arg(solverName.c_str()).arg(versionNumber.toString()),
																			 QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
				if (ret == QMessageBox::No) {return false;}
			}
		} else {
			// error occured reading solver information.
			int ret = QMessageBox::warning(parent, tr("Warning"),
																		 tr("This CGNS file does not have solver information. "
																				"We can not check whether this CGNS file is compatible with the solver. If it is not compatible, maybe only some part of the grid will be imported.\nDo you really want to import grid from this file?"),
																		 QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (ret == QMessageBox::No) {return false;}
		}

		ok = (grid->loadFromCgnsFile(*zone) == IRIC_NO_ERROR);
		if (ok) {
			m_gridDataItem->setGrid(grid);
			// if boundary condition exists, import it.
			auto bc = m_gridDataItem->bcGroupDataItem();
			if (bc != nullptr) {
				bc->clear();
				bc->importFromCgnsFile(*zone);
			}
		}

		if (! ok) {
			QMessageBox::critical(parent, tr("Error"), tr("Error occured while importing grid."));
			return false;
		}
		return true;
	} catch (...) {
		return false;
	}
}

bool CgnsGridImporter::selectZoneForImporting(const iRICLib::H5CgnsFile& file, iRICLib::H5CgnsZone** selectedZone, QWidget* parent)
{
	auto ccBase = file.ccBase();
	auto zones = ccBase->zoneNames();

	QStringList compatibleZoneNames;
	for (auto zname : zones) {
		auto z = ccBase->zone(zname);
		if (isZoneCompatible(*z)) {
			compatibleZoneNames.push_back(zname.c_str());
		}
	}
	if (compatibleZoneNames.size() == 0) {
		if (! m_disableWarning) {
			QMessageBox::warning(parent, tr("Warning"), tr("This file does not contain grid that can be imported."));
		}
		return false;
	} else if (compatibleZoneNames.size() == 1) {
		auto name = iRIC::toStr(compatibleZoneNames[0]);
		*selectedZone = ccBase->zone(name);
	} else {
		bool ok;
		auto zoneName = QInputDialog::getItem(parent, tr("Select grid"), tr("Select grid to import."), compatibleZoneNames, 0, false, &ok);
		if (! ok) {return false;}

		*selectedZone = ccBase->zone(iRIC::toStr(zoneName));
	}
	return true;
}

void CgnsGridImporter::setGridDataItem(PreProcessorGridDataItem* gridDataItem)
{
	m_gridDataItem = gridDataItem;
}

void CgnsGridImporter::setDisableWarning(bool disable)
{
	m_disableWarning = disable;
}
