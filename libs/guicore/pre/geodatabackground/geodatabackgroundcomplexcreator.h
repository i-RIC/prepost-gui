#ifndef GEODATABACKGROUNDCOMPLEXCREATOR_H
#define GEODATABACKGROUNDCOMPLEXCREATOR_H

#include "../geodata/geodatacreator.h"
#include "../../solverdef/solverdefinitiongridcomplexattribute.h"

#include <vtkIntArray.h>

class GeoDataBackgroundComplexCreator : public GeoDataCreator
{
	Q_OBJECT

private:
	GeoDataBackgroundComplexCreator();

public:
	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;
	QString defaultCaption(unsigned int) const override;

	static GeoDataBackgroundComplexCreator* instance();

private:
	static GeoDataBackgroundComplexCreator* m_instance;
};

#endif // GEODATABACKGROUNDCOMPLEXCREATOR_H
