#ifndef GRIDCREATINGCONDITIONIO_H
#define GRIDCREATINGCONDITIONIO_H

#include "../../guicore_global.h"

class GridCreatingCondition;
class GridCreatingConditionFactoryI;
class ProjectDataItem;

class QString;

class GUICOREDLL_EXPORT GridCreatingConditionIO
{
public:
	static GridCreatingCondition* importData(GridCreatingConditionFactoryI* factory, ProjectDataItem* parent, const QString& fileName, const QString& workFolder);
	static void exportData(GridCreatingCondition* cond, const QString& fileName, const QString& workFolder);

private:
	GridCreatingConditionIO();
};

#endif // GRIDCREATINGCONDITIONIO_H
