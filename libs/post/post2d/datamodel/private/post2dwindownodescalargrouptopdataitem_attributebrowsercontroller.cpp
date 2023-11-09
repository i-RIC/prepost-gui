#include "../post2dwindowcalculationresultdataitem.h"
#include "../post2dwindownodescalargrouptopdataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodescalargrouptopdataitem_attributebrowsercontroller.h"

#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/mathsupport.h>

Post2dWindowNodeScalarGroupTopDataItem::AttributeBrowserController::AttributeBrowserController(Post2dWindowNodeScalarGroupTopDataItem* item) :
	Post2dWindowAttributeBrowserController {item}
{}

void Post2dWindowNodeScalarGroupTopDataItem::AttributeBrowserController::initialize()
{
	auto cont = dataContainer();
	if (cont == nullptr) {return;}

	auto grid = cont->gridData()->grid();
	auto sGrid = dynamic_cast<v4Structured2dGrid*>(grid);
	if (sGrid != nullptr) {
		propertyBrowserView()->resetForVertex(true);
	} else {
		propertyBrowserView()->resetForVertex(false);
	}
}

bool Post2dWindowNodeScalarGroupTopDataItem::AttributeBrowserController::doUpdate(const QPoint& p, VTKGraphicsView* v)
{
	auto cont = dataContainer();
	if (cont == nullptr) {return false;}

	auto pos = worldPos(p, v);
	auto grid2d = dynamic_cast<v4Grid2d*> (cont->gridData()->grid());
	auto pointId = grid2d->vtkData()->findClosestPointWithinRadius(v->stdDistance(iRIC::nearRadius()), pos.x(), pos.y(), 0);
	if (pointId < 0) {
		propertyBrowserView()->resetAttributes();
		return false;
	}

	auto pointCoords = grid2d->point2d(pointId);
	auto attributes = buildAttributes(pointId, grid2d->vtkData()->data()->GetPointData(), grid2d->attributeDataProvider());

	auto sGrid = dynamic_cast<v4Structured2dGrid*>(grid2d);
	if (sGrid != nullptr) {
		vtkIdType i, j;
		sGrid->getPointIJIndex(pointId, &i, &j);
		propertyBrowserView()->setVertexAttributes(i, j, pointCoords.x(), pointCoords.y(), attributes);
	} else {
		propertyBrowserView()->setVertexAttributes(pointId, pointCoords.x(), pointCoords.y(), attributes);
	}
	return true;
}

v4PostZoneDataContainer* Post2dWindowNodeScalarGroupTopDataItem::AttributeBrowserController::dataContainer() const
{
	auto i = dynamic_cast<Post2dWindowNodeScalarGroupTopDataItem*> (item());
	return i->resultDataItem()->zoneDataItem()->v4DataContainer();
}
