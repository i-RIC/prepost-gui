#include "unstructured2dgridcgnsimporter.h"
#include "unstructured2dgridprojectimporter.h"

SolverDefinitionGridType::GridType Unstructured2DGridProjectImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtUnstructured2DGrid;
}

CgnsGridImporter* Unstructured2DGridProjectImporter::getCgnsImporter()
{
	return new Unstructured2DGridCgnsImporter();
}
