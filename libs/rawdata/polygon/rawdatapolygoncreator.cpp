#include "rawdatapolygon.h"
#include "rawdatapolygoncreator.h"
#include "rawdatapolygonimporter.h"
#include "rawdatapolygonshapeexporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>

RawDataPolygonCreator::RawDataPolygonCreator()
	: RawDataCreator()
{
	m_caption = tr("Polygon");

	m_importers.append(new RawDataPolygonImporter(this));
	m_exporters.append(new RawDataPolygonShapeExporter(this));
}

RawData* RawDataPolygonCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	RawDataPolygon* polygon = new RawDataPolygon(parent, this, condition);
	polygon->setPosition(condition->position());
	polygon->setDefaultMapper();
	return polygon;
}

QString RawDataPolygonCreator::name(unsigned int index)
{
	return QString("polygon%1").arg(index);
}

QString RawDataPolygonCreator::defaultCaption(unsigned int index)
{
	return QString(tr("Polygon%1")).arg(index);
}
