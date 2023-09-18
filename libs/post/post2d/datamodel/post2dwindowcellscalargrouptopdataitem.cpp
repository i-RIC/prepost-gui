#include "post2dwindowcellscalargroupdataitem.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowCellScalarGroupTopDataItem::Post2dWindowCellScalarGroupTopDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowAbstractCellScalarGroupTopDataItem {tr("Scalar (cell center)"), parent}
{}

void Post2dWindowCellScalarGroupTopDataItem::getIJIndex(vtkIdType idx, vtkIdType* i, vtkIdType* j) const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return;}

	auto grid = cont->gridData()->grid();
	if (grid == nullptr) {return;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*> (grid);
	sGrid->getCellIJIndex(idx, i, j);
}

vtkPointSetExtended* Post2dWindowCellScalarGroupTopDataItem::data() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	auto grid = cont->gridData();
	if (grid == nullptr) {return nullptr;}

	return grid->grid()->vtkData();
}

Post2dWindowAbstractCellScalarGroupDataItem* Post2dWindowCellScalarGroupTopDataItem::createChild(const std::string& target)
{
	return new Post2dWindowCellScalarGroupDataItem(target, this);
}
