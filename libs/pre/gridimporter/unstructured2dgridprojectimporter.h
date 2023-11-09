#ifndef UNSTRUCTURED2DGRIDPROJECTIMPORTER_H
#define UNSTRUCTURED2DGRIDPROJECTIMPORTER_H

#include "projectgridimporter.h"

class Unstructured2DGridProjectImporter : public ProjectGridImporter
{
	Q_OBJECT
	Q_INTERFACES(GridImporterI)

public:
	Unstructured2DGridProjectImporter();

	SolverDefinitionGridType::GridType supportedGridType() const override;
	CgnsGridImporter* getCgnsImporter() override;
};

#endif // UNSTRUCTURED2DGRIDPROJECTIMPORTER_H
