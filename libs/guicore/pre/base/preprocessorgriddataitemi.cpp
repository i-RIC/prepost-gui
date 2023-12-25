#include "preprocessorgriddataitemi.h"

PreProcessorGridDataItemI::PreProcessorGridDataItemI(PreProcessorDataItem* parent) :
	PreProcessorDataItem(PreProcessorDataItem::tr("Grid"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent)
{}

QString PreProcessorGridDataItemI::subPath() const
{
	return ProjectDataItem::subPath();
}
