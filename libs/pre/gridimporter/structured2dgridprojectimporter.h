#ifndef STRUCTURED2DGRIDPROJECTIMPORTER_H
#define STRUCTURED2DGRIDPROJECTIMPORTER_H

#include "projectgridimporter.h"

class Structured2DGridProjectImporter : public ProjectGridImporter
{

public:
	Structured2DGridProjectImporter() : ProjectGridImporter() {}
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override {
		return gt == SolverDefinitionGridType::gtStructured2DGrid;
	}

protected:
	CgnsGridImporter* getCgnsImporter() override;
};

#endif // STRUCTURED2DGRIDPROJECTIMPORTER_H
