#include "post2dwindowabstractcellscalargrouptopdataitem.h"
#include "post2dwindowjedgescalargroupdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowabstractcellscalargroupdataitem_impl.h"
#include "private/post2dwindowabstractcellscalargroupdataitem_setting.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowJEdgeScalarGroupDataItem::Post2dWindowJEdgeScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* parent) :
	Post2dWindowAbstractCellScalarGroupDataItem {target, parent}
{}

void Post2dWindowJEdgeScalarGroupDataItem::getDimensions(int* dimI, int* dimJ) const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (topDataItem()->zoneDataItem()->v4DataContainer()->gridData()->grid());
	*dimI = sGrid->dimensionI() - 1;
	*dimJ = sGrid->dimensionJ();
}

v4SolutionGrid::Position Post2dWindowJEdgeScalarGroupDataItem::position() const
{
	return v4SolutionGrid::Position::JFace;
}

vtkPointSet* Post2dWindowJEdgeScalarGroupDataItem::buildRegionFilteredData() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (topDataItem()->zoneDataItem()->v4DataContainer()->gridData()->grid());
	return impl->m_setting.regionSetting.buildJEdgeFilteredData(sGrid);
}

const ValueRangeContainer& Post2dWindowJEdgeScalarGroupDataItem::valueRange(const::std::string& name) const
{
	return topDataItem()->zoneDataItem()->gridTypeDataItem()->jEdgeValueRange(name);
}

vtkPointSetExtended* Post2dWindowJEdgeScalarGroupDataItem::data() const
{
	auto cont = topDataItem()->zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	auto grid = cont->gridData();
	if (grid == nullptr) {return nullptr;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*> (grid->grid());
	return sGrid->vtkJEdgeData();
}

vtkPointSet* Post2dWindowJEdgeScalarGroupDataItem::filteredData() const
{
	auto cont = topDataItem()->zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	auto grid = cont->gridData();
	if (grid == nullptr) {return nullptr;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*> (grid->grid());
	return sGrid->vtkJEdgeFilteredData();
}
