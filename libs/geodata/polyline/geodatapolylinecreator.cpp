#include "geodatapolyline.h"
#include "geodatapolylinecreator.h"
#include "geodatapolylineimporter.h"
#include "geodatapolylineshapeexporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>

GeoDataPolyLineCreator::GeoDataPolyLineCreator(const QString& typeName) :
	GeoDataCreator {typeName, tr("Polyline")}
{
	importers().push_back(new GeoDataPolyLineImporter(this));
	exporters().push_back(new GeoDataPolyLineShapeExporter(this));
}

QString GeoDataPolyLineCreator::name(unsigned int index) const
{
	return QString("polyline%1").arg(index);
}

QString GeoDataPolyLineCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Polyline%1")).arg(index);
}

bool GeoDataPolyLineCreator::isCreatable() const
{
	return true;
}

GeoData* GeoDataPolyLineCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataPolyLine* polyline = new GeoDataPolyLine(parent, this, condition);
	polyline->setPosition(condition->position());
	polyline->setDefaultMapper();
	return polyline;
}
