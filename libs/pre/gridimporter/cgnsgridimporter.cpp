#include "cgnsgridimporter.h"

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

bool CgnsGridImporter::import(v4InputGrid* /*grid*/, const QString& /*filename*/, const QString& /*selectedFilter*/, QWidget* /*parent*/)
{
	// the implementation is never used.
	// use PreProcessorGridDataItem::CgnsImporter
	return true;
}
