#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONPROJECTIMPORTER_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONPROJECTIMPORTER_H

#include "projectgridimporter.h"

class Structured15DGridWithCrossSectionProjectImporter : public ProjectGridImporter
{

public:
	Structured15DGridWithCrossSectionProjectImporter() : ProjectGridImporter() {}
	SolverDefinitionGridType::GridType supportedGridType() const;

protected:
	CgnsGridImporter* getCgnsImporter() override;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONPROJECTIMPORTER_H
