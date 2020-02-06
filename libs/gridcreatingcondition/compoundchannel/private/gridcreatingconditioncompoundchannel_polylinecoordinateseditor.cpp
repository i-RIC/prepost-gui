#include "gridcreatingconditioncompoundchannel_polylinecoordinateseditor.h"
#include "gridcreatingconditioncompoundchannel_editpolylinecoordinatescommand.h"
#include "../gridcreatingconditioncompoundchannel.h"
#include "../gridcreatingconditioncompoundchannelabstractline.h"

#include <guibase/coordinates/coordinatesedit.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>

GridCreatingConditionCompoundChannel* GridCreatingConditionCompoundChannel::PolylineCoordinatesEditor::targetCondition = 0;
bool GridCreatingConditionCompoundChannel::PolylineCoordinatesEditor::applied = false;

void GridCreatingConditionCompoundChannel::PolylineCoordinatesEditor::edit(GridCreatingConditionCompoundChannel * condition)
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
												GridCreatingConditionCompoundChannel::tr("Polyline Coordinates"));
}

void GridCreatingConditionCompoundChannel::PolylineCoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GridCreatingConditionCompoundChannel::EditPolyLineCoordinatesCommand(false, coords2, targetCondition->m_selectedLine, targetCondition));

	targetCondition->restoreMouseEventMode();
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionCompoundChannel::PolylineCoordinatesEditor::rejectCallback()
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

void GridCreatingConditionCompoundChannel::PolylineCoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	iRICUndoStack::instance().push(new GridCreatingConditionCompoundChannel::EditPolyLineCoordinatesCommand(true, coords2, targetCondition->m_selectedLine, targetCondition));

	applied = true;
}

std::vector<QPointF> GridCreatingConditionCompoundChannel::PolylineCoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetCondition->offset());
}

std::vector<QPointF> GridCreatingConditionCompoundChannel::PolylineCoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetCondition->offset());
}
