#include "gridcreatingconditionpoisson_coordinateseditor.h"
#include "gridcreatingconditionpoisson_editcoordinatescommand.h"
#include "gridcreatingconditionpoisson_impl.h"
#include "../gridcreatingconditionpoisson.h"

#include <guibase/coordinates/coordinatesedit.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/iricundostack.h>

#include <QString>

PolyLineController* GridCreatingConditionPoisson::CoordinatesEditor::targetLine = nullptr;
GridCreatingConditionPoisson* GridCreatingConditionPoisson::CoordinatesEditor::targetCondition = nullptr;
bool GridCreatingConditionPoisson::CoordinatesEditor::applied = false;
QString GridCreatingConditionPoisson::CoordinatesEditor::lineName = "";

void GridCreatingConditionPoisson::CoordinatesEditor::edit(PolyLineController* line, const QString& name, GridCreatingConditionPoisson * condition)
{
	condition->impl->m_mouseEventMode = Impl::MouseEventMode::EditCoodinatesDialog;
	condition->iricMainWindow()->enterModelessDialogMode();

	targetLine = line;
	targetCondition = condition;
	lineName = name;
	applied = false;

	auto coords = applyOffset(line->polyLine());
	CoordinatesEdit::edit(coords, condition->preProcessorWindow(),
												acceptCallback,
												rejectCallback,
												applyCallback,
												GridCreatingConditionPoisson::tr("%1 Coordinates").arg(name));
}

void GridCreatingConditionPoisson::CoordinatesEditor::acceptCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	targetCondition->pushUpdateLabelsCommand(new GridCreatingConditionPoisson::EditCoordinatesCommand(lineName, coords2, targetLine));

	targetCondition->restoreMouseEventMode();
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionPoisson::CoordinatesEditor::rejectCallback()
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
		targetCondition->renderGraphicsView();
	}
	targetCondition->restoreMouseEventMode();
	targetCondition->iricMainWindow()->exitModelessDialogMode();
}

void GridCreatingConditionPoisson::CoordinatesEditor::applyCallback(const std::vector<QPointF>& coords)
{
	if (applied) {
		// undo the older action
		iRICUndoStack::instance().undo();
	}
	auto coords2 = unapplyOffset(coords);
	targetCondition->pushUpdateLabelsCommand(new GridCreatingConditionPoisson::EditCoordinatesCommand(lineName, coords2, targetLine), true);

	applied = true;
}

std::vector<QPointF> GridCreatingConditionPoisson::CoordinatesEditor::applyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::applyOffset(coords, targetCondition->offset());
}

std::vector<QPointF> GridCreatingConditionPoisson::CoordinatesEditor::unapplyOffset(const std::vector<QPointF>& coords)
{
	return CoordinatesEdit::unapplyOffset(coords, targetCondition->offset());
}
