#include "gridcreatingconditiontriangle_polygoncoordinateseditor.h"
#include "gridcreatingconditiontriangle_editpolygoncoordinatescommand.h"
#include "../gridcreatingconditiontriangle.h"
#include "../gridcreatingconditiontriangleabstractpolygon.h"

#include <guibase/coordinates/coordinatesedit.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>

GridCreatingConditionTriangle* GridCreatingConditionTriangle::PolygonCoordinatesEditor::targetCondition = 0;
bool GridCreatingConditionTriangle::PolygonCoordinatesEditor::applied = false;

void GridCreatingConditionTriangle::PolygonCoordinatesEditor::edit(GridCreatingConditionTriangle * condition)
{
	condition->m_mouseEventMode = meEditVerticesDialog;
	condition->iricMainWindow()->enterModelessDialogMode();

	targetCondition = condition;
	applied = false;

	auto coords = vtkPointsUtil::getCoordinates(condition->m_selectedPolygon->getVtkPolygon());
	auto coords2 = applyOffset(coords);
	CoordinatesEdit::edit(coords2, condition->preProcessorWindow(),
												acceptCallback,
												rejectCallback,
												applyCallback,
												GridCreatingConditionTriangle::tr("Polygon Coordinates"));
}

void GridCreatingConditionTriangle::PolygonCoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GridCreatingConditionTriangle::EditPolygonCoordinatesCommand(false, coords2, targetCondition));

	targetCondition->restoreMouseEventMode();
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionTriangle::PolygonCoordinatesEditor::rejectCallback()
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
		targetCondition->m_selectedPolygon->updateShapeData();
		targetCondition->renderGraphicsView();
	}
	targetCondition->restoreMouseEventMode();
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionTriangle::PolygonCoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GridCreatingConditionTriangle::EditPolygonCoordinatesCommand(true, coords2, targetCondition));

	applied = true;
}

std::vector<QPointF> GridCreatingConditionTriangle::PolygonCoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetCondition->offset());
}

std::vector<QPointF> GridCreatingConditionTriangle::PolygonCoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetCondition->offset());
}
