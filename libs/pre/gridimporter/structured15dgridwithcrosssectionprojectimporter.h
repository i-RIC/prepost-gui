#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONPROJECTIMPORTER_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONPROJECTIMPORTER_H

#include "projectgridimporter.h"

class Structured15DGridWithCrossSectionProjectImporter : public ProjectGridImporter
{

public:
	Structured15DGridWithCrossSectionProjectImporter() : ProjectGridImporter() {}
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override {
		return gt ==SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection;
	}

protected:
	CgnsGridImporter* getCgnsImporter() override;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONPROJECTIMPORTER_H
