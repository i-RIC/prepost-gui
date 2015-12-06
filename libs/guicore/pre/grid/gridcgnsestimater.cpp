#include "gridcgnsestimater.h"
#include "structured2dgrid.h"
#include "unstructured2dgrid.h"
#include "structured15dgridwithcrosssection.h"
#include "project/projectcgnsfile.h"

#include <cgnslib.h>

Grid* GridCgnsEstimater::buildGrid(int fn, int baseId, int zoneId, ProjectDataItem* parent)
{
	int ier;
	cgsize_t size[9];
	ZoneType_t zoneType;
	char buffer[ProjectCgnsFile::BUFFERLEN];

	ier = cg_zone_type(fn, baseId, zoneId, &zoneType);
	if (ier != 0) {return 0;}

	if (zoneType == Unstructured) {
		return new Unstructured2DGrid(parent);
	} else if (zoneType != Structured) {
		return 0;
	}

	ier = cg_zone_read(fn, baseId, zoneId, buffer, size);
	if (ier != 0) {return 0;}
	if (size[1] == 1) {
		return new Structured15DGridWithCrossSection(parent);
	}
	return new Structured2DGrid(parent);
}

GridCgnsEstimater::GridCgnsEstimater()
{}
