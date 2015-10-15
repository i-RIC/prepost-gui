#include "structured15dgridwithcrosssectioncgnsimporter.h"
#include "structured15dgridwithcrosssectionprojectimporter.h"

SolverDefinitionGridType::GridType Structured15DGridWithCrossSectionProjectImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection;
}

CgnsGridImporter* Structured15DGridWithCrossSectionProjectImporter::getCgnsImporter()
{
	return new Structured15DGridWithCrossSectionCgnsImporter();
}
