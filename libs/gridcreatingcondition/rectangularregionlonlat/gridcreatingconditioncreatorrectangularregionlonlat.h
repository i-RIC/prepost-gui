#ifndef GRIDCREATINGCONDITIONCREATORRECTANGULARREGIONLONLAT_H
#define GRIDCREATINGCONDITIONCREATORRECTANGULARREGIONLONLAT_H

#include "gcc_rectangularregionlonlat_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_RECTANGULARREGIONLONLAT_EXPORT GridCreatingConditionCreatorRectangularRegionLonLat : public GridCreatingConditionCreator
{
	Q_OBJECT

public:
	GridCreatingConditionCreatorRectangularRegionLonLat();
	~GridCreatingConditionCreatorRectangularRegionLonLat() {}
	SolverDefinitionGridType::GridType gridType() const override {return SolverDefinitionGridType::gtStructured2DGrid;}
	GridCreatingCondition* create(ProjectDataItem* parent) override;
};

#endif // GRIDCREATINGCONDITIONCREATORRECTANGULARREGIONLONLAT_H
