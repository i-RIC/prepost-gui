#include "gridcreatingconditioncompoundchannel_polygoncoordinateseditor.h"
#include "gridcreatingconditioncompoundchannel_editpolygoncoordinatescommand.h"
#include "../gridcreatingconditioncompoundchannel.h"
#include "../gridcreatingconditioncompoundchannelabstractpolygon.h"

#include <guibase/coordinates/coordinatesedit.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>

GridCreatingConditionCompoundChannel* GridCreatingConditionCompoundChannel::PolygonCoordinatesEditor::targetCondition = 0;
bool GridCreatingConditionCompoundChannel::PolygonCoordinatesEditor::applied = false;

void GridCreatingConditionCompoundChannel::PolygonCoordinatesEditor::edit(GridCreatingConditionCompoundChannel * condition)
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
												GridCreatingConditionCompoundChannel::tr("Polygon Coordinates"));
}

void GridCreatingConditionCompoundChannel::PolygonCoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GridCreatingConditionCompoundChannel::EditPolygonCoordinatesCommand(false, coords2, targetCondition->m_selectedPolygon, targetCondition));

	targetCondition->restoreMouseEventMode();
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionCompoundChannel::PolygonCoordinatesEditor::rejectCallback()
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

void GridCreatingConditionCompoundChannel::PolygonCoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GridCreatingConditionCompoundChannel::EditPolygonCoordinatesCommand(true, coords2, targetCondition->m_selectedPolygon, targetCondition));

	applied = true;
}

std::vector<QPointF> GridCreatingConditionCompoundChannel::PolygonCoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetCondition->offset());
}

std::vector<QPointF> GridCreatingConditionCompoundChannel::PolygonCoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetCondition->offset());
}
