#ifndef PREPROCESSORGRIDDATAITEM_CGNSEXPORTER_H
#define PREPROCESSORGRIDDATAITEM_CGNSEXPORTER_H

#include "../preprocessorgriddataitem.h"

class PreProcessorGridDataItem::CgnsExporter
{
public:
	CgnsExporter(PreProcessorGridDataItem* item);

	bool exportGrid(const QString& fileName);

private:
	PreProcessorGridDataItem* m_item;
};

#endif // PREPROCESSORGRIDDATAITEM_CGNSEXPORTER_H
