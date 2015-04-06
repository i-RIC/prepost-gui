#include "gridinternalimporter.h"
#include "grid.h"

GridInternalImporter::GridInternalImporter()
	: QObject(0), GridImporterInterface()
{

}

ProjectData* GridInternalImporter::getProjectData(Grid* g)
{
	return g->projectData();
}
