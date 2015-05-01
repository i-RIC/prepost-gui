#ifndef UNSTRUCTURED2DGRIDPROJECTIMPORTER_H
#define UNSTRUCTURED2DGRIDPROJECTIMPORTER_H

#include "projectgridimporter.h"

class Unstructured2DGridProjectImporter : public ProjectGridImporter
{
	Q_OBJECT
	Q_INTERFACES(GridImporterInterface)

public:
	Unstructured2DGridProjectImporter() : ProjectGridImporter() {}
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt) const override {
		return gt == SolverDefinitionGridType::gtUnstructured2DGrid;
	}

protected:
	CgnsGridImporter* getCgnsImporter();
};

#endif // UNSTRUCTURED2DGRIDPROJECTIMPORTER_H
