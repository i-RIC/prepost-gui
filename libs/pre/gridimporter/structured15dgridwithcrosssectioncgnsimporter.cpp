#include "structured15dgridwithcrosssectioncgnsimporter.h"

#include <guicore/project/projectcgnsfile.h>

#include <cgnslib.h>

bool Structured15DGridWithCrossSectionCgnsImporter::isZoneCompatible(int fn, int B, int Z)
{
	ZoneType_t type;
	int ret, celldim, physdim;
	char buffer[ProjectCgnsFile::BUFFERLEN];

	ret = cg_base_read(fn, B, buffer, &celldim, &physdim);
	if (ret != 0) {return false;}
	if (celldim != 2) {return false;}
	ret = cg_zone_type(fn, B, Z, &type);
	if (ret != 0) {return false;}
	if (type != Structured) {return false;}
	cgsize_t size[9];
	ret = cg_zone_read(fn, B, Z, buffer, size);
	if (ret != 0) {return false;}
	int riversize = size[0];
	// find "GridCrosssections" under the grid.
	ret = cg_goto(fn, B, "Base_t", B, "Zone_t", Z, "GridCrosssections", 0, "end");
	if (ret != 0) {return false;}
	int narrays;
	ret = cg_narrays(&narrays);
	if (ret != 0) {return false;}
	if (riversize != narrays) {return false;}
	return true;
}
