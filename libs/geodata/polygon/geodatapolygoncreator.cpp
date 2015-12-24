#include "geodatapolygon.h"
#include "geodatapolygoncreator.h"
#include "geodatapolygonimporter.h"
#include "geodatapolygonshapeexporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>

GeoDataPolygonCreator::GeoDataPolygonCreator() :
	GeoDataCreator {}
{
	m_caption = GeoDataPolygonCreator::tr("Polygon");

	m_importers.append(new GeoDataPolygonImporter(this));
	m_exporters.append(new GeoDataPolygonShapeExporter(this));
}

QString GeoDataPolygonCreator::name(unsigned int index)
{
	return QString("polygon%1").arg(index);
}

QString GeoDataPolygonCreator::defaultCaption(unsigned int index)
{
	return QString(tr("Polygon%1")).arg(index);
}

bool GeoDataPolygonCreator::isCreatable() const
{
	return true;
}

GeoData* GeoDataPolygonCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataPolygon* polygon = new GeoDataPolygon(parent, this, condition);
	polygon->setPosition(condition->position());
	polygon->setDefaultMapper();
	return polygon;
}
