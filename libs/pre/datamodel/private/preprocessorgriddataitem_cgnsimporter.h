#ifndef PREPROCESSORGRIDDATAITEM_CGNSIMPORTER_H
#define PREPROCESSORGRIDDATAITEM_CGNSIMPORTER_H

#include "../preprocessorgriddataitem.h"

class CgnsGridImporter;

class PreProcessorGridDataItem::CgnsImporter
{
public:
	CgnsImporter(PreProcessorGridDataItem* item);

	bool importGrid(const QString& fileName, CgnsGridImporter* importer);

private:
	PreProcessorGridDataItem* m_item;
};

#endif // PREPROCESSORGRIDDATAITEM_CGNSIMPORTER_H
