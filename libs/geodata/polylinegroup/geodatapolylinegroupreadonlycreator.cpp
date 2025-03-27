#include "geodatapolylinegroup.h"
#include "geodatapolylinegroupreadonlycreator.h"
#include "geodatapolylinegroupcsvexporter.h"
#include "geodatapolylinegroupcsvimporter.h"
#include "geodatapolylinegroupshpexporter.h"
#include "geodatapolylinegroupshpimporter.h"

GeoDataPolyLineGroupReadOnlyCreator::GeoDataPolyLineGroupReadOnlyCreator(const QString& typeName) :
	GeoDataPolyDataGroupCreator {typeName, tr("Lines (read only)")}
{
	importers().push_back(new GeoDataPolyLineGroupShpImporter(this));
	importers().push_back(new GeoDataPolyLineGroupCsvImporter(this));
	exporters().push_back(new GeoDataPolyLineGroupShpExporter(this));
	exporters().push_back(new GeoDataPolyLineGroupCsvExporter(this));
}

QString GeoDataPolyLineGroupReadOnlyCreator::name(unsigned int index) const
{
	return QString("polyLineGroup%1").arg(index);
}

QString GeoDataPolyLineGroupReadOnlyCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Lines%1")).arg(index);
}

bool GeoDataPolyLineGroupReadOnlyCreator::isCreatable() const
{
	return true;
}

int GeoDataPolyLineGroupReadOnlyCreator::shapeType() const
{
	return SHPT_ARC;
}

QString GeoDataPolyLineGroupReadOnlyCreator::shapeName() const
{
	return tr("line");
}

QString GeoDataPolyLineGroupReadOnlyCreator::shapeNameCamelCase() const
{
	return tr("Line");
}

bool GeoDataPolyLineGroupReadOnlyCreator::isReadOnly() const
{
	return true;
}
