#include "geodatapolygongroupreadonlycreator.h"
#include "geodatapolygongroupcsvexporter.h"
#include "geodatapolygongroupcsvimporter.h"
#include "geodatapolygongroupshpexporter.h"
#include "geodatapolygongroupshpimporter.h"

#include <shapefil.h>

GeoDataPolygonGroupReadOnlyCreator::GeoDataPolygonGroupReadOnlyCreator(const QString& typeName) :
	GeoDataPolyDataGroupCreator {typeName, tr("Polygons")}
{
	importers().push_back(new GeoDataPolygonGroupShpImporter(this));
	importers().push_back(new GeoDataPolygonGroupCsvImporter(this));
	exporters().push_back(new GeoDataPolygonGroupShpExporter(this));
	exporters().push_back(new GeoDataPolygonGroupCsvExporter(this));
}

QString GeoDataPolygonGroupReadOnlyCreator::name(unsigned int index) const
{
	return QString("polygons%1").arg(index);
}

QString GeoDataPolygonGroupReadOnlyCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Polygons%1")).arg(index);
}

bool GeoDataPolygonGroupReadOnlyCreator::isCreatable() const
{
	return false;
}

int GeoDataPolygonGroupReadOnlyCreator::shapeType() const
{
	return SHPT_POLYGON;
}

QString GeoDataPolygonGroupReadOnlyCreator::shapeName() const
{
	return tr("polygon");
}

QString GeoDataPolygonGroupReadOnlyCreator::shapeNameCamelCase() const
{
	return tr("Polygon");
}

bool GeoDataPolygonGroupReadOnlyCreator::isReadOnly() const
{
	return true;
}
