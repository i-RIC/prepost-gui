#ifndef STRUCTURED2DGRIDCGNSIMPORTER_H
#define STRUCTURED2DGRIDCGNSIMPORTER_H

#include "cgnsgridimporter.h"

class Structured2DGridCgnsImporter : public CgnsGridImporter
{
public:
	Structured2DGridCgnsImporter() : CgnsGridImporter() {}
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) {
		return gt == SolverDefinitionGridType::gtStructured2DGrid;
	}

protected:
	bool isZoneCompatible(int fn, int B, int Z);
};

#endif // STRUCTURED2DGRIDCGNSIMPORTER_H
