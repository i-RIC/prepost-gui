#include "../../gridimporter/cgnsgridimporter.h"
#include "../preprocessorgridtypedataitem.h"
#include "../preprocessorbcgroupdataitem.h"
#include "preprocessorgriddataitem_cgnsimporter.h"

#include <guicore/misc/cgnszoneselectdialog.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/pre/grid/v4inputgridio.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/fileremover.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>
#include <iriclib_errorcodes.h>

#include <memory>

PreProcessorGridDataItem::CgnsImporter::CgnsImporter(PreProcessorGridDataItem* item) :
	m_item {item}
{}

bool PreProcessorGridDataItem::CgnsImporter::importGrid(const QString& fileName, CgnsGridImporter* importer)
{
	QString cgnsName = fileName;
	QString tmpName;
	std::unique_ptr<FileRemover> remover;

	auto projectData = m_item->projectData();
	if (! iRIC::isAscii(fileName)) {
		tmpName = projectData->tmpFileName();
		QFile::copy(fileName, tmpName);
		cgnsName = tmpName;
		remover.reset(new FileRemover(tmpName));
	}

	try {
		QWidget* parent = m_item->mainWindow();
		iRICLib::H5CgnsFile file(iRIC::toStr(cgnsName), iRICLib::H5CgnsFile::Mode::OpenReadOnly);
		iRICLib::H5CgnsBase* base = file.ccBase();
		iRICLib::H5CgnsZone* zone = nullptr;

		std::vector<iRICLib::H5CgnsZone*> compatibleZones;
		for (int zoneId = 1; zoneId <= base->zoneNum(); ++ zoneId) {
			auto zone = base->zoneById(zoneId);
			if (importer->isZoneCompatible(*zone)) {
				compatibleZones.push_back(zone);
			}
		}

		if (compatibleZones.size() == 0) {
			return false;
		} else if (compatibleZones.size() == 1) {
			zone = compatibleZones.at(0);
		} else {
			CgnsZoneSelectDialog dialog(m_item->preProcessorWindow());
			dialog.setZones(compatibleZones);
			int ret = dialog.exec();
			if (ret == QDialog::Rejected) {return false;}
			zone = dialog.selectedZone();
		}

		// Check the compatibility.
		std::string solverName;
		VersionNumber versionNumber;
		bool ok = (ProjectCgnsFile::readSolverInfo(file, &solverName, &versionNumber) == IRIC_NO_ERROR);
		if (ok == true) {
			SolverDefinition* solverDef = projectData->solverDefinition();
			if (solverDef->name() != solverName || (! solverDef->version().compatibleWith(versionNumber))) {
				int ret = QMessageBox::warning(parent, PreProcessorGridDataItem::tr("Error"),
						PreProcessorGridDataItem::tr("This CGNS file is created for %1 version %2. It is not compatible with the current solver. "
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

		int ier;
		auto grid = v4InputGridIO::load(*zone, m_item->gridTypeDataItem(), "", m_item->offset(), &ier);
		if (ier != IRIC_NO_ERROR) {return false;}

		m_item->setGrid(grid);
		auto bc = m_item->bcGroupDataItem();
		if (bc != nullptr) {
			bc->clear();
			ier = bc->importFromCgnsFile(*zone);
		}
		return true;
	} catch (...) {
		return false;
	}
}
