#include "geodatapoint.h"
#include "geodatapointcreator.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

#include <QStandardItem>

GeoDataPointCreator::GeoDataPointCreator(const QString& typeName) :
	GeoDataCreator {typeName, tr("Point")}
{}

QString GeoDataPointCreator::name(unsigned int index) const
{
	return QString("point%1").arg(index);
}

QString GeoDataPointCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Point%1")).arg(index);
}

bool GeoDataPointCreator::isCreatable() const
{
	return true;
}

GeoData* GeoDataPointCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataPoint* point = new GeoDataPoint(parent, this, condition);
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent);
	item->standardItem()->setData(QVariant(tr("Deleting this item will also remove any graph windows associated with this point.  Are you sure you want to delete this item?")), Qt::UserRole + 20);
	point->setPosition(condition->position());
	point->setDefaultMapper();
	return point;
}
