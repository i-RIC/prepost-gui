#include "geodatapolyline.h"
#include "geodatapolylinecreator.h"
#include "geodatapolylineimporter.h"
#include "geodatapolylineshapeexporter.h"

#include <guicore/pre/base/preprocessorgeodatadataitemi.h>

#include <QStandardItem>

GeoDataPolyLineCreator::GeoDataPolyLineCreator(const QString& typeName) :
	GeoDataCreator {typeName, tr("Polyline")}
{
	// importers().push_back(new GeoDataPolyLineImporter(this));
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
	return false;
}

