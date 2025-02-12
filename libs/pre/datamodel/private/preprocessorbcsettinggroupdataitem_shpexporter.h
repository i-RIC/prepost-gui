#ifndef PREPROCESSORBCSETTINGGROUPDATAITEM_SHPEXPORTER_H
#define PREPROCESSORBCSETTINGGROUPDATAITEM_SHPEXPORTER_H

#include "../preprocessorbcsettinggroupdataitem.h"

#include <shapefil.h>

class PreProcessorBCSettingDataItem;

class PreProcessorBCSettingGroupDataItem::ShpExporter
{
public:
	ShpExporter(const QString& filename);
	~ShpExporter();

	void exportPolygon(PreProcessorBCSettingDataItem* item);

private:
	SHPHandle m_shpHandle;
	DBFHandle m_dbfHandle;

	int m_index;
};

#endif // PREPROCESSORBCSETTINGGROUPDATAITEM_SHPEXPORTER_H
