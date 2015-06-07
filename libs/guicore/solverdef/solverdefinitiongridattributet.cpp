#include "../pre/rawdatabackground/rawdatabackgroundintegercreator.h"
#include "../pre/rawdatabackground/rawdatabackgroundrealcreator.h"
#include "solverdefinitiongridattributet.h"

RawData* SolverDefinitionGridAttributeInteger::buildBackgroundRawData(ProjectDataItem* parent)
{
	return RawDataBackgroundIntegerCreator::instance()->create(parent, this);
}

RawData* SolverDefinitionGridAttributeReal::buildBackgroundRawData(ProjectDataItem* parent)
{
	return RawDataBackgroundRealCreator::instance()->create(parent, this);
}
