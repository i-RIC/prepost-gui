#ifndef PREPROCESSORBCSETTINGGROUPDATAITEM_SHPIMPORTER_H
#define PREPROCESSORBCSETTINGGROUPDATAITEM_SHPIMPORTER_H

#include "../preprocessorbcsettinggroupdataitem.h"

#include <shapefil.h>

class PreProcessorBCSettingGroupDataItem::ShpImporter
{
public:
	ShpImporter(const QString& fileName);
	~ShpImporter();

	void importPolygons(PreProcessorBCSettingGroupDataItem* item);

private:
	SHPHandle m_shpHandle;
	DBFHandle m_dbfHandle;
};

#endif // PREPROCESSORBCSETTINGGROUPDATAITEM_SHPIMPORTER_H
