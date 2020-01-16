#include "gridcreatingconditioncenterandwidth_coordinateseditor.h"
#include "gridcreatingconditioncenterandwidth_editcoordinatescommand.h"
#include "../gridcreatingconditioncenterandwidth.h"

#include <guibase/coordinates/coordinatesedit.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>

GridCreatingConditionCenterAndWidth* GridCreatingConditionCenterAndWidth::CoordinatesEditor::targetCond = 0;
bool GridCreatingConditionCenterAndWidth::CoordinatesEditor::applied = false;

void GridCreatingConditionCenterAndWidth::CoordinatesEditor::edit(GridCreatingConditionCenterAndWidth* cond)
{
	cond->m_mouseEventMode = meEditVerticesDialog;
	cond->iricMainWindow()->enterModelessDialogMode();

	targetCond = cond;
	applied = false;

	auto coords = applyOffset(cond->polyLine());

	CoordinatesEdit::edit(coords, cond->preProcessorWindow(),
												acceptCallback,
												rejectCallback,
												applyCallback,
												GridCreatingConditionCenterAndWidth::tr("Polyline Coordinates"));
}

void GridCreatingConditionCenterAndWidth::CoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	targetCond->pushUpdateShapeCommand(new GridCreatingConditionCenterAndWidth::EditCoordinatesCommand(coords2, targetCond));

	targetCond->restoreMouseEventMode();
	targetCond->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionCenterAndWidth::CoordinatesEditor::rejectCallback()
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
		targetCond->renderGraphicsView();
	}
	targetCond->restoreMouseEventMode();
	targetCond->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionCenterAndWidth::CoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	targetCond->pushUpdateShapeCommand(new GridCreatingConditionCenterAndWidth::EditCoordinatesCommand(coords2, targetCond), true);

	applied = true;
}

std::vector<QPointF> GridCreatingConditionCenterAndWidth::CoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetCond->offset());
}

std::vector<QPointF> GridCreatingConditionCenterAndWidth::CoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetCond->offset());
}
