#include "preprocessorgeodatatopdataitemi.h"
#include "preprocessorgridtypedataitemi.h"

PreProcessorGeoDataTopDataItemI::PreProcessorGeoDataTopDataItemI(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
	PreProcessorDataItem(itemlabel, icon, parent)
{}

PreProcessorGeoDataTopDataItemI::~PreProcessorGeoDataTopDataItemI()
{}

PreProcessorGridTypeDataItemI* PreProcessorGeoDataTopDataItemI::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItemI*> (parent());
}
