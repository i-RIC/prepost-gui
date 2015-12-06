#include "grid.h"
#include "gridinternalimporter.h"

GridInternalImporter::GridInternalImporter() :
	QObject(nullptr), GridImporterInterface()
{}

ProjectData* GridInternalImporter::getProjectData(Grid* g)
{
	return g->projectData();
}
