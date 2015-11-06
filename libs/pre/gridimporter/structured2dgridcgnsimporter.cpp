#include "structured2dgridcgnsimporter.h"

#include <guicore/project/projectcgnsfile.h>

#include <cgnslib.h>

Structured2DGridCgnsImporter::Structured2DGridCgnsImporter() :
	CgnsGridImporter()
{}

SolverDefinitionGridType::GridType Structured2DGridCgnsImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

bool Structured2DGridCgnsImporter::isZoneCompatible(int fn, int B, int Z)
{
	ZoneType_t type;
	int ret, celldim, physdim;
	char basename[ProjectCgnsFile::BUFFERLEN];

	ret = cg_base_read(fn, B, basename, &celldim, &physdim);
	if (ret != 0) {return false;}
	if (celldim != 2) {return false;}
	ret = cg_zone_type(fn, B, Z, &type);
	if (ret != 0) {return false;}
	if (type != Structured) {return false;}
	return true;
}
