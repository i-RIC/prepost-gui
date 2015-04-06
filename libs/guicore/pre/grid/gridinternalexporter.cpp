#include "gridinternalexporter.h"
#include "grid.h"

GridInternalExporter::GridInternalExporter()
	: QObject(0), GridExporterInterface()
{

}

ProjectData* GridInternalExporter::getProjectData(Grid* g)
{
	return g->projectData();
}
