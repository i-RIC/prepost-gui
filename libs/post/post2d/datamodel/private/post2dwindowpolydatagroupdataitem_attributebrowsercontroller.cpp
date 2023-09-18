#include "post2dwindowpolydatagroupdataitem_attributebrowsercontroller.h"

#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/grid/v4polydata2d.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <misc/mathsupport.h>

Post2dWindowPolyDataGroupDataItem::AttributeBrowserController::AttributeBrowserController(Post2dWindowPolyDataGroupDataItem* item) :
	Post2dWindowAttributeBrowserController {item}
{}

void Post2dWindowPolyDataGroupDataItem::AttributeBrowserController::initialize()
{
	propertyBrowserView()->resetForPolyData();
}

bool Post2dWindowPolyDataGroupDataItem::AttributeBrowserController::doUpdate(const QPoint& p, VTKGraphicsView* v)
{
	auto polyData = dynamic_cast<Post2dWindowPolyDataGroupDataItem*>(item())->polyData();
	if (polyData == nullptr) {return false;}

	double weights[4];
	auto pos = worldPos(p, v);
	auto cellId = polyData->vtkConcreteData()->findCell(pos.x(), pos.y(), 0, v->stdDistance(iRIC::nearRadius()), weights);
	if (cellId < 0) {
		propertyBrowserView()->resetAttributes();
		return false;
	}
	auto attributes = buildAttributes(cellId, polyData->vtkData()->data()->GetCellData(), polyData->attributeDataProvider());
	vtkIdType origCellId = polyData->cellIds().at(cellId);
	const auto& cellIds = polyData->cellIds();
	auto b = std::lower_bound(cellIds.begin(), cellIds.end(), origCellId);
	auto e = std::upper_bound(cellIds.begin(), cellIds.end(), origCellId);

	std::vector<QPolygonF> polygons;

	for (auto it = b; it != e; ++it) {
		auto cellId = it - cellIds.begin();
		auto cell = polyData->vtkData()->data()->GetCell(cellId);
		if (dynamic_cast<vtkLine*> (cell) != nullptr || dynamic_cast<vtkPolyLine*> (cell) != nullptr) {
			auto polyline = vtkPointsUtil::getCoordinates(cell);
			propertyBrowserView()->setPolyDataAttributes(polyline, origCellId, attributes);
			return true;
		} else {
			polygons.push_back(vtkPointsUtil::getPolygon(cell));
		}
	}

	propertyBrowserView()->setPolyDataAttributes(polygons, origCellId, attributes);
	return true;
}
