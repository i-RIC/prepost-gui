#include "preprocessorgeodatagroupdataitemi.h"

PreProcessorGeoDataGroupDataItemI::PreProcessorGeoDataGroupDataItemI(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent) :
	PreProcessorDataItem(cond->caption(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent)
{}

ProjectData* PreProcessorGeoDataGroupDataItemI::projectData() const
{
	return ProjectDataItem::projectData();
}
