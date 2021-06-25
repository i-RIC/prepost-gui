#include "structured2dgridcgnsimporter.h"

#include <guicore/project/projectcgnsfile.h>

#include <h5cgnszone.h>

Structured2DGridCgnsImporter::Structured2DGridCgnsImporter() :
	CgnsGridImporter()
{}

SolverDefinitionGridType::GridType Structured2DGridCgnsImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

bool Structured2DGridCgnsImporter::isZoneCompatible(const iRICLib::H5CgnsZone& zone)
{
	return (zone.type() == iRICLib::H5CgnsZone::Type::Structured);
}
