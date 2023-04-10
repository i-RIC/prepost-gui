#include "geodatapolylinegroup.h"
#include "geodatapolylinegroupcreator.h"
#include "geodatapolylinegroupcsvexporter.h"
#include "geodatapolylinegroupcsvimporter.h"
#include "geodatapolylinegroupshpexporter.h"
#include "geodatapolylinegroupshpimporter.h"

GeoDataPolyLineGroupCreator::GeoDataPolyLineGroupCreator(const QString& typeName) :
	GeoDataPolyDataGroupCreator {typeName, tr("Lines")}
{
	importers().push_back(new GeoDataPolyLineGroupShpImporter(this));
	importers().push_back(new GeoDataPolyLineGroupCsvImporter(this));
	exporters().push_back(new GeoDataPolyLineGroupShpExporter(this));
	exporters().push_back(new GeoDataPolyLineGroupCsvExporter(this));
}

QString GeoDataPolyLineGroupCreator::name(unsigned int index) const
{
	return QString("polyLineGroup%1").arg(index);
}

QString GeoDataPolyLineGroupCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Lines%1")).arg(index);
}

bool GeoDataPolyLineGroupCreator::isCreatable() const
{
	return true;
}

int GeoDataPolyLineGroupCreator::shapeType() const
{
	return SHPT_ARC;
}

QString GeoDataPolyLineGroupCreator::shapeName() const
{
	return tr("line");
}

QString GeoDataPolyLineGroupCreator::shapeNameCamelCase() const
{
	return tr("Line");
}
