#include "post2dwindowabstractcellscalargrouptopdataitem.h"
#include "post2dwindowiedgescalargroupdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowabstractcellscalargroupdataitem_impl.h"
#include "private/post2dwindowabstractcellscalargroupdataitem_setting.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowIEdgeScalarGroupDataItem::Post2dWindowIEdgeScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* parent) :
	Post2dWindowAbstractCellScalarGroupDataItem {target, parent}
{}

void Post2dWindowIEdgeScalarGroupDataItem::getDimensions(int* dimI, int* dimJ) const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (topDataItem()->zoneDataItem()->v4DataContainer()->gridData()->grid());
	*dimI = sGrid->dimensionI();
	*dimJ = sGrid->dimensionJ() - 1;
}

v4SolutionGrid::Position Post2dWindowIEdgeScalarGroupDataItem::position() const
{
	return v4SolutionGrid::Position::IFace;
}

vtkPointSet* Post2dWindowIEdgeScalarGroupDataItem::buildRegionFilteredData() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (topDataItem()->zoneDataItem()->v4DataContainer()->gridData()->grid());
	return impl->m_setting.regionSetting.buildIEdgeFilteredData(sGrid);
}

const ValueRangeContainer& Post2dWindowIEdgeScalarGroupDataItem::valueRange(const::std::string& name) const
{
	return topDataItem()->zoneDataItem()->gridTypeDataItem()->iEdgeValueRange(name);
}

vtkPointSetExtended* Post2dWindowIEdgeScalarGroupDataItem::data() const
{
	auto cont = topDataItem()->zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	auto grid = cont->gridData();
	if (grid == nullptr) {return nullptr;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*> (grid->grid());
	return sGrid->vtkIEdgeData();
}

vtkPointSet* Post2dWindowIEdgeScalarGroupDataItem::filteredData() const
{
	auto cont = topDataItem()->zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	auto grid = cont->gridData();
	if (grid == nullptr) {return nullptr;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*> (grid->grid());
	return sGrid->vtkIEdgeFilteredData();
}
