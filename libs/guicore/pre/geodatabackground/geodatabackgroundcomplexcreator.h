#ifndef GEODATABACKGROUNDCOMPLEXCREATOR_H
#define GEODATABACKGROUNDCOMPLEXCREATOR_H

#include "../geodata/geodatacreator.h"
#include "../../solverdef/solverdefinitiongridcomplexattribute.h"

#include <vtkIntArray.h>

class GeoDataBackgroundComplexCreator : public GeoDataCreator
{

private:
	/// Constructor
	GeoDataBackgroundComplexCreator();

public:
	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	bool isCompatibleWith(SolverDefinitionGridAttribute* /* condition*/) override;
	QString defaultCaption(unsigned int /*index*/) override {return "";}

	static GeoDataBackgroundComplexCreator* instance();

protected:
	static GeoDataBackgroundComplexCreator* m_instance;

};

#endif // GEODATABACKGROUNDCOMPLEXCREATOR_H
