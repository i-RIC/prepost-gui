#include "gridcreatingconditiontriangle_polylinecoordinateseditor.h"
#include "gridcreatingconditiontriangle_editpolylinecoordinatescommand.h"
#include "../gridcreatingconditiontriangle.h"
#include "../gridcreatingconditiontriangleabstractline.h"

#include <guibase/coordinates/coordinatesedit.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>

GridCreatingConditionTriangle* GridCreatingConditionTriangle::PolylineCoordinatesEditor::targetCondition = 0;
bool GridCreatingConditionTriangle::PolylineCoordinatesEditor::applied = false;

void GridCreatingConditionTriangle::PolylineCoordinatesEditor::edit(GridCreatingConditionTriangle * condition)
{
	condition->m_mouseEventMode = meEditVerticesDialog;
	condition->iricMainWindow()->enterModelessDialogMode();

	targetCondition = condition;
	applied = false;

	auto coords = vtkPointsUtil::getCoordinates(condition->m_selectedLine->getVtkLine());
	auto coords2 = applyOffset(coords);
	CoordinatesEdit::edit(coords2, condition->preProcessorWindow(),
												acceptCallback,
												rejectCallback,
												applyCallback,
												GridCreatingConditionTriangle::tr("Break Line Coordinates"));
}

void GridCreatingConditionTriangle::PolylineCoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GridCreatingConditionTriangle::EditPolyLineCoordinatesCommand(false, coords2, targetCondition));

	targetCondition->restoreMouseEventMode();
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionTriangle::PolylineCoordinatesEditor::rejectCallback()
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
		targetCondition->m_selectedLine->updateShapeData();
		targetCondition->renderGraphicsView();
	}
	targetCondition->restoreMouseEventMode();
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionTriangle::PolylineCoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GridCreatingConditionTriangle::EditPolyLineCoordinatesCommand(true, coords2, targetCondition));

	applied = true;
}

std::vector<QPointF> GridCreatingConditionTriangle::PolylineCoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetCondition->offset());
}

std::vector<QPointF> GridCreatingConditionTriangle::PolylineCoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetCondition->offset());
}
