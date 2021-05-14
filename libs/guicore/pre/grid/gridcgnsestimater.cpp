#include "gridcgnsestimater.h"
#include "structured2dgrid.h"
#include "unstructured2dgrid.h"
#include "structured15dgridwithcrosssection.h"

#include <h5cgnszone.h>

Grid* GridCgnsEstimater::buildGrid(const iRICLib::H5CgnsZone& zone, ProjectDataItem* parent)
{
	if (zone.type() == iRICLib::H5CgnsZone::Type::Unstructured) {
		return new Unstructured2DGrid(parent);
	} else {
		auto size = zone.size();
		if (size[1] == 1) {
			return new Structured15DGridWithCrossSection(parent);
		} else {
			return new Structured2DGrid(parent);
		}
	}
}

GridCgnsEstimater::GridCgnsEstimater()
{}
