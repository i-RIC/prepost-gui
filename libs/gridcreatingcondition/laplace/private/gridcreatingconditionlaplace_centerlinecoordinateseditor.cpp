#include "gridcreatingconditionlaplace_centerlinecoordinateseditor.h"
#include "gridcreatingconditionlaplace_impl.h"
#include "../gridcreatingconditionlaplace.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <geoio/coordinatesedit.h>
#include <misc/iricundostack.h>

#include <QString>

PolyLineController* GridCreatingConditionLaplace::CenterLineCoordinatesEditor::targetLine = nullptr;
GridCreatingConditionLaplace* GridCreatingConditionLaplace::CenterLineCoordinatesEditor::targetCondition = nullptr;
bool GridCreatingConditionLaplace::CenterLineCoordinatesEditor::applied = false;

void GridCreatingConditionLaplace::CenterLineCoordinatesEditor::edit(PolyLineController* line, GridCreatingConditionLaplace * condition)
{
	condition->impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::EditCoodinatesDialog;
	condition->iricMainWindow()->enterModelessDialogMode();

	targetLine = line;
	targetCondition = condition;
	applied = false;

	auto coords = applyOffset(line->polyLine());
	CoordinatesEdit::edit(coords, condition->preProcessorWindow(),
												acceptCallback,
												rejectCallback,
												applyCallback,
												GridCreatingConditionLaplace::tr("Center Line Coordinates"));
}

void GridCreatingConditionLaplace::CenterLineCoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	targetCondition->impl->pushCenterLineEditCoordinatesCommand(coords2);

	targetCondition->impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Normal;
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionLaplace::CenterLineCoordinatesEditor::rejectCallback()
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
		targetCondition->renderGraphicsView();
	}
	targetCondition->impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Normal;
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionLaplace::CenterLineCoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	targetCondition->impl->pushCenterLineEditCoordinatesCommand(coords2);

	applied = true;
}

std::vector<QPointF> GridCreatingConditionLaplace::CenterLineCoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetCondition->offset());
}

std::vector<QPointF> GridCreatingConditionLaplace::CenterLineCoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetCondition->offset());
}
