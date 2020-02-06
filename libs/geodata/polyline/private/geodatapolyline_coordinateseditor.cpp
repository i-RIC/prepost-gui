#include "geodatapolyline_coordinateseditor.h"
#include "geodatapolyline_editcoordinatescommand.h"
#include "../geodatapolylineimplpolyline.h"

#include <guibase/coordinates/coordinatesedit.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>

#include <vtkPolyLine.h>

GeoDataPolyLine* GeoDataPolyLine::CoordinatesEditor::targetPolyLine = nullptr;
bool GeoDataPolyLine::CoordinatesEditor::applied = false;

void GeoDataPolyLine::CoordinatesEditor::edit(GeoDataPolyLine* polyline)
{
	polyline->setMouseEventMode(meEditVerticesDialog);
	polyline->iricMainWindow()->enterModelessDialogMode();

	targetPolyLine = polyline;
	applied = false;

	auto coords = polyline->polyLine()->polyLine();

	CoordinatesEdit::edit(coords, polyline->preProcessorWindow(),
												acceptCallback,
												rejectCallback,
												applyCallback,
												GeoDataPolyLine::tr("PolyLine Coordinates"));
}

void GeoDataPolyLine::CoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GeoDataPolyLine::EditCoordinatesCommand(false, coords2, targetPolyLine));

	targetPolyLine->restoreMouseEventMode();
	targetPolyLine->iricMainWindow()->exitModelessDialogMode();
}

void GeoDataPolyLine::CoordinatesEditor::rejectCallback()
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
		targetPolyLine->renderGraphicsView();
	}
	targetPolyLine->restoreMouseEventMode();
	targetPolyLine->iricMainWindow()->exitModelessDialogMode();
}

void GeoDataPolyLine::CoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GeoDataPolyLine::EditCoordinatesCommand(true, coords2, targetPolyLine));

	applied = true;
}

std::vector<QPointF> GeoDataPolyLine::CoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetPolyLine->offset());
}

std::vector<QPointF> GeoDataPolyLine::CoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetPolyLine->offset());
}
