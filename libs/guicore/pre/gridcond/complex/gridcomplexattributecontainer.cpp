#include "gridcomplexattributecontainer.h"

#include "../../../solverdef/solverdefinitiongridcomplexattribute.h"
#include "../../../project/projectcgnsfile.h"
#include "../../base/preprocessorgeodatacomplexgroupdataiteminterface.h"
#include "../../base/preprocessorgeodatagroupdataiteminterface.h"
#include "../../base/preprocessorgeodatatopdataiteminterface.h"
#include "../../base/preprocessorgriddataiteminterface.h"
#include "../../base/preprocessorgridtypedataiteminterface.h"
#include "../../grid/grid.h"
#include <misc/stringtool.h>

#include <cgnslib.h>
#include <vtkDataArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPointSet.h>
#include <vtkCellData.h>

GridComplexAttributeContainer::GridComplexAttributeContainer(Grid* grid, SolverDefinitionGridComplexAttribute* cond) :
	GridAttributeContainerT<int> {grid, cond}
{}

GridComplexAttributeContainer::~GridComplexAttributeContainer()
{}

int GridComplexAttributeContainer::value(unsigned int index) const
{
	return dataArray()->GetValue(index);
}

void GridComplexAttributeContainer::setValue(unsigned int index, int value)
{
	dataArray()->SetValue(index, value);
}

void GridComplexAttributeContainer::allocate()
{
	vtkIntArray* da = dataArray();
	if (da == nullptr) {
		// not found maybe reset?
		da = vtkIntArray::New();
		da->SetName(name().c_str());
		if (gridAttribute()->position() == SolverDefinitionGridAttribute::Node) {
			grid()->vtkGrid()->GetPointData()->AddArray(da);
		} else {
			grid()->vtkGrid()->GetCellData()->AddArray(da);
		}
		da->Delete();
	}
	da->Allocate(dataCount());
	int count = dataCount();
	for (int i = 0; i < count; ++i) {
		da->InsertValue(i, 0);
	}
}

vtkIntArray* GridComplexAttributeContainer::dataArray() const
{
	vtkPointSet* ps = const_cast<vtkPointSet*> (GridAttributeContainer::grid()->vtkGrid());
	vtkDataArray* da = nullptr;
	if (gridAttribute()->position() == SolverDefinitionGridAttribute::Node) {
		da = ps->GetPointData()->GetArray(GridComplexAttributeContainer::name().c_str());
	} else {
		da = ps->GetCellData()->GetArray(GridComplexAttributeContainer::name().c_str());
	}
	return vtkIntArray::SafeDownCast(da);
}

vtkIntArray* GridComplexAttributeContainer::dataArrayCopy() const
{
	vtkIntArray* copy = vtkIntArray::New();
	copy->DeepCopy(dataArray());
	return copy;
}

unsigned int GridComplexAttributeContainer::dataCount() const
{
	if (gridAttribute()->position() == SolverDefinitionGridAttribute::Node) {
		return grid()->nodeCount();
	} else {
		return grid()->cellCount();
	}
}

void GridComplexAttributeContainer::setModified()
{
	dataArray()->Modified();
}

bool GridComplexAttributeContainer::getValueRange(double* min, double* max)
{
	double range[2];
	dataArray()->GetRange(range);
	*min = range[0];
	*max = range[1];
	return true;
}

std::vector<GridComplexConditionGroup*> GridComplexAttributeContainer::groups() const
{
	std::vector<GridComplexConditionGroup*> empty;
	auto gItem = grid()->dataItem();
	if (gItem == nullptr) {return empty;}

	auto gTypeItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*> (gItem->parent()->parent());
	if (gTypeItem == nullptr) {return empty;}
	auto geoTopItem = gTypeItem->geoDataTop();
	if (geoTopItem == nullptr) {return empty;}
	auto gItem2 = geoTopItem->groupDataItem(gridAttribute()->name());
	if (gItem2 == nullptr) {return empty;}
	auto gItem3 = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemInterface*> (gItem2);
	if (gItem3 == nullptr) {return empty;}

	return gItem3->groups();
}

DataType_t GridComplexAttributeContainer::dataType() const
{
	return Integer;
}

SolverDefinitionGridComplexAttribute* GridComplexAttributeContainer::gridAttribute() const
{
	return dynamic_cast<SolverDefinitionGridComplexAttribute*>(GridAttributeBaseObject::gridAttribute());
}
