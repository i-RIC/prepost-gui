#include "structured2dgridcgnsimporter.h"
#include "structured2dgridprojectimporter.h"

Structured2DGridProjectImporter::Structured2DGridProjectImporter() :
	ProjectGridImporter()
{}

SolverDefinitionGridType::GridType Structured2DGridProjectImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

CgnsGridImporter* Structured2DGridProjectImporter::getCgnsImporter()
{
	return new Structured2DGridCgnsImporter();
}
