#ifndef STRUCTURED2DGRIDPROJECTIMPORTER_H
#define STRUCTURED2DGRIDPROJECTIMPORTER_H

#include "projectgridimporter.h"

class Structured2DGridProjectImporter : public ProjectGridImporter
{

public:
	Structured2DGridProjectImporter();
	SolverDefinitionGridType::GridType supportedGridType() const override;

protected:
	CgnsGridImporter* getCgnsImporter() override;
};

#endif // STRUCTURED2DGRIDPROJECTIMPORTER_H
