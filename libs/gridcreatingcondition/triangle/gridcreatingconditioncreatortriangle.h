#ifndef GRIDCREATINGCONDITIONCREATORTRIANGLE_H
#define GRIDCREATINGCONDITIONCREATORTRIANGLE_H

#include "gcc_triangle_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_TRIANGLE_EXPORT GridCreatingConditionCreatorTriangle : public GridCreatingConditionCreator
{
	Q_OBJECT
public:
	GridCreatingConditionCreatorTriangle();
	~GridCreatingConditionCreatorTriangle() {}
	// temporary
	SolverDefinitionGridType::GridType gridType() const {return SolverDefinitionGridType::gtUnstructured2DGrid;}
	GridCreatingCondition* create(ProjectDataItem* parent);
};

#endif // GRIDCREATINGCONDITIONCREATORTRIANGLE_H
