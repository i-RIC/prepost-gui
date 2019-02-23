#include "geodatapolyline.h"
#include "geodatapolylinecreator.h"
#include "geodatapolylineimporter.h"
#include "geodatapolylineshapeexporter.h"

#include <guibase/irictoolbar.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

#include <QAction>
#include <QIcon>
#include <QStandardItem>

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
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent);
	item->standardItem()->setData(QVariant("Deleting this item will also remove any graph windows associated with this polyline.  Are you sure you want to delete this item?"), Qt::UserRole + 20);
	polyline->setPosition(condition->position());
	polyline->setDefaultMapper();
	return polyline;
}
