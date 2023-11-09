#include "post2dwindowjedgescalargroupdataitem.h"
#include "post2dwindowjedgescalargrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowJEdgeScalarGroupTopDataItem::Post2dWindowJEdgeScalarGroupTopDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowAbstractCellScalarGroupTopDataItem {tr("Scalar (edgeJ)"), parent}
{}

void Post2dWindowJEdgeScalarGroupTopDataItem::getIJIndex(vtkIdType idx, vtkIdType* i, vtkIdType* j) const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return;}

	auto grid = cont->gridData()->grid();
	if (grid == nullptr) {return;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*> (grid);
	sGrid->getJEdgeIJIndex(idx, i, j);
}

vtkPointSetExtended* Post2dWindowJEdgeScalarGroupTopDataItem::data() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	auto grid = cont->gridData();
	if (grid == nullptr) {return nullptr;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*>(grid->grid());
	return sGrid->vtkJEdgeData();
}

Post2dWindowAbstractCellScalarGroupDataItem* Post2dWindowJEdgeScalarGroupTopDataItem::createChild(const std::string& target)
{
	return new Post2dWindowJEdgeScalarGroupDataItem(target, this);
}
