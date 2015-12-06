#include "grid.h"
#include "gridinternalexporter.h"

GridInternalExporter::GridInternalExporter() :
	QObject(nullptr), GridExporterInterface()
{}

ProjectData* GridInternalExporter::getProjectData(Grid* g)
{
	return g->projectData();
}
