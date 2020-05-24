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
	point->setPosition(condition->position());
	point->setDefaultMapper();
	return point;
}
