#ifndef STRUCTURED2DGRIDCGNSIMPORTER_H
#define STRUCTURED2DGRIDCGNSIMPORTER_H

#include "cgnsgridimporter.h"

class Structured2DGridCgnsImporter : public CgnsGridImporter
{

public:
	Structured2DGridCgnsImporter() : CgnsGridImporter() {}
	SolverDefinitionGridType::GridType supportedGridType() const override;

protected:
	bool isZoneCompatible(int fn, int B, int Z) override;
};

#endif // STRUCTURED2DGRIDCGNSIMPORTER_H
