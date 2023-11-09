#include "preprocessorgriddataitem_cgnsexporter.h"
#include "preprocessorgriddataitem_impl.h"
#include "../preprocessorbcgroupdataitem.h"

#include <guicore/pre/grid/v4inputgridio.h>
#include <guicore/project/projectdata.h>
#include <misc/fileremover.h>
#include <misc/stringtool.h>

#include <h5cgnsfile.h>

#include <QFile>

PreProcessorGridDataItem::CgnsExporter::CgnsExporter(PreProcessorGridDataItem* item) :
	m_item {item}
{}

bool PreProcessorGridDataItem::CgnsExporter::exportGrid(const QString& fileName)
{
	auto tmpName = m_item->projectData()->tmpFileName();
	try {
		auto file = new iRICLib::H5CgnsFile(iRIC::toStr(tmpName), iRICLib::H5CgnsFile::Mode::Create);
		iRICLib::H5CgnsZone* zone;
		v4InputGridIO::saveGrid(m_item->impl->m_grid, file->ccBase(), "iRICZone", m_item->offset(), &zone);
		if (m_item->m_bcGroupDataItem != nullptr) {
			m_item->m_bcGroupDataItem->saveToCgnsFile(zone);
		}
		delete file;

		// if it already exists, remove it first.
		QFile cgnsfile(fileName);
		if (cgnsfile.exists()) {
			cgnsfile.remove();
		}
		FileRemover cgnsRemover(tmpName); // in destructer the tmpFile is removed

		// copy to the specified file.
		bool ok = QFile::copy(tmpName, fileName);
		return ok;
	}  catch (...) {
		return false;
	}
}
