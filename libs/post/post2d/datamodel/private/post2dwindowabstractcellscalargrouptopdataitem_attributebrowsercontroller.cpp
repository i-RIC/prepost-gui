#include "../post2dwindowcalculationresultdataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindowabstractcellscalargrouptopdataitem_attributebrowsercontroller.h"

#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/mathsupport.h>

Post2dWindowAbstractCellScalarGroupTopDataItem::AttributeBrowserController::AttributeBrowserController(Post2dWindowAbstractCellScalarGroupTopDataItem* item) :
	Post2dWindowAttributeBrowserController {item}
{}

void Post2dWindowAbstractCellScalarGroupTopDataItem::AttributeBrowserController::initialize()
{
	auto cont = dataContainer();
	if (cont == nullptr) {return;}

	auto grid = cont->gridData()->grid();
	auto sGrid = dynamic_cast<v4Structured2dGrid*>(grid);
	if (sGrid != nullptr) {
		propertyBrowserView()->resetForCell(true);
	} else {
		propertyBrowserView()->resetForCell(false);
	}
}

bool Post2dWindowAbstractCellScalarGroupTopDataItem::AttributeBrowserController::doUpdate(const QPoint& p, VTKGraphicsView* v)
{
	auto topDataItem = dynamic_cast<Post2dWindowAbstractCellScalarGroupTopDataItem*> (item());
	auto cont = dataContainer();
	if (cont == nullptr) {return false;}

	double weights[4];
	auto pos = worldPos(p, v);
	auto grid2d = dynamic_cast<v4Grid2d*> (cont->gridData()->grid());
	auto cellId = topDataItem->data()->findCell(pos.x(), pos.y(), 0, v->stdDistance(iRIC::nearRadius()), weights);
	if (cellId < 0) {
		propertyBrowserView()->resetAttributes();
		return false;
	}

	auto polygon = vtkPointsUtil::getPolygon(topDataItem->data()->data()->GetCell(cellId));
	auto attributes = buildAttributes(cellId, topDataItem->data()->data()->GetCellData(), grid2d->attributeDataProvider());

	auto sGrid = dynamic_cast<v4Structured2dGrid*>(grid2d);
	if (sGrid != nullptr) {
		vtkIdType i, j;
		topDataItem->getIJIndex(cellId, &i, &j);
		propertyBrowserView()->setCellAttributes(i, j, polygon, attributes);
	} else {
		propertyBrowserView()->setCellAttributes(cellId, polygon, attributes);
	}
	return true;
}

v4PostZoneDataContainer* Post2dWindowAbstractCellScalarGroupTopDataItem::AttributeBrowserController::dataContainer() const
{
	auto i = dynamic_cast<Post2dWindowAbstractCellScalarGroupTopDataItem*> (item());
	return i->resultDataItem()->zoneDataItem()->v4DataContainer();
}
