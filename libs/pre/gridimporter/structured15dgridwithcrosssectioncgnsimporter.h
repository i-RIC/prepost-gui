#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCGNSIMPORTER_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCGNSIMPORTER_H

#include "cgnsgridimporter.h"

class Structured15DGridWithCrossSectionCgnsImporter : public CgnsGridImporter
{
public:
	Structured15DGridWithCrossSectionCgnsImporter() : CgnsGridImporter() {}
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) {
		return gt ==SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection;
	}

protected:
	bool isZoneCompatible(int fn, int B, int Z);
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCGNSIMPORTER_H
