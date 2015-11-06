#ifndef UNSTRUCTURED2DGRIDPROJECTIMPORTER_H
#define UNSTRUCTURED2DGRIDPROJECTIMPORTER_H

#include "projectgridimporter.h"

class Unstructured2DGridProjectImporter : public ProjectGridImporter
{
	Q_OBJECT
	Q_INTERFACES(GridImporterInterface)

public:
	Unstructured2DGridProjectImporter();
	SolverDefinitionGridType::GridType supportedGridType() const override;

protected:
	CgnsGridImporter* getCgnsImporter() override;
};

#endif // UNSTRUCTURED2DGRIDPROJECTIMPORTER_H
