#ifndef STRUCTURED2DGRIDCGNSIMPORTER_H
#define STRUCTURED2DGRIDCGNSIMPORTER_H

#include "cgnsgridimporter.h"

class Structured2DGridCgnsImporter : public CgnsGridImporter
{
public:
	Structured2DGridCgnsImporter();
	SolverDefinitionGridType::GridType supportedGridType() const override;

private:
	bool isZoneCompatible(const iRICLib::H5CgnsZone& zone) override;
};

#endif // STRUCTURED2DGRIDCGNSIMPORTER_H
