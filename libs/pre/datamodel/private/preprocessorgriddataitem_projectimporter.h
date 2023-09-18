#ifndef PREPROCESSORGRIDDATAITEM_PROJECTIMPORTER_H
#define PREPROCESSORGRIDDATAITEM_PROJECTIMPORTER_H

#include "../preprocessorgriddataitem.h"

class ProjectGridImporter;

class PreProcessorGridDataItem::ProjectImporter
{
public:
	ProjectImporter(PreProcessorGridDataItem* item);

	bool importGrid(const QString& fileName, ProjectGridImporter* importer);

private:
	PreProcessorGridDataItem* m_item;
};

#endif // PREPROCESSORGRIDDATAITEM_PROJECTIMPORTER_H
