#include "post2dwindowattributebrowsercontroller.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowjedgescalargroupdataitem.h"
#include "post2dwindowjedgescalargrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowJEdgeScalarGroupTopDataItem::Post2dWindowJEdgeScalarGroupTopDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowAbstractCellScalarGroupTopDataItem {tr("Scalar (edgeJ)"), parent}
{}

void Post2dWindowJEdgeScalarGroupTopDataItem::openGraphWindow()
{
	auto index = attributeBrowserController()->fixedIndex();
	std::vector<std::string> vals;
	for (const auto& child : m_childItems) {
		const auto& item = dynamic_cast<const Post2dWindowJEdgeScalarGroupDataItem*> (child);
		if (item->standardItem()->checkState() == Qt::Checked) {
			vals.push_back(item->target());
		}
	}

	auto zItem = resultDataItem()->zoneDataItem();
	iricMainWindow()->createGraph2dHybridWindowForJFace(zItem->zoneName(), index, vals);
}

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
