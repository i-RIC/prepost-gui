#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONPROJECTIMPORTER_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONPROJECTIMPORTER_H

#include "projectgridimporter.h"

class Structured15DGridWithCrossSectionProjectImporter : public ProjectGridImporter
{

public:
	Structured15DGridWithCrossSectionProjectImporter();

	SolverDefinitionGridType::GridType supportedGridType() const override;
	CgnsGridImporter* getCgnsImporter() override;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONPROJECTIMPORTER_H
