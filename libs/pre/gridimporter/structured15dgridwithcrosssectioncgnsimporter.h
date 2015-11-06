#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCGNSIMPORTER_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCGNSIMPORTER_H

#include "cgnsgridimporter.h"

class Structured15DGridWithCrossSectionCgnsImporter : public CgnsGridImporter
{

public:
	Structured15DGridWithCrossSectionCgnsImporter();
	SolverDefinitionGridType::GridType supportedGridType() const override;

protected:
	bool isZoneCompatible(int fn, int B, int Z) override;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCGNSIMPORTER_H
