#include "unstructured2dgridcgnsimporter.h"

#include <guicore/project/projectcgnsfile.h>
#include <cgnslib.h>

bool Unstructured2DGridCgnsImporter::isZoneCompatible(int fn, int B, int Z)
{
	ZoneType_t type;
	int ret, celldim, physdim;
	char basename[ProjectCgnsFile::BUFFERLEN];

	ret = cg_base_read(fn, B, basename, &celldim, &physdim);
	if (ret != 0) {return false;}
	if (celldim != 2) {return false;}
	ret = cg_zone_type(fn, B, Z, &type);
	if (ret != 0) {return false;}
	if (type != Unstructured) {return false;}
	return true;
}
