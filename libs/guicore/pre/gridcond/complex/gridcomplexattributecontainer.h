#ifndef GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
#define GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H

#include "../../../guicore_global.h"
#include "../base/gridattributecontainert.h"

#include <vtkIntArray.h>

class GridComplexConditionGroup;
class SolverDefinitionGridComplexAttribute;

class PreProcessorGeoDataComplexGroupDataItemI;

class GUICOREDLL_EXPORT GridComplexAttributeContainer : public GridAttributeContainerT<int, vtkIntArray>
{
public:
	GridComplexAttributeContainer(v4InputGrid* grid, SolverDefinitionGridComplexAttribute* cond);
	~GridComplexAttributeContainer();

	void setDataItem(PreProcessorGeoDataComplexGroupDataItemI* dataItem);
	std::vector<GridComplexConditionGroup*> groups() const;

private:
	SolverDefinitionGridComplexAttribute* gridAttribute() const;

	PreProcessorGeoDataComplexGroupDataItemI* m_dataItem;
};

#endif // GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
