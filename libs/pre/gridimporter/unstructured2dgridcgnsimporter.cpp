#include "unstructured2dgridcgnsimporter.h"

#include <guicore/project/projectcgnsfile.h>

#include <cgnslib.h>

#include <h5cgnszone.h>

Unstructured2DGridCgnsImporter::Unstructured2DGridCgnsImporter() :
	CgnsGridImporter()
{}

SolverDefinitionGridType::GridType Unstructured2DGridCgnsImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtUnstructured2DGrid;
}

bool Unstructured2DGridCgnsImporter::isZoneCompatible(const iRICLib::H5CgnsZone& zone)
{
	return (zone.type() == iRICLib::H5CgnsZone::Type::Unstructured);
}
