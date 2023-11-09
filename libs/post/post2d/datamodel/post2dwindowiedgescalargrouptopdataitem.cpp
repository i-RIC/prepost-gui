#include "post2dwindowiedgescalargroupdataitem.h"
#include "post2dwindowiedgescalargrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowIEdgeScalarGroupTopDataItem::Post2dWindowIEdgeScalarGroupTopDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowAbstractCellScalarGroupTopDataItem {tr("Scalar (edgeI)"), parent}
{}

void Post2dWindowIEdgeScalarGroupTopDataItem::getIJIndex(vtkIdType idx, vtkIdType* i, vtkIdType* j) const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return;}

	auto grid = cont->gridData()->grid();
	if (grid == nullptr) {return;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*> (grid);
	sGrid->getIEdgeIJIndex(idx, i, j);
}

vtkPointSetExtended* Post2dWindowIEdgeScalarGroupTopDataItem::data() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	auto grid = cont->gridData();
	if (grid == nullptr) {return nullptr;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*>(grid->grid());
	return sGrid->vtkIEdgeData();
}

Post2dWindowAbstractCellScalarGroupDataItem* Post2dWindowIEdgeScalarGroupTopDataItem::createChild(const std::string& target)
{
	return new Post2dWindowIEdgeScalarGroupDataItem(target, this);
}
