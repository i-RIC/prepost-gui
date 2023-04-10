#include "geodatapolygon.h"
#include "geodatapolygoncreator.h"
#include "geodatapolygonimporter.h"
#include "geodatapolygonshapeexporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>

GeoDataPolygonCreator::GeoDataPolygonCreator(const QString& typeName) :
	GeoDataCreator {typeName, tr("Polygon")}
{
	// importers().push_back(new GeoDataPolygonImporter(this));
	exporters().push_back(new GeoDataPolygonShapeExporter(this));
}

QString GeoDataPolygonCreator::name(unsigned int index) const
{
	return QString("polygon%1").arg(index);
}

QString GeoDataPolygonCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Polygon%1")).arg(index);
}

bool GeoDataPolygonCreator::isCreatable() const
{
	return false;
}
