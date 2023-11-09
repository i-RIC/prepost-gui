#ifndef UNSTRUCTURED2DGRIDCGNSIMPORTER_H
#define UNSTRUCTURED2DGRIDCGNSIMPORTER_H

#include "cgnsgridimporter.h"

class Unstructured2DGridCgnsImporter : public CgnsGridImporter
{
public:
	Unstructured2DGridCgnsImporter();
	SolverDefinitionGridType::GridType supportedGridType() const override;

private:
	bool isZoneCompatible(const iRICLib::H5CgnsZone& zone) override;
};

#endif // UNSTRUCTURED2DGRIDCGNSIMPORTER_H
