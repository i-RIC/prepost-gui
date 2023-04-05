#include "geodatapolygongroup.h"
#include "geodatapolygongroupcreator.h"
#include "geodatapolygongroupcsvexporter.h"
#include "geodatapolygongroupcsvimporter.h"
#include "geodatapolygongroupshpexporter.h"
#include "geodatapolygongroupshpimporter.h"

#include <shapefil.h>

GeoDataPolygonGroupCreator::GeoDataPolygonGroupCreator(const QString& typeName) :
	GeoDataPolyDataGroupCreator {typeName, tr("Polygons")}
{
	importers().push_back(new GeoDataPolygonGroupShpImporter(this));
	importers().push_back(new GeoDataPolygonGroupCsvImporter(this));
	exporters().push_back(new GeoDataPolygonGroupShpExporter(this));
	exporters().push_back(new GeoDataPolygonGroupCsvExporter(this));
}

QString GeoDataPolygonGroupCreator::name(unsigned int index) const
{
	return QString("polygons%1").arg(index);
}

QString GeoDataPolygonGroupCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Polygons%1")).arg(index);
}

bool GeoDataPolygonGroupCreator::isCreatable() const
{
	return true;
}

int GeoDataPolygonGroupCreator::shapeType() const
{
	return SHPT_POLYGON;
}

QString GeoDataPolygonGroupCreator::shapeName() const
{
	return tr("polygon");
}

QString GeoDataPolygonGroupCreator::shapeNameCamelCase() const
{
	return tr("Polygon");
}
