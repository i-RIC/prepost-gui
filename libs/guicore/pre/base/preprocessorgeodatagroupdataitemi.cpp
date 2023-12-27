#include "preprocessorgeodatagroupdataitemi.h"
#include "preprocessorgeodatatopdataitemi.h"

PreProcessorGeoDataGroupDataItemI::PreProcessorGeoDataGroupDataItemI(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent) :
	PreProcessorDataItem(cond->caption(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent)
{}

ProjectData* PreProcessorGeoDataGroupDataItemI::projectData() const
{
	return ProjectDataItem::projectData();
}

PreProcessorGeoDataTopDataItemI* PreProcessorGeoDataGroupDataItemI::geoDataTopDataItem() const
{
	return dynamic_cast<PreProcessorGeoDataTopDataItemI*> (parent());
}
