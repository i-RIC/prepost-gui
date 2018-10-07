#include "geodatapolygon_coordinateseditor.h"
#include "geodatapolygon_editcoordinatescommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <geoio/coordinatesedit.h>
#include <misc/iricundostack.h>

#include <vtkPolygon.h>

GeoDataPolygon* GeoDataPolygon::CoordinatesEditor::targetPolygon = 0;
bool GeoDataPolygon::CoordinatesEditor::applied = false;

void GeoDataPolygon::CoordinatesEditor::edit(GeoDataPolygon* polygon)
{
	polygon->setMouseEventMode(meEditVerticesDialog);
	polygon->iricMainWindow()->enterModelessDialogMode();

	targetPolygon = polygon;
	applied = false;

	auto coords = vtkPointsUtil::getCoordinates(polygon->selectedPolygon()->getVtkPolygon());
	auto coords2 = applyOffset(coords);

	CoordinatesEdit::edit(coords2, polygon->preProcessorWindow(),
												acceptCallback,
												rejectCallback,
												applyCallback,
												GeoDataPolygon::tr("Polygon Coordinates"));
}

void GeoDataPolygon::CoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GeoDataPolygon::EditCoordinatesCommand(false, coords2, targetPolygon));

	targetPolygon->restoreMouseEventMode();
	targetPolygon->iricMainWindow()->exitModelessDialogMode();
	targetPolygon->updatePolyData();
}

void GeoDataPolygon::CoordinatesEditor::rejectCallback()
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
		targetPolygon->renderGraphicsView();
		targetPolygon->updatePolyData();
	}
	targetPolygon->restoreMouseEventMode();
	targetPolygon->iricMainWindow()->exitModelessDialogMode();
}

void GeoDataPolygon::CoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GeoDataPolygon::EditCoordinatesCommand(true, coords2, targetPolygon));

	targetPolygon->updatePolyData();

	applied = true;
}

std::vector<QPointF> GeoDataPolygon::CoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetPolygon->offset());
}

std::vector<QPointF> GeoDataPolygon::CoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetPolygon->offset());
}
