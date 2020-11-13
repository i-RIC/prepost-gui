#include "geodatapoint_coordinateseditor.h"
#include "geodatapoint_editcoordinatescommand.h"

#include <guibase/coordinates/coordinatesedit.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>

GeoDataPoint* GeoDataPoint::CoordinatesEditor::targetPoint = nullptr;
bool GeoDataPoint::CoordinatesEditor::applied = false;

void GeoDataPoint::CoordinatesEditor::edit(GeoDataPoint* point)
{
	point->setMouseEventMode(meEditCoordinatesDialog);
	point->iricMainWindow()->enterModelessDialogMode();

	targetPoint = point;
	applied = false;

	QPointF p = point->point();
	std::vector<QPointF> coords;
	coords.push_back(p);
	auto coords2 = applyOffset(coords);

	CoordinatesEdit::edit(coords2, point->preProcessorWindow(),
												acceptCallback,
												rejectCallback,
												applyCallback,
												GeoDataPoint::tr("Point Coordinates"));
}

void GeoDataPoint::CoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);

	iRICUndoStack::instance().push(new GeoDataPoint::EditCoordinatesCommand(false, coords2.at(0), targetPoint));

	targetPoint->restoreMouseEventMode();
	targetPoint->iricMainWindow()->exitModelessDialogMode();
}

void GeoDataPoint::CoordinatesEditor::rejectCallback()
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
		targetPoint->renderGraphicsView();
	}
	targetPoint->restoreMouseEventMode();
	targetPoint->iricMainWindow()->exitModelessDialogMode();
}

void GeoDataPoint::CoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GeoDataPoint::EditCoordinatesCommand(true, coords2.at(0), targetPoint));

	applied = true;
}

std::vector<QPointF> GeoDataPoint::CoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetPoint->offset());
}

std::vector<QPointF> GeoDataPoint::CoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetPoint->offset());
}
