#include "solverdefinitiongridrelatedconditiont.h"
#include "../pre/rawdatabackground/rawdatabackgroundintegercreator.h"
#include "../pre/rawdatabackground/rawdatabackgroundrealcreator.h"

RawData* SolverDefinitionGridRelatedIntegerCondition::buildBackgroundRawData(ProjectDataItem* parent)
{
	return RawDataBackgroundIntegerCreator::instance()->create(parent, this);
}

RawData* SolverDefinitionGridRelatedRealCondition::buildBackgroundRawData(ProjectDataItem* parent)
{
	return RawDataBackgroundRealCreator::instance()->create(parent, this);
}
