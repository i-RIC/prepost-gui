#include "grid.h"
#include "gridinternalimporter.h"

GridInternalImporter::GridInternalImporter()
	: QObject(0), GridImporterInterface()
{

}

ProjectData* GridInternalImporter::getProjectData(Grid* g)
{
	return g->projectData();
}
