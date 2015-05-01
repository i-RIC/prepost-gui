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
	SolverDefinitionGridType::GridType gridType() const override {return SolverDefinitionGridType::gtUnstructured2DGrid;}
	GridCreatingCondition* create(ProjectDataItem* parent) override;
};

#endif // GRIDCREATINGCONDITIONCREATORTRIANGLE_H
