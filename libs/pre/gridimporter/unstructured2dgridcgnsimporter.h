#ifndef UNSTRUCTURED2DGRIDCGNSIMPORTER_H
#define UNSTRUCTURED2DGRIDCGNSIMPORTER_H

#include "cgnsgridimporter.h"

class Unstructured2DGridCgnsImporter : public CgnsGridImporter
{

public:
	Unstructured2DGridCgnsImporter() : CgnsGridImporter() {}
	SolverDefinitionGridType::GridType supportedGridType() const override;

protected:
	bool isZoneCompatible(int fn, int B, int Z) override;
};

#endif // UNSTRUCTURED2DGRIDCGNSIMPORTER_H
