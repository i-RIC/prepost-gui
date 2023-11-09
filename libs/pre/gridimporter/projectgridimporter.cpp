#include "../datamodel/preprocessorgriddataitem.h"
#include "cgnsgridimporter.h"
#include "projectgridimporter.h"

ProjectGridImporter::ProjectGridImporter() :
	GridInternalImporter {}
{}

ProjectGridImporter::~ProjectGridImporter()
{}

void ProjectGridImporter::setGridDataItem(PreProcessorGridDataItem* gridDataItem)
{
	m_gridDataItem = gridDataItem;
}

QString ProjectGridImporter::caption() const
{
	return tr("Project file");
}

QStringList ProjectGridImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("iRIC Project files (*.ipro)");
	return ret;
}

bool ProjectGridImporter::import(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	// the implementation is never used.
	// use PreProcessorGridDataItem::ProjectImporter
	return false;
}
