#include "gridcomplexattributecontainer.h"

#include "../../../solverdef/solverdefinitiongridcomplexattribute.h"
#include "../../../project/projectcgnsfile.h"
#include "../../base/preprocessorgeodatacomplexgroupdataitemi.h"
#include "../../base/preprocessorgeodatagroupdataitemi.h"
#include "../../base/preprocessorgeodatatopdataitemi.h"
#include "../../base/preprocessorgriddataitemi.h"
#include "../../base/preprocessorgridtypedataitemi.h"
#include "../../complex/gridcomplexconditiongroup.h"
#include "../../grid/v4inputgrid.h"
#include "../../../grid/v4grid.h"
#include "../../../grid/v4structured2dgrid.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <misc/stringtool.h>

#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPointSet.h>

GridComplexAttributeContainer::GridComplexAttributeContainer(v4InputGrid* grid, GridAttributeContainerIoI* io, SolverDefinitionGridComplexAttribute* cond) :
	GridAttributeContainerT<int, vtkIntArray> {grid, io, cond}
{}

GridComplexAttributeContainer::~GridComplexAttributeContainer()
{}

std::vector<GridComplexConditionGroup*> GridComplexAttributeContainer::groups() const
{
	std::vector<GridComplexConditionGroup*> empty;
	auto gItem = grid()->gridDataItem();
	if (gItem == nullptr) {return empty;}

	auto gTypeItem = dynamic_cast<PreProcessorGridTypeDataItemI*> (gItem->parent()->parent());
	if (gTypeItem == nullptr) {return empty;}
	auto geoTopItem = gTypeItem->geoDataTop();
	if (geoTopItem == nullptr) {return empty;}
	auto gItem2 = geoTopItem->groupDataItem(gridAttribute()->name());
	if (gItem2 == nullptr) {return empty;}
	auto gItem3 = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemI*> (gItem2);
	if (gItem3 == nullptr) {return empty;}

	return gItem3->groups();
}

void GridComplexAttributeContainer::fixComplexValuesToDefaultIfInvalid()
{
	int defaultValue = 1;
	auto glist = groups();
	int maxValue = glist.size();
	for (int i = 0; i < glist.size(); ++i) {
		auto g = glist.at(i);
		if (g->isDefault()) {
			defaultValue = i + 1;
		}
	}
	for (unsigned int i = 0; i < dataCount(); ++i) {
		int current = value(i);
		if (current < 1 || current > maxValue) {
			setValue(i, defaultValue);
		}
	}
}

SolverDefinitionGridComplexAttribute* GridComplexAttributeContainer::gridAttribute() const
{
	return dynamic_cast<SolverDefinitionGridComplexAttribute*>(GridAttributeBaseObject::gridAttribute());
}
